#ifndef CLIENT_TEXT_MESSAGING_H
#define CLIENT_TEXT_MESSAGING_H

#include "client_messaging.h"

int send_text_message(char* text, int text_len, int msgflg);
int get_text_message(char** text, int msgflg);


#endif//CLIENT_TEXT_MESSAGING_H