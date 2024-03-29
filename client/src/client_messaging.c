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