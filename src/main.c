#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ALF_std.h"

#include <time.h>


#define msgSize 1024

void getHttpDate(char *dst_http_date, size_t maxsize){
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(dst_http_date, maxsize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

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

typedef enum{
    GET, // The GET method requests a representation of the specified resource. Requests using GET should only retrieve data and should have no other effect. (This is also true of some other HTTP methods.)[1] The W3C has published guidance principles on this distinction, saying, "Web application design should be informed by the above principles, but also by the relevant limitations."[22] See safe methods below.
    HEAD, // The HEAD method asks for a response identical to that of a GET request, but without the response body. This is useful for retrieving meta-information written in response headers, without having to transport the entire content.
    // POST, // The POST method requests that the server accept the entity enclosed in the request as a new subordinate of the web resource identified by the URI. The data POSTed might be, for example, an annotation for existing resources; a message for a bulletin board, newsgroup, mailing list, or comment thread; a block of data that is the result of submitting a web form to a data-handling process; or an item to add to a database.[23]
    // PUT, // The PUT method requests that the enclosed entity be stored under the supplied URI. If the URI refers to an already existing resource, it is modified; if the URI does not point to an existing resource, then the server can create the resource with that URI.[24]
    // DELETE, // The DELETE method deletes the specified resource.
    // TRACE, // The TRACE method echoes the received request so that a client can see what (if any) changes or additions have been made by intermediate servers.
    // OPTIONS, // The OPTIONS method returns the HTTP methods that the server supports for the specified URL. This can be used to check the functionality of a web server by requesting '*' instead of a specific resource.
    // CONNECT, // [25] The CONNECT method converts the request connection to a transparent TCP/IP tunnel, usually to facilitate SSL-encrypted communication (HTTPS) through an unencrypted HTTP proxy.[26][27] See HTTP CONNECT method.
    // PATCH, // The PATCH method applies partial modifications to a resource.[28]GET
} ReqMethodEnum;

typedef struct{
    ReqMethodEnum req_type;
    char *route;
    long http_ver;
    long http_ver_minor;
} ReqLine;

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

int main(int argc, char **argv){
    printf("%s\n", argv[argc-argc]);

    ALF_socket *server = init_server(NULL, 8888);
    if(server == NULL){
        printf("%s\n", ALF_sockets_getLastErrorMsg());
        return ALF_sockets_getLastError();
    }

    while(true){
        printf("Waiting connections...\n");

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
