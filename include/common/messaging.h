#ifndef MESSAGING_H
#define MESSAGING_H

#include "protocol.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>

extern const char* queue_path;

int send_notification(int msqid, long to, int notif_type, int msgflg);
int receive_notification(int msqid, long from, int* notif_type, int msgflg);
int send_transaction(int msqid, long to, void* transaction, size_t size, int msgflg);
int receive_transaction(int msqid, long from, void* transaction, size_t size, int msgflg);

int save_message_to_history(char* fullmessage, char* history, int max_history_size);
#endif//MESSAGING_H