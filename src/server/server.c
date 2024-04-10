#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../../include/server/threads.h"

int main() {
    // Создаём файл для ключа очереди (с удалением, если он есть)
    FILE *file;
    // Открываем файл в режиме добавления с чтением
    file = fopen(queue_path, "a+");
    if (file == NULL) {
        printf("fopen(queue_path) error\n");
        return -1;
    }
    // Закрываем файл перед удалением, если он открыт
    fclose(file);
    // Удаляем файл, если он существует
    if (remove(queue_path) != 0) {
        printf("remove(queue_path)\n");
        return -1;
    }
    // Создаем новый файл
    file = fopen(queue_path, "w");
    if (file == NULL) {
        printf("fopen(queue_path), creation\n");
        return -1;
    }
    // Закрываем файл после создания
    fclose(file);

    // Получаем ключ очереди
    key_t queue_key = ftok(queue_path, 0);  
    if (queue_key == (key_t)(-1)) {
        perror("ftok() for control_queue_key in main()");
        return -1;
    }
    printf("Key: %d\n", (int)queue_key);


    // Создаю очередь
    int msqid = msgget(queue_key, IPC_CREAT | 0666);
    if (msqid == -1){
        perror("msgget() for msqid in main()");
        return -1;
    }
    printf("msqid: %d\n", msqid);

    // Инициализировать структуры
    struct List users_list;
    initialize_list(&users_list);
    char history[MAX_HISTORY_SZ];
    memset(history, '\0', MAX_HISTORY_SZ);

    // Создать поток сервера
    pthread_t serverthread_tid;
    int* serverthread_status;
    struct ServerThreadArgs serverthread_args = {
        .history = history,
        .msqid = msqid,
        .users_list = &users_list
    };
    if (pthread_create(&serverthread_tid, NULL, serverthread, (void*)&serverthread_args) != 0){
        perror("Creating serverthread()");
        freeList(&users_list);
        return -1;
    }

    char ch;
    printf("Kill server? (any sumbol): ");
    ch = fgetc(stdin);
    (void)ch;
    terminate_flag = 1;

    // Ждать завершение потока
    pthread_join(serverthread_tid, (void**)&serverthread_status);
    if (serverthread_status != NULL){
        freeList(&users_list);
        perror("serverthread() has broken");
        return -1;
    }

    // Ждём 100 милисекунд
    usleep(100000);

    // Удаляем очередь сообщений
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return -1;
    }

    freeList(&users_list);

    // Проверить существование файла
    if (access(queue_path, F_OK) != -1) {
        // Файл существует, удаляем его
        if (remove(queue_path) != 0) {
            perror("Error deleting file");
            return -1;
        }
    } else {
        // Файл не существует
    }

    return 0;
}