#ifndef THREADS_H
#define THREADS_H

#include "transiver.h"
#include "graphics.h"
#include <pthread.h>
#include <unistd.h>

struct UplinkArgs {
    long PTS;
    int msqid;
    const char* cmd_exit;
    const char* msg_promt;
    char* own_name;    
    Window* wnd_command;
};

struct DownlinkArgs {
    long PFS;
    int msqid;
    pthread_t uplink_tid;
    char* history;
    char* own_name; 
    struct List* users_list;
    Window* wnd_command;
    Window* wnd_chat;
    Window* wnd_list;
};

// uplink uplink завершится мягко, если будет введена команда выхода, или жёстко, из downlink (возможно только в случае отключения сервера)

void* uplink(void* uplink_args);

// downlink завершится если примет NT_USER_DISCONNECTED и имя в транзакции совпадает с его менем

void* downlink(void* downlink_args);
extern volatile int uplink_is_dead;

#endif//THREADS_H