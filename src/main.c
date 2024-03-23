#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "submain.h"

int main(){

    pid_t cpid = fork();
    // --> Child
    if (cpid == 0){
        // Присоединение к очереди
        char* fullpath = get_full_path(QUEUE_FILE);
        key_t key = ftok((const char*)fullpath, QUEUE_PROJ_ID);     if (key == -1) EXITMSG("child ftok");
        int id = msgget(key, 0);                                    if (id == -1) EXITMSG("child msgget create queue");

        // Блокирующее чтение сообщения
        get_msg(id, 0, "Child receive: ");
        // Блокирующая отправка сообщения
        send_msg(id, 1, "Child send: ", "Goodbye, world!");
        free(fullpath);
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
        char* fullpath = get_full_path(QUEUE_FILE);
        key_t key = ftok((const char*)fullpath, QUEUE_PROJ_ID);     if (key == -1) EXITMSG("parent ftok");
        int id = msgget(key, IPC_CREAT | 0600);                     if (id == -1) EXITMSG("parent msgget create queue");
        
        // Блокирующая отправка сообщения
        send_msg(id, 1, "Parent send: ", "Hello, world!");
        // Блокирующее чтение сообщения
        get_msg(id, 0, "Parent receive ");
        free(fullpath);
        if (msgctl(id, IPC_RMID, NULL) == -1) EXITMSG("msgctl delete queue");
        return 0;
    }
}