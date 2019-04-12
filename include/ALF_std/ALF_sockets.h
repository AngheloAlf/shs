/** \file ALF_sockets.h
* A wrapper for easy use of sockets.
**/

#ifndef ALF_sockets_h
#define ALF_sockets_h

#include "ALF_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#else
#include <sys/socket.h>
#endif

#define ALF_SOCKETS_TYPE_TCP SOCK_STREAM
#define ALF_SOCKETS_TYPE_UDP SOCK_DGRAM

typedef struct {
    int sock_desc;
    struct sockaddr_in *addr;

    int backlog;
    size_t maxRecv;

    bool binded;
    bool listening;
    bool connected;

    int type;
} ALF_socket;

/// Generates the socket object.
/**
 * Params:

 * - type: Indicates the type of the socket. Must be ALF_SOCKETS_TYPE_TCP or ALF_SOCKETS_TYPE_UDP

 * - ip: A string containing the ip of the socket.

 * - port: The port of the socket.

 * Return value:

 - = NULL: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().

 - Otherwhise: The object.

**/
ALF_socket *ALF_sockets_init(int type, const char *ip, uint16_t port);

/// Alternative object. Usefull with UDP connections.
/**
 * Return value:

 - = NULL: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().

 - Otherwhise: The object.
**/
ALF_socket *ALF_sockets_createDestObj(int type);

/// Object destroyer.
void ALF_sockets_free(ALF_socket *handler);

/// Backlog setter.
void ALF_sockets_setBacklog(ALF_socket *handler, int backlog);

/// Backlog getter.
int ALF_sockets_getBacklog(ALF_socket *handler);

/// TCP client connect to server.
/*
 * Return value:

 - = 0: OK.

 - < 0: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().
**/
int ALF_sockets_connect(ALF_socket *handler);

/// Server binder.
/*
 * Return value:

 - = 0: OK.

 - < 0: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().
**/
int ALF_sockets_bind(ALF_socket *handler);

/// TCP Server listener. Needs to be binded.
/*
 * Return value:

 - = 0: OK.

 - < 0: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().
**/
int ALF_sockets_listen(ALF_socket *handler);

/// TCP Server. Accepts a connection.
/*
 * Return value:

 - = 0: OK.

 - < 0: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().
**/
ALF_socket *ALF_sockets_accept(ALF_socket *handler);

/// Recive a message from the socket. It only returns when a message is recived.
/**
 * Params:

 * - _from: The socket from where we are reciving the message.

 * - msg: A char pointer already allocated to store the message.

 * - maxRecv: The max ammount of character to be recived. The msg param should be able to store them. This does not include the null character.

 * - _to: The main server. Is only usefull in UDP connections.

 * Return value:

 - >= 0: Characters recived.

 - < 0: Error.
**/
ssize_t ALF_sockets_recv(ALF_socket *_from, char *msg, size_t maxRecv, ALF_socket *_to);

/// Recive a message from the socket. If there is no message aviable, then no message is setted.
/**
 * Params:

 * - _from: The socket from where we are reciving the message.

 * - msg: A char pointer already allocated to store the message.

 * - maxRecv: The max ammount of character to be recived. The msg param should be able to store them. This does not include the null character.

 * - _to: The main server. Is only usefull in UDP connections.

 * Return value:

 - > 0: Characters recived.

 - = 0: No message recived.

 - < 0: Error.
**/
ssize_t ALF_sockets_recvNonBlocking(ALF_socket *_from, char *msg, size_t maxRecv, ALF_socket *_to);

/// Send a message to the socket.
/**
 * Params:

 * - _to: The socket we are sending the message.

 * - msg: The message to be send.

 * - msgSize: The amount of character to be sended. If zero, will be calculated using strlen().

 * - _from: The main server. Is only usefull in UDP connections.

 * Return value:

 - >= 0: Characters send.

 - < 0: Error. See ALF_sockets_getLastError() and ALF_sockets_getLastErrorMsg().
**/
ssize_t ALF_sockets_send(ALF_socket *_to, const char* msg, size_t msgSize, ALF_socket *_from);

/// Returns the last error code.
int ALF_sockets_getLastError();
/// Returns the last error message.
const char *ALF_sockets_getLastErrorMsg();

#endif /* ALF_sockets_h */
