#include "submain.h"

void get_msg(int qid, int msgtype, const char* prefix)
{
    struct mymsgbuf msg;

    if (msgrcv(qid, &msg, MTEXT_MAX_SIZE, msgtype, MSG_NOERROR) == -1) {
        if (errno != ENOMSG) EXITMSG("msgsnd error");
        printf("No message available for msgrcv()\n");
    }

    if (prefix == NULL){
        printf("message received: '%s'\n", msg.mtext);
    } 
    else {
        printf("%s'%s'\n", prefix, msg.mtext);
    }
}

void send_msg(int qid, int msgtype, const char* prefix, const char* text)
{
    struct mymsgbuf  msg;

    msg.mtype = msgtype;
    strncpy(msg.mtext, text, MTEXT_MAX_SIZE);
    msg.mtext[MTEXT_MAX_SIZE-1] = '\0';

    if (msgsnd(qid, &msg, MTEXT_MAX_SIZE, 0) == -1) EXITMSG("msgsnd error");

    if (prefix == NULL){
        printf("sent: '%s'\n", msg.mtext);
    } 
    else {
        printf("%s'%s'\n", prefix, msg.mtext);
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