#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "ALF_std.h"

#include "packet_parser.h"
#include "SocketThread.h"
#include "util.h"

// https://stackoverflow.com/a/4217052/6292472
static volatile bool shs_running = true;
void intHandler(int _sig_){
    fprintf(stderr, "\nCatched signal %i\n", _sig_);
    shs_running = false;
}


ALF_socket *init_server(const char *ip_addr, uint16_t port){
    printf("Starting server in port %u.\n", port);
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
    if(bytes_readed <= 0){
        return bytes_readed;
    }
    total_readed += bytes_readed;

    char *wea = strstr(*dst_msg, "\r\n\r\n");
    bool msg_ready = false;
    unsigned long content_length;
    while(!msg_ready){
        if(wea == NULL){
            *dst_msg = realloc(*dst_msg, sizeof(char)*((*msg_size)*2+1));
            if(*dst_msg == NULL){
                return -1;
            }
            bytes_readed = ALF_sockets_recv(client, &(*dst_msg)[*msg_size], *msg_size, NULL);
            if(bytes_readed <= 0){
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
                    if(*dst_msg == NULL){
                        return -1;
                    }
                    bytes_readed = ALF_sockets_recv(client, &(*dst_msg)[*msg_size], *msg_size, NULL);
                    if(bytes_readed <= 0){
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

void *handle_client(void *client_pointer){
    size_t msg_size = MSG_SIZE;
    SocketThread *client_thread = (SocketThread *)client_pointer;
    ALF_socket *client = client_thread->socket_;

    char *msg = malloc(sizeof(char)*(msg_size+1));
    bool valid = true;
    ssize_t bytes_readed;
    while(valid && (bytes_readed = recv_request_packet(client, &msg, &msg_size)) > 0){
        // printf("recv<< %s\n", msg);
        valid = parse_request_packet(client, msg);
    }
    free(msg);
    printf("Connection end.\n");
    ALF_sockets_free(client);
    client_thread->has_thread_end = true;

    return NULL; //What should I return?
}

void chk_args(int argc, char **argv){
    if(argc < 2){
        printf("Usage: %s port\n", argv[0]);
        exit(-1);
    }
}

int main(int argc, char **argv){
    chk_args(argc, argv);
    unsigned int port_number;
    if(sscanf(argv[1], "%u", &port_number) != 1){
        fprintf(stderr, "Could't read port number.\n");
        exit(-1);
    }
    // signal(SIGINT, intHandler);

    ALF_socket *server = init_server(NULL, port_number);
    if(server == NULL){
        fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
        return ALF_sockets_getLastError();
    }

    VectorSocketThread *clients_vector = VectorSocketThread_init(16);
    printf("Waiting connections...\n\n");
    while(shs_running){
        ALF_socket *client = ALF_sockets_accept(server);
        if(client == NULL){
            fprintf(stderr, "%s\n", ALF_sockets_getLastErrorMsg());
            printf("Exiting...");
            ALF_sockets_free(server);
            return ALF_sockets_getLastError();
        }
        printf("Client accepted.\n");

        SocketThread *client_thread = SocketThread_init(client);
        if(pthread_create(client_thread->thread_id, NULL, handle_client, client_thread) != 0){
            fprintf(stderr, "Error creating thread.\n");
            ALF_sockets_free(client);
            SocketThread_free(client_thread);
        }
        VectorSocketThread_endadd(clients_vector, client_thread);

        long still_alive = VectorSocketThread_remove_notrunning_threads(clients_vector);
        printf("Still alive threads: %li\n\n", still_alive);
    }
    printf("Exiting...");

    VectorSocketThread_clean(clients_vector);
    VectorSocketThread_free(clients_vector);

    ALF_sockets_free(server);

    return 0;
}
