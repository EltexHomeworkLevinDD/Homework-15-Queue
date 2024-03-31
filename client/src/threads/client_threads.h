#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* control_thread();
void* text_thread();

#endif//CLIENT_THREADS_H