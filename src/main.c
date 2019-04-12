#include <stdio.h>
#include <stdlib.h>
#include "ALF_std.h"

#include <time.h>


#define msgSize 1024

#define __chk_socket_error(condition) do {if(condition){\
        printf("%s\n", ALF_sockets_getLastErrorMsg());\
        return ALF_sockets_getLastError();\
    }} while(0);

void getHttpDate(char *dst_http_date, size_t maxsize){
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(dst_http_date, maxsize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

int main(int argc, char **argv){
    ALF_socket *server = ALF_sockets_init(ALF_SOCKETS_TYPE_TCP, NULL, 80);
    __chk_socket_error(server == NULL)
    printf("Socket created.\n");

    __chk_socket_error(ALF_sockets_bind(server))
    printf("Bind done.\n");

    __chk_socket_error(ALF_sockets_listen(server))
    printf("Listen done.\n");

    printf("Waiting connections...\n");

    ALF_socket *client = ALF_sockets_accept(server);
    __chk_socket_error(client == NULL)
    printf("Client accepted.\n");

    char msg[msgSize + 1];
    char date[msgSize + 1];

    ssize_t asd = ALF_sockets_recv(client, msg, msgSize, NULL);
    while(asd > 0){
        // fprintf(stderr, "recv: %s\n", msg);

        getHttpDate(date, msgSize);
        sprintf(msg, "HTTP/1.1 200 OK\nDate: %s\nServer: shhs\nContent-Type: text/html;charset=UTF-8\nContent-Length: %i\n\n%s\n", date, 13, "Hello, World!");
        // printf("send: %s\n", msg);
        asd = ALF_sockets_send(client, msg, 0, NULL);
        if(asd < 0){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            break;
        }
        asd = ALF_sockets_recv(client, msg, msgSize, NULL);
        if(asd < 0){
            printf("%s\n", ALF_sockets_getLastErrorMsg());
            break;
        }
    }
    printf("End. Message: %s\n", ALF_sockets_getLastErrorMsg());

    printf("Connection ended.\n");

    ALF_sockets_free(client);
    ALF_sockets_free(server);

    return 0;
}
