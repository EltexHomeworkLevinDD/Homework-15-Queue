#ifndef CLIENT_CONTROL_MESSAGING_H
#define CLIENT_CONTROL_MESSAGING_H

#include "client_messaging.h"

int send_control_message(struct ControlMessage* msg, int msgflg);
int get_control_message(struct ControlMessage* msg, long priority, int msgflg);
int send_break_connect(int msgflg);
int get_history(char** history, int msgsndflg, int msgrcvflg);
int get_users_list(char*** list, int msgsndflg, int msgrcvflg);
int establish_connection(char* name, int name_size, 
                        char** history, int* history_size, 
                        char*** list, int* list_size,
                        int msgsndflg, int msgrcvflg);

#endif//CLIENT_CONTROL_MESSAGING_H