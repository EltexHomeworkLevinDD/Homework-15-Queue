#include "c_messaging.h"

key_t control_queue_key = -1;
key_t text_queue_key = -1;
int control_queue_msqid = -1;
int text_queue_msqid = -1;
char* full_control_queue_path = NULL;
char* full_text_queue_path = NULL;

char* get_canonic_path(const char* relative_path){
    // Получаю каноничный путь из относительного
    char* canonical_dest_path = canonicalize_file_name(relative_path);
    if (canonical_dest_path == NULL) {
        perror("canonicalize_file_name [get_canonic_path()]");
        exit(EXIT_FAILURE);
    }
    return canonical_dest_path;
}

char* send_setup_connect_request(char* name, int name_size) {
    // Получаю каноничные пути
    full_control_queue_path  = get_canonic_path(CONTROL_QUEUE_RELATIVE_PATH);
    full_text_queue_path  = get_canonic_path(TEXT_QUEUE_RELATIVE_PATH);
    // Получаю ключи для обеих очередей
    control_queue_key = ftok((const char*)full_control_queue_path, 256);    
    if (control_queue_key == (key_t)(-1)) 
        EXITMSG("ftok control_queue_key [send_setup_connect_request()]");

    text_queue_key = ftok((const char*)full_control_queue_path, 256);       
    if (text_queue_key == (key_t)(-1)) 
        EXITMSG("ftok text_queue_key [send_setup_connect_request()]");

    // Подключаюсь к очереди управления
    int msqid = msgget(control_queue_key, 0);
    if (text_queue_key == (key_t)(-1)) 
        EXITMSG("msgget control_queue [send_setup_connect_request()]");

    // Формирую сообщение setup_connect_request
    struct ControlMessage ctrl_msg_snd;
    ctrl_msg_snd.mtype = 1; // prio = 1 - на сервер (перед установлением соединения)
    strncpy(ctrl_msg_snd.content, name, name_size);
    ctrl_msg_snd.content[name_size-1] = '\0';

    // Отправляю сообщение на сервер
    if (msgsnd(msqid, &ctrl_msg_snd, sizeof(struct ControlMessage), 0) == -1) 
        EXITMSG("msgsnd ControlMessage [send_setup_connect_request()]");
    
    // Формирую сообщение для получения данных от сервера setup_connect_response
    struct ControlMessage recv_msg_snd;
    recv_msg_snd.mtype = 1;
    strncpy(recv_msg_snd.content, name, name_size);
    recv_msg_snd.content[name_size-1] = '\0';

    // Жду ответное сообщение со своим именем и приоритетом
    // prio = 2 - на сервер (перед установлением соединения)
    if (msgrcv(msqid, &recv_msg_snd, sizeof(struct ControlMessage), 2, MSG_NOERROR) == -1)
        if (errno != ENOMSG) 
            EXITMSG("msgsnd error");
    // Разбиваю сообщение на имя и приоритет
    char received_name[MAX_OWN_NAME_SIZE];
    // Получаю имя
    char* token = strtok(recv_msg_snd.content, "|");
    if (token != NULL){
        strncpy(received_name, token, MAX_OWN_NAME_SIZE);
        received_name[strlen(token)] = '\0';
    } else if (token == NULL) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("strtok name [send_setup_connect_request()]");
    }

    // Получаю приоритет в строковом виде
    char received_cprio[MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE];
    token = strtok(NULL, "|");
    if (token != NULL){
        strncpy(received_cprio, token, MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE);
        received_cprio[strlen(token)] = '\0';
    } else if (token == NULL) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("strtok prioriry [send_setup_connect_request()]");
    }
    // Получаю приоритет в long виде
    PTS = atol((const char*)received_cprio);
    if (PTS == 0) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("atoi PTS [send_setup_connect_request()]");
    }
    PFS = PTS + 1;

    // Формирую сообщение для запроса получения истории сообщений
    struct ControlHistory hist;
    hist.mtype = PTS;
    hist.hist[0] = '\0';

}

void send_break_connect(){
    if (full_control_queue_path != NULL)
        free(full_control_queue_path);
    if (full_text_queue_path != NULL)
        free(full_text_queue_path);
}