#include "../../include/common/messaging.h"

const char* queue_path = "/tmp/queue_key_file";

/*Послать уведомление
Запускает заного, если вызов был прерван сигналом
Возвращает 
- 0 - в случае успеха 
- -1 - в случае критической ошибки */
int send_notification(int msqid, long to, int notif_type, int msgflg){
    struct Notification notif;
    notif.mtype = to;
    notif.type = notif_type;
    while (1){
        if (msgsnd(msqid, &notif, sizeof(int), msgflg) == -1) { 
            switch (errno)
            {
            // Вызов был прерван сигналом, повторить
            case EINTR:
                continue;
                break;
            }
            perror("msgsnd() in send_notification()");
            return -1;
        }
        return 0;
    }
}

/*Послать уведомление
Запускает заного, если вызов был прерван сигналом

Возвращает 
- 0 - в случае успеха 
- 1 - если сообщения нет
- -1 - в случае критической ошибки */
int receive_notification(int msqid, long from, int* notif_type, int msgflg){
    struct Notification notif;
    while (1){
        if (msgrcv(msqid, &notif, sizeof(int), from, msgflg) == -1) {
            switch (errno)
            {
            // Вызов был прерван сигналом, повторить
            case EINTR:
                continue;
                break;
            case ENOMSG:
                return 1;
                break;
            }
            perror("msgsnd() in send_notification()");
            return -1;
        }
        *notif_type = notif.type;
        return 0;
    }
}

/*Послать уведомление
Запускает заного, если вызов был прерван сигналом
- transaction - указатель на транзакцию
- size - размер транзакции (размер только полей, без gap вставок)

Возвращает 
- 0 - в случае успеха 
- -1 - в случае критической ошибки */
int send_transaction(int msqid, long to, void* transaction, size_t size, int msgflg) {
    long* first_field = (long*)transaction;
    *first_field = to;
    while (1){
        if (msgsnd(msqid, transaction, size, msgflg) == -1) {
            switch (errno)
            {
            // Вызов был прерван сигналом, повторить
            case EINTR:
                continue;
                break;
            }
            perror("msgsnd() in send_notification()");
            return -1;
        }
        return 0;
    }
}

/*Послать уведомление
Запускает заного, если вызов был прерван сигналом
- transaction - пустой указатель на получаемую транзакцию
- size - размер транзакции (размер только полей, без gap вставок)

Возвращает 
- 0 - в случае успеха 
- 1 - если сообщения нет
- -1 - в случае критической ошибки */
int receive_transaction(int msqid, long from, void* transaction, size_t size, int msgflg){
    while (1){
        if (msgrcv(msqid, transaction, size, from, msgflg) == -1) {
            switch (errno)
            {
            // Вызов был прерван сигналом, повторить
            case EINTR:
                continue;
                break;
            case ENOMSG:
                return 1;
                break;
            }
            perror("msgsnd() in send_notification()");
            return -1;
        }
        return 0;
    }
}

int save_message_to_history(char* fullmessage, char* history, int max_history_size) {
    int len = strlen(fullmessage);
    int history_size = strlen(history);
    
    // Если сообщение полностью занимает историю, затираем последний символ и сдвигаем
    if (history_size + len > max_history_size) {
        memmove(history + len, history, max_history_size - len);
        strncpy(history, fullmessage, len);
    } else {
        // Сдвигаем историю вправо на len символов
        memmove(history + len, history, history_size + 1); // +1 для '\0'
        strncpy(history, fullmessage, len);
    }

    history[max_history_size - 1] = '\0';

    return 0;
}