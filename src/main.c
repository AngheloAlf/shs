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


int main(int argc, char **argv){
    printf("%s\n", argv[argc-argc]);

    ALF_socket *server = init_server(NULL, 8888);
    if(server == NULL){
        printf("%s\n", ALF_sockets_getLastErrorMsg());
        return ALF_sockets_getLastError();
    }

    while(true){
        printf("Waiting connections...\n\n");

        ALF_socket *client = ALF_sockets_accept(server);
        if(client == NULL){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            return ALF_sockets_getLastError();
        }
        printf("Client accepted.\n");

        char msg[msgSize + 1];

        ssize_t asd;
        bool valid;
        while((asd = ALF_sockets_recv(client, msg, msgSize, NULL)) > 0){
            valid = parse_request_packet(client, msg);
            if(!valid){
                break;
            }
            // printf("recv<< %s\n", msg);
        }
        printf("End. Message: %s\n", ALF_sockets_getLastErrorMsg());
        printf("Connection ended.\n");
        ALF_sockets_free(client);
    }
    ALF_sockets_free(server);

    printf("Exiting...");

    return 0;
}
