#ifndef CLIENT_CONTROL_MESSAGING_H
#define CLIENT_CONTROL_MESSAGING_H

#include "client_messaging.h"

int send_control_message(struct ControlMessage* msg);
int get_control_message(struct ControlMessage* msg, long priority);
int send_break_connect();
int get_history(char** history);
int get_users_list(char*** list);
int establish_connection(char* name, int name_size, 
                        char** history, int* history_size, 
                        char*** list, int* list_size);

#endif//CLIENT_CONTROL_MESSAGING_H