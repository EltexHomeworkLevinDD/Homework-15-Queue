#ifndef CLIENT_MESSAGING_H
#define CLIENT_MESSAGING_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif//_GNU_SOURCE

#define ETRACE_CLIENT_MESSAGING // Объявите, чтобы отслеживать пути ошибок во всех функциях

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#ifndef EXITMSG
#define EXITMSG(msg) do { \
        perror(msg); \
        exit(EXIT_FAILURE); } while(0)
#endif//EXITMSG

#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "../../msgtypes.h"

#define CONTROL_QUEUE_RELATIVE_PATH "../server/control_queue"
#define TEXT_QUEUE_RELATIVE_PATH    "../server/text_queue"
#define MAX_OWN_NAME_SIZE 32 
// MAX_OWN_NAME_SIZE должен умещаться в MAX_CONTENT_SIZE из msgtypes.h
// и оставлять достаточно места под char запись long числа
// для MAX_OWN_NAME_SIZE=32 и MAX_OWN_NAME_SIZE=64 в MAX_OWN_NAME_SIZE остается 32 под запись long числа

#define ECMSG_SUCCESS            0      // Успех
#define ECMSG_NOMSG             -1      // Нет сообщений
#define ECMSG_SIGNIT            -2      // Выполнение прервано сигналом
#define ECMSG_CRITICAL          -3      // Критическая ошибка, взаимодействие с очередью невозможно
#define ECMSG_QUEUE_EXISTS      -4      // Очередь сущеуствует для данного ключа

// Ключи очередей
extern key_t control_queue_key;
extern key_t text_queue_key;
// msqid очередей
extern int control_queue_msqid;
extern int text_queue_msqid;
// Приоритеты (направления) передачи сообщений
extern long PTS; // priority to server - На сервер
extern long PFS; // priority from server - От сервера клиенту
// Флаг установления соединения (0 если не установлено)
extern int flg_connection_established;

int check_msgrcv();
int check_msgsnd();

#endif//CLIENT_MESSAGING_H