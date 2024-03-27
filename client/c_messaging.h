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
extern int text_queue_msqid;
// Указатели на динамические массивы полных путей очередей
extern char* full_control_queue_path;
extern char* full_text_queue_path;
// Приоритеты (направления) передачи сообщений
extern long PTS; // priority to server - На сервер
extern long PFS; // priority from server - От сервера клиенту
// Флаг установления соединения (0 если не установлено)
extern int flg_connection_established;

// ============ Control plane ============
/*
Установить соединение и получить историю сообщений
- name - Имя клиента
- name_size - Размер имени клиента
- history - Пустой указатель на историю сообщений

После выполнения инициализируются глобальные
- control_queue_key
- text_queue_key
- control_queue_msqid
- text__queue_msqid
- full_control_queue_path
- full_text_queue_path
- PTS
- PFS

Возвращает размер allocated массива history, 
освобождайте его самостоятельно

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect() и освободить 
- full_text_queue_path
- full_control_queue_path
*/
size_t establish_connection(char* name, int name_size, char** history);
/*
Получить историю сообщений
- history - Пустой указатель на историю сообщений

Возвращает размер allocated массива history в байтах,
освобождайте его самостоятельно
*/
size_t get_history(char** history);
/*
Послать сообщение разрыва соединения
Освобождает 
- full_text_queue_path
- full_control_queue_path

И устанавливает flg_connection_established = 0
*/
void send_break_connect();

// ============ Text plane ============
/*
Отправить сообщение на сервер
- text - сообщение
- text_len - длина сообщения
*/
void send_text_message(char* text, int text_len);
/*
Получить сообщение
- text - пустой указатель на сообщение

Возвращает размер allocated массива text в байтах,
освобождайте его самостоятельно

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect() и освободить 
- full_text_queue_path
- full_control_queue_path
*/
size_t get_text_message(char** text);
// ============ Other ============

// Получить канонический полный путь из относительного пути
char* get_canonic_path(const char* relative_path);

#endif//C_MESSAGING_H