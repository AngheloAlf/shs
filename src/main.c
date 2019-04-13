#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ALF_std.h"

#include "packet_parser.h"
#include "util.h"


ALF_socket *init_server(const char *ip_addr, uint16_t port){
    ALF_socket *server = ALF_sockets_init(ALF_SOCKETS_TYPE_TCP, ip_addr, port);
    if(server == NULL){
        return NULL;
    }
    printf("Socket created.\n");

    if(ALF_sockets_bind(server)){
        ALF_sockets_free(server);
        return NULL;
    }
    printf("Bind done.\n");

    if(ALF_sockets_listen(server)){
        ALF_sockets_free(server);
        return NULL;
    }
    printf("Listen done.\n");

    return server;
}

ssize_t recv_request_packet(ALF_socket *client, char **dst_msg, size_t *msg_size){
    ssize_t total_readed = 0;
    ssize_t bytes_readed = ALF_sockets_recv(client, *dst_msg, *msg_size, NULL);
    if(bytes_readed < 0){
        return bytes_readed;
    }
    total_readed += bytes_readed;

    char *wea = strstr(*dst_msg, "\r\n\r\n");
    bool msg_ready = false;
    unsigned long content_length;
    while(!msg_ready){
        if(wea == NULL){
            *dst_msg = realloc(*dst_msg, sizeof(char)*((*msg_size)*2+1));
            bytes_readed = ALF_sockets_recv(client, &(*dst_msg)[*msg_size], *msg_size, NULL);
            if(bytes_readed < 0){
                return bytes_readed;
            }
            total_readed += bytes_readed;
            *msg_size *= 2;
            wea = strstr(*dst_msg, "\r\n\r\n");
        }
        else{
            char *other_wea = strstr(*dst_msg, "Content-Length: ");
            if(other_wea == NULL){
                msg_ready = true;
            }
            else{
                sscanf(other_wea + strlen("Content-Length: "), "%lu", &content_length);
                if(strlen(wea + 4) == content_length){
                    msg_ready = true;
                }
                else{
                    *dst_msg = realloc(*dst_msg, sizeof(char)*((*msg_size)*2+1));
                    bytes_readed = ALF_sockets_recv(client, &(*dst_msg)[*msg_size], *msg_size, NULL);
                    if(bytes_readed < 0){
                        return bytes_readed;
                    }
                    total_readed += bytes_readed;
                    *msg_size *= 2;
                }
            }
        }
    }

    return total_readed;
}

int main(int argc, char **argv){
    printf("%s\n", argv[argc-argc]);

    ALF_socket *server = init_server(NULL, 8080);
    if(server == NULL){
        printf("%s\n", ALF_sockets_getLastErrorMsg());
        return ALF_sockets_getLastError();
    }

    size_t msg_size = msgSize;
    while(true){
        printf("Waiting connections...\n\n");

        ALF_socket *client = ALF_sockets_accept(server);
        if(client == NULL){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            return ALF_sockets_getLastError();
        }
        printf("Client accepted.\n");

        char *msg = malloc(sizeof(char)*(msg_size+1));
        bool valid = true;
        ssize_t bytes_readed;
        while((bytes_readed = recv_request_packet(client, &msg, &msg_size)) > 0 && valid){
            // printf("recv<< %s\n", msg);
            valid = parse_request_packet(client, msg);
        }
        free(msg);
        printf("End. Message: %s\n", ALF_sockets_getLastErrorMsg());
        printf("Connection ended.\n");
        ALF_sockets_free(client);
    }
    ALF_sockets_free(server);

    printf("Exiting...");

    return 0;
}
