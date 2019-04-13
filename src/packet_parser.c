#include "packet_parser.h"

#include <string.h>


#include "util.h"


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

bool send_response_packet(ALF_socket *client){
    char msg[msgSize + 1];
    char date[msgSize + 1];

    getHttpDate(date, msgSize);
    sprintf(msg, "HTTP/1.1 200 OK\nDate: %s\r\nServer: shs 1.0\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: %i\r\n\r\n%s\r\n", date, 13, "Hello, World!");
    // printf("send>> %s\n", msg);
    ssize_t asd = ALF_sockets_send(client, msg, 0, NULL);
    if(asd < 0){
        printf("%s\n", ALF_sockets_getLastErrorMsg());
        return false;
    }
    return true;
}

bool parse_request_packet(ALF_socket *client, char *msg_packet){
    // printf("recv<< %s\n", msg_packet);
    char **msg_arr = ALF_STR_split(msg_packet, "\r\n");
    ReqLine req_line;
    bool valid = parse_req_line(&req_line, msg_arr[0]);
    if(!valid){
        printf("not valid\n");
        ALF_STR_freeSplitted(msg_arr);
        return valid;
    }
    char *req_par = msg_arr[1];
    long i;
    for(i = 1; req_par != NULL; req_par = msg_arr[++i]){
        if(strcmp(req_par, "") == 0){
            break;
        }
        printf("recv << %s\n", req_par);
    }
    valid = send_response_packet(client);
    ALF_STR_freeSplitted(msg_arr);
    return valid;
}

