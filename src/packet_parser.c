#include "packet_parser.h"

#include <stdio.h>
#include <string.h>


#include "util.h"



bool send_response_packet(ALF_socket *client, Request *req){
    ssize_t chars_send;
    char date[msgSize + 1];
    char msg[msgSize + 1];
    char *res_header = "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: shs 1.0\r\nContent-Type: text/html;charset=UTF-8\r\n";
    getHttpDate(date, msgSize);

    sprintf(msg, res_header, date);
    chars_send = ALF_sockets_send(client, msg, 0, NULL);
    if(chars_send < 0){
        printf("%s\n", ALF_sockets_getLastErrorMsg());
        return false;
    }
    
    if(req->request->req_type == HEAD){
        chars_send = ALF_sockets_send(client, "\r\n", 0, NULL);
        if(chars_send < 0){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            return false;
        }
    }
    else if(req->request->req_type == GET){
        // Content-Length: %i\r\n\r\n%s\r\n", 13, "Hello, World!"
        sprintf(msg, "Content-Length: %i\r\n\r\n%s\r\n", 13, "Hello, World!");
        chars_send = ALF_sockets_send(client, msg, 0, NULL);
        if(chars_send < 0){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            return false;
        }
    }
    else{
        printf("how?\n");
        return false;
    }
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
    bool valid = parse_req_line(&req_line, msg_arr[0]);
    if(!valid){
        printf("not valid\n");
        ALF_STR_freeSplitted(msg_arr);
        return valid;
    }

    printf("%s\n", msg_arr[0]);

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
            printf("Error\n");
            printf("recv << %s\n", req_par);
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
        // printf("recv << %s\n", req_par);
    }
    printf("\n");

    // printf("%s\n", msg_packet);

    Request req;
    req.request = &req_line;
    req.req_args = headers;

    valid = keep_alive && send_response_packet(client, &req);

    StringDict_free(headers);
    ALF_STR_freeSplitted(msg_arr);
    return valid;
}

