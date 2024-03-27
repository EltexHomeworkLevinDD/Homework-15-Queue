#include "c_messaging.h"

key_t control_queue_key = -1;
key_t text_queue_key = -1;
int control_queue_msqid = -1;
int text_queue_msqid = -1;
char* full_control_queue_path = NULL;
char* full_text_queue_path = NULL;
int flg_connection_established = 0;

void send_break_connect();

// Возвращает NULL в случае провала
char* get_canonic_path(const char* relative_path){
    // Получаю каноничный путь из относительного
    char* canonical_dest_path = canonicalize_file_name(relative_path);
    if (canonical_dest_path == NULL) {
        return NULL;
    }
    return canonical_dest_path;
}

/*
Подключается к очередям и инициализируюет глобальные
- control_queue_key
- text_queue_key
- control_queue_msqid
- text__queue_msqid
- full_control_queue_path
- full_text_queue_path
*/
static void connect_all_queue(){
    // Получаю каноничные пути
    full_control_queue_path  = get_canonic_path(CONTROL_QUEUE_RELATIVE_PATH);
    if (full_control_queue_path == NULL)
        EXITMSG("get_canonic_path for full_control_queue_path [connect_all_queue()]");
    full_text_queue_path  = get_canonic_path(TEXT_QUEUE_RELATIVE_PATH);
    if (full_text_queue_path == NULL)
        EXITMSG("get_canonic_path for full_text_queue_path [connect_all_queue()]");
    // Получаю ключи для обеих очередей
    control_queue_key = ftok((const char*)full_control_queue_path, 256);    
    if (control_queue_key == (key_t)(-1)) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("ftok control_queue_key [connect_all_queue()]");
    }

    text_queue_key = ftok((const char*)full_text_queue_path, 256);       
    if (text_queue_key == (key_t)(-1)) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("ftok text_queue_key [connect_all_queue()]");
    }

    // Подключаюсь к очереди управления
    control_queue_msqid = msgget(control_queue_key, 0);
    if (control_queue_msqid == -1) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("msgget control_queue_msqid [connect_all_queue()]");
    }

    // Подключаюсь к очереди текстовых сообщений
    text_queue_msqid = msgget(text_queue_key, 0);
    if (text_queue_msqid == -1) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        EXITMSG("msgget text_queue_msqid [connect_all_queue()]");
    }
}

// Отправить запрос соедениния
static void send_setup_connect_request(char* name, int name_size){
    // Формирую сообщение setup_connect_request
    struct ControlMessage msg;
    msg.mtype = 1; // prio = 1 - отправить на сервер (используется до установления соединения)
    msg.message = CM_SETUP_CONNECT_REQEST;
    strncpy(msg.content, name, name_size);
    msg.content[name_size-1] = '\0';
    
    // Отправляю сообщение на сервер
    if (msgsnd(control_queue_msqid, &msg, sizeof(struct ControlMessage), 0) == -1) {
        // Критическая ошибка
        EXITMSG("msgsnd CM_SETUP_CONNECT_REQEST [send_setup_connect_request()]");
    }
}

// Получить ответ сервера
// - name - собественное имя клиента
// - name_len - длина собственного имени клиента
// 
// Иницаализирует глобальные
// - PTS
// - PFS
// Возвращает 0 в случае ошибки, 1 в случае успеха
static int get_setup_connect_response(){
    // Формирую сообщение для получения данных от сервера setup_connect_response
    struct ControlMessage msg;

    // Жду сообщение со своим именем и приоритетом ("Name|Prio")
    // prio = 2 - получить от сервера (используется перед установлением соединения)
    if (msgrcv(control_queue_msqid, &msg, sizeof(struct ControlMessage), 2, MSG_NOERROR) == -1) {
        // Критическая ошибка
        if (errno != ENOMSG) {
            free(full_text_queue_path);
            free(full_control_queue_path);
            EXITMSG("msgrcv CM_SETUP_CONNECT_RESPONSE [get_setup_connect_response()]");
        }
        // Если сообщения нет
        return 0;
    }

    // Разбиваю сообщение на имя и приоритет
    char received_name[MAX_OWN_NAME_SIZE];
    // Получаю имя
    char* token = strtok(msg.content, "|");
    if (token != NULL){
        strncpy(received_name, token, MAX_OWN_NAME_SIZE);
        received_name[strlen(token)] = '\0';
    } else if (token == NULL) {
        perror("strtok name [get_setup_connect_response()]");
        return 0;
    }

    // Получаю приоритет в строковом виде
    char received_cprio[MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE];
    token = strtok(NULL, "|");
    if (token != NULL){
        strncpy(received_cprio, token, MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE);
        received_cprio[strlen(token)] = '\0';
    } else if (token == NULL) {
        perror("strtok prioriry [get_setup_connect_response()]");
        return 0;
    }
    // Получаю приоритет в long виде
    PTS = atol((const char*)received_cprio);
    if (PTS == 0) {
        EXITMSG("atoi PTS [get_setup_connect_response()]");
    }
    PFS = PTS + 1;
    return 1;
}

