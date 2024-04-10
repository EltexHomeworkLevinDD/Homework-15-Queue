#ifndef TRANSIVER_H
#define TRANSIVER_H

#define TRACE_MSG

#include "../common/userslist.h"
#include "../common/messaging.h"

/*Возвращает 
- msqid - Идентификатор очереди
- users_list - указатель на список пользоваталей (пользователь будет добавлен в конец)
- history - указатель на историю сообщений

- 0 - Если процедура завершена
- 1 - Если сообщения нет
- -1 - Если возникла критическая ошибка*/
int process_procedure_connect(int msqid, struct List* users_list, char* history);
int process_procedure_user_connected(int msqid, char* name, struct List* users_list);
int process_procedure_new_message_from_user(int msqid, long from, struct List* users_list, char* history);
int process_procedure_user_disconnected(int msqid, long from, struct List* users_list);

#endif//TRANSIVER_H