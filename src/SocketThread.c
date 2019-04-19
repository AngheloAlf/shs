#include "SocketThread.h"

#include <stdlib.h>

SocketThread *SocketThread_init(ALF_socket *socket){
    SocketThread *socket_thread = malloc(sizeof(SocketThread));
    socket_thread->socket_ = socket;
    socket_thread->has_thread_end = false;
    socket_thread->thread_id = malloc(sizeof(pthread_t));
    return socket_thread;
}

void SocketThread_free(SocketThread *socket_thread){
    free(socket_thread->thread_id);
    free(socket_thread);
}


long VectorSocketThread_remove_notrunning_threads(VectorSocketThread *vector){
    long still_alive = 0;
    for(long i = VectorSocketThread_len(vector) - 1; i >= 0; --i){
        SocketThread *item = VectorSocketThread_item(vector, i);
        if(item->has_thread_end){
            SocketThread_free(item);
            VectorSocketThread_pop(vector, i);
        }
        else{
            ++still_alive;
        }
    }
    return still_alive;
}

long VectorSocketThread_clean(VectorSocketThread *vector){
    long still_alive = 0;
    for(long i = VectorSocketThread_len(vector) - 1; i >= 0; --i){
        SocketThread *item = VectorSocketThread_item(vector, i);
        if(!item->has_thread_end){
            ++still_alive;
            ALF_sockets_free(item->socket_);
        }
        SocketThread_free(item);
        VectorSocketThread_pop(vector, i);
    }
    return still_alive;

}
