#ifndef TRANSIVER_H
#define TRANSIVER_H

//#define TRACE_MSG

#include "../common/userslist.h"
#include "../common/messaging.h"


int create_message(char* text, char* own_name, char** fullmessage);
//  Установление соединения
int process_msg_procedure_connect(int msqid, char* name, struct List* users_list, char* history, long* PTS, long* PFS);
//  Отправка
int process_msg_procedure_new_message_to_server(int msqid, char* simple_message, char* your_name, long to);
int process_msg_procedure_i_am_disconnected(int msqid, long to);
//  Получение (Использовать после получения уведомления)
int process_msg_procedure_new_message_from_server(int msqid, long from, char* history);
int process_msg_procedure_user_connected(int msqid, long from, struct List* users_list);
int process_msg_procedure_user_disconnected(int msqid, long from, struct List* users_list, char* your_name);
#endif//TRANSIVER_H