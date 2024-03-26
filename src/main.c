#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mqueue.h>
#include "submain.h"

int main(){

    pid_t cpid = fork();
    char name[] = "/myqueue";
    // --> Child
    if (cpid == 0){
        // Присоединение к очереди
        mqd_t des = mq_open(name, O_RDWR);
        if (des == ((mqd_t)-1)) EXITMSG("mq_open connection");
        // Блокирующее чтение сообщения
        get_msg(des, "Child receive: ");
        // Блокирующая отправка сообщения
        send_msg(des, "Child send: ", "Goodbye, world!");
        mq_unlink(name);
        exit(EXIT_SUCCESS);
    // Child <--
    }
    // Error
    else if (cpid == -1){
        perror("Fork");
        return 0;
    }
    // Parent -->
    else {
        // Инициализировать очередь
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_curmsgs = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = MTEXT_MAX_SIZE;
        mqd_t des = mq_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
        if (des == ((mqd_t)-1)) EXITMSG("mq_open creation");
        
        // Блокирующая отправка сообщения
        send_msg(des, "Parent send: ", "Hello, world!");
        int status;
        wait(&status);
        // Блокирующее чтение сообщения
        get_msg(des, "Parent receive ");
        mq_close(des);
        return 0;
    }
}