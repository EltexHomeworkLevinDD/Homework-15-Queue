#ifndef SUBMAIN_H
#define SUBMAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_FILE "src/main.c"
#define QUEUE_PROJ_ID 256
#define MTEXT_MAX_SIZE 20

#define EXITMSG(msg) do { \
        perror(msg); \
        exit(EXIT_FAILURE); } while(0)

struct mymsgbuf{
    long mtype;
    char mtext[MTEXT_MAX_SIZE];
};

void get_msg(mqd_t id, const char* prefix);
void send_msg(mqd_t id, const char* prefix, const char* text);
char* get_full_path(const char* path);

#endif//SUBMAIN_H