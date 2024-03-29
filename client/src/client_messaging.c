#include "client_messaging.h"

key_t control_queue_key = -1;
key_t text_queue_key = -1;
int control_queue_msqid = -1;
int text_queue_msqid = -1;
char* full_control_queue_path = NULL;
char* full_text_queue_path = NULL;
int flg_connection_established = 0;

long PTS; 
long PFS;

/*Проверить функцию msgrcv
Возвращает 
- ECMSG_SUCCESS
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
int check_msgrcv(){
    switch (errno)
    {
        case ENOMSG:
            // Сообщения нет
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Сообщения нет in get_control_message()");
            #endif
            return ECMSG_NOMSG;
        case EIDRM:
            // Идентификатор удалён
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Идентификатор удалён in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        case EINTR:
            // Системный вызов был прерван сигналом, пробуем еще раз
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Вызов был прерван сигналом in get_control_message()");
            #endif
            return ECMSG_SIGNIT;
        case EACCES:
            // Нет разрешения на доступ к очереди сообщений
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Нет разрешения на доступ к очереди in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        case EFAULT:
            // Указан неправильный адрес сообщения
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Неправильный адрес in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        default:
            // Другие неожиданные ошибки
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Неожиданная ошибка in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
    }
    return ECMSG_SUCCESS;
}

/*Проверить функию msgsnd
Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
int check_msgsnd(){
    switch (errno) {
        case EACCES:
            // Нет разрешения на доступ к очереди сообщений
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Нет разрешения на доступ к очереди in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EAGAIN:
            // Сообщение не может быть отправлено из-за ограничения msg_qbytes для очереди
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Сообщение не может быть отправлено in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EFAULT:
            // Указанный адрес недоступен
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Неправильный адрес in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EIDRM:
            // Идентификатор очереди удален
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Идентификатор очереди удален in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EINTR:
            // Системный вызов прерван сигналом, пробуем еще раз
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Вызов прерван сигналом in send_control_message()");
            #endif
            return ECMSG_SIGNIT;
            break;
        case EINVAL:
            // Ошибка в аргументах функции
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Ошибка в аргументах функции in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case ENOMEM:
            // Недостаточно памяти для создания копии сообщения
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Недостаточно памяти in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        default:
            // Другие неожиданные ошибки
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Неожиданная ошибка in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
    }
    return ECMSG_SUCCESS;
}