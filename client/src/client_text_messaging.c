#include "client_text_messaging.h"
/*
Отправить сообщение на сервер
- text - сообщение
- text_len - длина сообщения

Возвращает
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае критической ошибки
послать send_break_connect()
*/
int send_text_message(char* text, int text_len, int msgflg){
    // Формирую сообщение для отправки
    struct TextMessage msg;
    msg.mtype = PTS;
    strncpy(msg.text, text, text_len);
    msg.text[text_len] = '\0';

    // Отправляю сообщение на сервер
    msgsnd(text_queue_msqid, &msg, sizeof(struct TextMessage), msgflg);
    int res = check_msgsnd();
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgsnd() for msg in send_text_message()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}
/*
Получить сообщение с сервера
- text - пустой указатель на сообщение

Возвращает размер allocated массива text в байтах,
освобождайте его самостоятельно

Возвращает
- Размер полученного сообщения
- 0 - если сообщение пустое
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()
*/
int get_text_message(char** text, int msgflg){
    // Формирую сообщение для отправки
    struct TextMessage msg;

    msgrcv(text_queue_msqid, &msg, sizeof(struct TextMessage), PFS, MSG_NOERROR | msgflg);
    int res = check_msgrcv();
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgsnd() for msg in send_text_message()");
        #endif
        return res;
    }
    // Если сообщение отсутствует, возвращаем нулевой размер
    *text = NULL;
    return 0;

    if (msg.text_size > 0) {
        // Выделяю память под текстовое сообщение
        *text = malloc(msg.text_size);
        if (*text == NULL) {
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("malloc() for text in get_text_message()");
            #endif
            return ECMSG_CRITICAL;
        }
        // Копирую текстовое сообщение
        memcpy(*text, msg.text, msg.text_size);
        (*text)[msg.text_size-1] = '\0';
    } else {
        *text = NULL;
        return 0;
    }

    return msg.text_size;
}