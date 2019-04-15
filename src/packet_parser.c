#include "packet_parser.h"

#include <stdio.h>
#include <string.h>


#include "util.h"

#define RESPONSE_PACKET "HTTP/1.1 %li %s\r\nDate: %s\r\nServer: shs 1.0\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: %li\r\n\r\n%s\r\n"

char *default_get_response(Request *req, unsigned long *response_size){
    FILE *html_file;
    if(req->request->route[1] == 0){
        html_file = fopen("index.html", "r");
    }
    else{
        html_file = fopen(&req->request->route[1], "r");
    }

    if(html_file == NULL){
        return NULL;
    }

    fseek(html_file, 0L, SEEK_END);
    *response_size = ftell(html_file);
    fseek(html_file, 0L, SEEK_SET);

    char *response = malloc(sizeof(char)*(*response_size));

    if(fread(response, 1, *response_size, html_file) != *response_size){
        free(response);
        fclose(html_file);
        return NULL;
    }

    fclose(html_file);
    return response;
}

bool send_status_and_body(ALF_socket *client, long status_code, const char *status_msg, long body_size, const char *body){
    char date[MSG_SIZE + 1];
    getHttpDate(date, MSG_SIZE);
    char *msg = malloc(sizeof(char) * (strlen(RESPONSE_PACKET) + 4 + strlen(status_msg) + strlen(date) + 22 + body_size + 1));
    sprintf(msg, RESPONSE_PACKET, status_code, status_msg, date, body_size, body);
    printf(">> %li %s\n", status_code, status_msg);
    ssize_t chars_send = ALF_sockets_send(client, msg, 0, NULL);
    free(msg);
    if(chars_send < 0){
        fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
        return false;
    }
    return true;
}

bool send_response_packet(ALF_socket *client, Request *req){
    char *response = NULL;
    unsigned long res_size;
    switch(req->request->req_type){
        case HEAD:
            response = default_get_response(req, &res_size);
            if(response != NULL && res_size > 0){
                if(!send_status_and_body(client, 200, "OK", 0, "")){
                    fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
                    free(response);
                    return false;
                }
            }
            else{
                if(!send_status_and_body(client, 404, "Not Found", 0, "")){
                    fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
                    free(response);
                    return false;
                }
            }
            break;
        case GET:
            response = default_get_response(req, &res_size);
            if(response != NULL && res_size > 0){
                if(!send_status_and_body(client, 200, "OK", res_size, response)){
                    fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
                    free(response);
                    return false;
                }
            }
            else{
                if(!send_status_and_body(client, 404, "Not Found", 0, "")){
                    fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
                    free(response);
                    return false;
                }
            }
            break;
        default:
            fprintf(stderr, "how?\n");
            if(!send_status_and_body(client, 500, "Internal Server Error", 0, "")){
                fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
            }
            free(response);
            return false;
    }
    free(response);
    return true;
}

bool parse_req_line(ReqLine *dst_req, const char *src_line){
    char **splitted = ALF_STR_split(src_line, " ");

    const char *method = splitted[0];
    if(method == NULL){
        ALF_STR_freeSplitted(splitted);
        return false;
    }
    if(strcmp(method, "GET") == 0){
        dst_req->req_type = GET;
    }
    else if(strcmp(method, "HEAD") == 0){
        dst_req->req_type = HEAD;
    }
    else{
        fprintf(stderr, "> Warning. The method '%s' is not implemented (yet?). Defaulting to 'GET'.\n", method);
        dst_req->req_type = GET;
    }

    const char *route = splitted[1];
    if(route == NULL){
        ALF_STR_freeSplitted(splitted);
        return false;
    }
    dst_req->route = strdup(route);

    const char *http_ver = splitted[2];
    if(http_ver == NULL || strncmp(http_ver, "HTTP/", 5) != 0){
        free(dst_req->route);
        ALF_STR_freeSplitted(splitted);
        return false;
    }
    if(sscanf(&http_ver[5], "%li.%li", &dst_req->http_ver, &dst_req->http_ver_minor) != 2){
        free(dst_req->route);
        ALF_STR_freeSplitted(splitted);
        return false;
    }

    ALF_STR_freeSplitted(splitted);
    return true;
}

bool parse_request_packet(ALF_socket *client, char *msg_packet){
    char **msg_arr = ALF_STR_split(msg_packet, "\r\n");

    ReqLine req_line;
    printf("<< %s\n", msg_arr[0]);
    bool valid = parse_req_line(&req_line, msg_arr[0]);
    if(!valid){
        fprintf(stderr, "not valid\n");
        ALF_STR_freeSplitted(msg_arr);
        return valid;
    }

    StringDict *headers = StringDict_init();
    char *req_par = msg_arr[1];
    long i;
    bool keep_alive = true;
    for(i = 1; req_par != NULL; req_par = msg_arr[++i]){
        if(strcmp(req_par, "") == 0){
            break;
        }
        char **header_pair = ALF_STR_split(req_par, ": ");
        if(header_pair[0] == NULL || header_pair[1] == NULL){
            // Error ?
            fprintf(stderr, "Error\n");
            fprintf(stderr, "recv << %s\n", req_par);
            continue;
        }
        if(strcicmp(header_pair[0], "Connection") == 0){
            if(strcicmp(header_pair[1], "close") == 0){
                keep_alive = false;
            }
            else if(strcicmp(header_pair[1], "keep-alive") == 0){
                keep_alive = true;
            }
        }
        StringDict_add(headers, strdup(header_pair[0]), strdup(header_pair[1]));
        ALF_STR_freeSplitted(header_pair);
        // printf("recv << %s\n", req_par);
    }

    // printf("%s\n", msg_packet);

    Request req;
    req.request = &req_line;
    req.req_args = headers;

    valid = keep_alive && send_response_packet(client, &req);

    free(req_line.route);
    for(StringDict_iter it=StringDict_begin(headers); !StringDict_iter_done(&it); StringDict_iter_next(&it)){
        free(StringDict_iter_key(&it));
        free(StringDict_iter_val(&it));
    }

    StringDict_free(headers);
    ALF_STR_freeSplitted(msg_arr);
    printf("\n");
    return valid;
}

