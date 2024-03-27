#ifndef C_MESSAGING_H
#define C_MESSAGING_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif//_GNU_SOURCE

#ifndef EXITMSG
#define EXITMSG(msg) do { \
        perror(msg); \
        exit(EXIT_FAILURE); } while(0)
#endif//EXITMSG

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "../msgtypes.h"

#define CONTROL_QUEUE_RELATIVE_PATH "../server/control_queue"
#define TEXT_QUEUE_RELATIVE_PATH    "../server/text_queue"
#define MAX_OWN_NAME_SIZE 32 
// MAX_OWN_NAME_SIZE должен умещаться в MAX_CONTENT_SIZE из msgtypes.h
// и оставлять достаточно места под char запись long числа
// для MAX_OWN_NAME_SIZE=32 в MAX_OWN_NAME_SIZE остается 32 под запись long числа

// Ключи очередей
extern key_t control_queue_key;
extern key_t text_queue_key;
// msqid очередей
extern int control_queue_msqid;
extern int text__queue_msqid;
// Указатели на динамические массивы полных путей очередей
extern char* full_control_queue_path;
extern char* full_text_queue_path;
// Приоритеты (направления) передачи сообщений
extern long PTS; // priority to server - На сервер
extern long PFS; // priority from server - От сервера клиенту

/*
Передать запрос установления соединения
- name - Имя клиента
- name_size - Размер имени клиента

После выполнения инициализируются глобальные
- control_queue_key
- text_queue_key
- control_queue_msqid
- text__queue_msqid
- full_control_queue_path
- full_text_queue_path
- PTS
- PFS

Возвращает указатель на динамический массив char, размером
MAX_HISTORY_SIZE, содержащий историю сообщений
*/
char* startup(char* name, int name_size);
void send_break_connect();
char* get_canonic_path(const char* relative_path);

static void send_setup_connect_request(char* name, int name_size);

#endif//C_MESSAGING_H