#include "submain.h"

void get_msg(mqd_t des, const char* prefix)
{
    char* msg = malloc(sizeof(char)*MTEXT_MAX_SIZE);
    if (msg == NULL) EXITMSG("Malloc, get_msg()");

    unsigned int prio;
    if (mq_receive(des, msg, MTEXT_MAX_SIZE, &prio) == -1){
        free(msg);
        EXITMSG("mq_receive");
    }
    else {
        printf("%s'%s'\n", prefix, msg);
        free(msg);
    }
}

void send_msg(mqd_t des, const char* prefix, const char* text)
{
    if (mq_send(des, text, MTEXT_MAX_SIZE, 1) == -1){
        EXITMSG("mq_send");
    }
    else{
        printf("%s'%s'\n", prefix, text);
    }
}

char* get_full_path(const char* path){
    // Получить каноничный полный путь к директории с бинарником (без ../)
    char* canonical_dest_path = canonicalize_file_name(path);
    if (canonical_dest_path == NULL) {
        perror("canonicalize_file_name");
        exit(EXIT_FAILURE);
    }
    return canonical_dest_path;
}