#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <pthread.h>
#include <stdbool.h>

#include "ALF_std/ALF_sockets.h"
#include "cadts_vector.h"

typedef struct{
    ALF_socket *socket_;
    volatile bool has_thread_end;
    pthread_t *thread_id;
} SocketThread;

SocketThread *SocketThread_init(ALF_socket *socket);
void SocketThread_free(SocketThread *socket_thread);


CADTS_VECTOR(VectorSocketThread, SocketThread*)

long VectorSocketThread_remove_notrunning_threads(VectorSocketThread *vector);
long VectorSocketThread_clean(VectorSocketThread *vector);


#endif /* SOCKET_THREAD_H */
