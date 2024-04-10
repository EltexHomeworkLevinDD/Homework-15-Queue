#ifndef THREADS_H
#define THREADS_H

#include "transiver.h"
#include <pthread.h>

struct ServerThreadArgs {
    int msqid;
    struct List* users_list;
    char* history;
};

// Флаг завершения потока
extern volatile int terminate_flag;

void* serverthread(void* serverthread_args);

#endif