// Послать сообщение запроса получения истории сообщений
static void send_get_history_request(){
    // Формирую сообщение для запроса получения истории сообщений
    struct ControlMessage msg;
    msg.mtype = PTS;
    msg.message = CM_GET_HISTORY_REQEST;
    msg.content[0] = '\0';

    // Отправляю сообщение на сервер
    if (msgsnd(control_queue_msqid, &msg, sizeof(struct ControlMessage), 0) == -1){
        send_break_connect();
        // Критическая ошибка
        EXITMSG("msgsnd CM_GET_HISTORY_REQEST [send_get_history_request()]");
    }
}

// Получить ответ сервара, содержадий историю сообщений
// Принимает пустой указатель на историю сообщений 
// Allocate *history array, (освобождайте его самостоятельно)
// Возвращает размер массива истории сообщений в байтах
// history вернёт NULL, если истории нет
static size_t get_history_response(char** history){
    // Формирую сообщение для получения истории сообщений
    struct ControlHistory history_msg;

    // Жду ответное сообщение CM_GET_HISTORY_RESPONSE
    // prio = 2 - получить от сервера (используется перед установлением соединения)
    if (msgrcv(control_queue_msqid, &history_msg, sizeof(struct ControlHistory), PFS, MSG_NOERROR) == -1) {
        if (errno != ENOMSG) {
            send_break_connect();
            EXITMSG("msgrcv CM_GET_HISTORY_RESPONSE [get_history_response()]");
        }
        send_break_connect();
        // Если сообщение отсутствует, возвращаем нулевой размер
        *history = NULL;
        return 0;
    }

    if (history_msg.hist_size > 0) {
        // Выделяю память под историю сообщений
        *history = malloc(history_msg.hist_size);
        if (*history == NULL) {
            // Критическая ошибка
            free(full_text_queue_path);
            free(full_control_queue_path);
            EXITMSG("malloc history pointer [get_history_response()]");
        }
        // Копирую историю сообщений
        memcpy(*history, history_msg.hist, history_msg.hist_size);
        (*history)[history_msg.hist_size-1] = '\0';
    } else{
        *history = NULL;
        return 0;
    }

    return history_msg.hist_size;
}

size_t get_history(char** history){
    // Отправить запрос истории
    send_get_history_request();
    // Получить ответ на запрос истории сообщений
    *history = NULL;
    size_t history_size = get_history_response(&(*history));
    if ((*history) == NULL || history_size == 0){
        send_break_connect();
        EXITMSG("get_history_response, [establish_connection()]");
    }
    return history_size;
}

size_t establish_connection(char* name, int name_size, char** history) {
    
    // Подключиться к обеим очередям
    connect_all_queue();
    // Отправить запрос на установление соединения
    send_setup_connect_request(name, name_size);
    // Получить ответ установления соединения
    if (get_setup_connect_response(name, name_size) == 0){
        send_get_history_request();
        EXITMSG("get_setup_connect_response, [establish_connection()]");
    }
    // Отправить запрос истории
    send_get_history_request();
    // Получить ответ на запрос истории сообщений
    *history = NULL;
    size_t history_size = get_history_response(&(*history));
    if ((*history) == NULL || history_size == 0){
        send_get_history_request();
        EXITMSG("get_history_response, [establish_connection()]");
    }

    flg_connection_established = 1;

    return history_size;
}

void send_break_connect(){
    // Формирую сообщение для отправки
    struct ControlMessage msg;
    msg.mtype = PTS;
    msg.message = CM_BREAK_CONNECT;
    msg.content[0] = '\0';

    // Отправляю сообщение на сервер
    if (msgsnd(control_queue_msqid, &msg, sizeof(struct ControlMessage), 0) == -1){
        free(full_text_queue_path);
        free(full_control_queue_path);
        flg_connection_established = 0;
        // Критическая ошибка
        EXITMSG("msgsnd CM_BREAK_CONNECT [send_break_connect()]");
    }

    if (full_control_queue_path != NULL && full_text_queue_path != NULL){
        free(full_control_queue_path);
        free(full_text_queue_path);
    }
    flg_connection_established = 0;
}

void send_text_message(char* text, int text_len){
    // Формирую сообщение для отправки
    struct TextMessage msg;
    msg.mtype = PTS;
    strncpy(msg.text, text, text_len);
    msg.text[text_len] = '\0';

    // Отправляю сообщение на сервер
    if (msgsnd(text_queue_msqid, &msg, sizeof(struct TextMessage), 0) == -1){
        send_break_connect();
        // Критическая ошибка
        EXITMSG("msgsnd [send_text_message()]");
    }
}

size_t get_text_message(char** text){
    // Формирую сообщение для отправки
    struct TextMessage msg;

    if (msgrcv(text_queue_msqid, &msg, sizeof(struct TextMessage), PFS, MSG_NOERROR) == -1) {
        if (errno != ENOMSG) {
            send_break_connect();
            EXITMSG("msgrcv [get_text_message()]");
        }
        // Если сообщение отсутствует, возвращаем нулевой размер
        *text = NULL;
        return 0;
    }

    if (msg.text_size > 0) {
        // Выделяю память под текстовое сообщение
        *text = malloc(msg.text_size);
        if (*text == NULL) {
            // Критическая ошибка
            send_break_connect();
            EXITMSG("malloc history pointer [get_history_response()]");
        }
        // Копирую текстовое сообщение
        memcpy(*text, msg.text, msg.text_size);
        (*text)[msg.text_size-1] = '\0';
    } else{
        *text = NULL;
        return 0;
    }

    return msg.text_size;
}
