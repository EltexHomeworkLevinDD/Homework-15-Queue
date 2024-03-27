#ifndef C_THREADS_H
#define C_THREADS_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "c_messaging.h"

void* control_thread();
void* text_thread();

#endif