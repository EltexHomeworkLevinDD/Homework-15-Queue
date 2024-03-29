#include "client_control_messaging.h"

// Static functions prototypes

static char* get_canonic_path(const char*);
static int check_msgrcv();
static int check_msgsnd();

// Public functions prototypes

int send_control_message(struct ControlMessage* msg);
int get_control_message(struct ControlMessage* msg, long priority);

//////////////////////////////////////////////////////////////////////
////////////////////////// STATIC FUNCTIONS //////////////////////////


/*Получить канонический полный путь из относительного
Возвращает 
- Указатель на alloceted массив - В случае успеха 
- NULL - В случае провала*/
static char* get_canonic_path(const char* relative_path){
    // Получаю каноничный путь из относительного
    char* canonical_dest_path = canonicalize_file_name(relative_path);
    if (canonical_dest_path == NULL) {
        return NULL;
    }
    return canonical_dest_path;
}

/*Проверить функцию msgrcv
Возвращает 
- ECMSG_SUCCESS
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int check_msgrcv(){
    switch (errno)
    {
        case ENOMSG:
            // Сообщения нет
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Сообщения нет in get_control_message()");
            #endif
            return ECMSG_NOMSG;
        case EIDRM:
            // Идентификатор удалён
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Идентификатор удалён in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        case EINTR:
            // Системный вызов был прерван сигналом, пробуем еще раз
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Вызов был прерван сигналом in get_control_message()");
            #endif
            return ECMSG_SIGNIT;
        case EACCES:
            // Нет разрешения на доступ к очереди сообщений
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Нет разрешения на доступ к очереди in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        case EFAULT:
            // Указан неправильный адрес сообщения
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Неправильный адрес in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
        default:
            // Другие неожиданные ошибки
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgrcv() Неожиданная ошибка in get_control_message()");
            #endif
            return ECMSG_CRITICAL;
    }
    return ECMSG_SUCCESS;
}

/*Проверить функию msgsnd
Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int check_msgsnd(){
    switch (errno) {
        case EACCES:
            // Нет разрешения на доступ к очереди сообщений
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Нет разрешения на доступ к очереди in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EAGAIN:
            // Сообщение не может быть отправлено из-за ограничения msg_qbytes для очереди
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Сообщение не может быть отправлено in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EFAULT:
            // Указанный адрес недоступен
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Неправильный адрес in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EIDRM:
            // Идентификатор очереди удален
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Идентификатор очереди удален in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case EINTR:
            // Системный вызов прерван сигналом, пробуем еще раз
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Вызов прерван сигналом in send_control_message()");
            #endif
            return ECMSG_SIGNIT;
            break;
        case EINVAL:
            // Ошибка в аргументах функции
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Ошибка в аргументах функции in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        case ENOMEM:
            // Недостаточно памяти для создания копии сообщения
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Недостаточно памяти in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
        default:
            // Другие неожиданные ошибки
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("msgsnd() Неожиданная ошибка in send_control_message()");
            #endif
            return ECMSG_CRITICAL;
            break;
    }
    return ECMSG_SUCCESS;
}


/*Подключиться к очереди
- key - Ключ очереди

Возвращает 
- msqid - В случае успеха
- ECMSG_CRITICAL
- ECMSG_QUEUE_EXISTS
*/
static int connect_queue(key_t key){
    int msqid = msgget(key, 0);
    if (msqid == -1){
        switch (errno) {
            case EACCES:
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("msgget: Нет разрешения на доступ к очереди [connect_all_queue()]");
                #endif
                return ECMSG_CRITICAL;
                break;
            case EEXIST:
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("msgget: Очередь сообщений уже существует для указанного ключа [connect_all_queue()]");
                #endif
                return ECMSG_QUEUE_EXISTS;
                break;
            case ENOMEM:
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("msgget: Недостаточно памяти в системе для создания новой структуры данных [connect_all_queue()]");
                #endif
                return ECMSG_CRITICAL;
                break;
            case ENOSPC:
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("msgget: Превышено максимальное количество очередей сообщений [connect_all_queue()]");
                #endif
                return ECMSG_CRITICAL;
                break;
            default:
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("msgget Неожиданная ошибка [connect_all_queue()]");
                #endif
                return ECMSG_CRITICAL;
                break;
        }
    }
    return msqid;
}

/*
Подключиться ко всем очередям и инициализировать глобальные
- control_queue_key
- text_queue_key
- control_queue_msqid
- text__queue_msqid
- full_control_queue_path
- full_text_queue_path

Возвращвет 
- ECMSG_SUCCESS
- ECMSG_CRITICAL
- ECMSG_QUEUE_EXISTS
*/
static int connect_all_queue(){
    // Получаю каноничные пути
    char* full_control_queue_path  = get_canonic_path(CONTROL_QUEUE_RELATIVE_PATH);
    if (full_control_queue_path == NULL) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_canonic_path() for full_control_queue_path in connect_all_queue()");
        #endif
        return ECMSG_CRITICAL;
    }

    char* full_text_queue_path  = get_canonic_path(TEXT_QUEUE_RELATIVE_PATH);
    if (full_text_queue_path == NULL){
        free(full_control_queue_path);
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_canonic_path() for full_text_queue_path in connect_all_queue()");
        #endif
        return ECMSG_CRITICAL;
    }
    // Получаю ключи для обеих очередей
    control_queue_key = ftok((const char*)full_control_queue_path, 256);    
    if (control_queue_key == (key_t)(-1)) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("ftok() for control_queue_key in connect_all_queue()");
        #endif
        return ECMSG_CRITICAL;
    }

    text_queue_key = ftok((const char*)full_text_queue_path, 256);       
    if (text_queue_key == (key_t)(-1)) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("ftok() for text_queue_key in connect_all_queue()");
        #endif
        return ECMSG_CRITICAL;
    }

    // Подключаюсь к очереди управления
    control_queue_msqid = connect_queue(control_queue_key);
    if (control_queue_msqid == ECMSG_CRITICAL || control_queue_msqid == ECMSG_QUEUE_EXISTS){
        free(full_text_queue_path);
        free(full_control_queue_path);
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("connect_queue() for control_queue_msqid in connect_all_queue()");
        #endif
        return control_queue_msqid;
    }

    // Подключаюсь к очереди текстовых сообщений
    text_queue_msqid = connect_queue(text_queue_key);
    if (text_queue_msqid == ECMSG_CRITICAL || text_queue_msqid == ECMSG_QUEUE_EXISTS) {
        free(full_text_queue_path);
        free(full_control_queue_path);
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("connect_queue() for text_queue_msqid in connect_all_queue()");
        #endif
        return text_queue_msqid;
    }
    return ECMSG_SUCCESS;
}

/*Отправить запрос соедениния CM_SETUP_CONNECT_REQEST
Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int send_setup_connect_request(char* name, int name_size){
    // Формирую сообщение setup_connect_request
    struct ControlMessage msg;
    msg.mtype = 1; // prio = 1 - отправить на сервер (используется до установления соединения)
    msg.message = CM_SETUP_CONNECT_REQEST;
    strncpy(msg.content, name, name_size);
    msg.content[name_size-1] = '\0';
    
    int res = send_control_message(&msg);
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_setup_connect_request()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*Ждать ответа CM_SETUP_CONNECT_RESPONSE
 - name - собественное имя клиента
 
 Иницаализирует глобальные
 - PTS
 - PFS

Возвращает
- ECMSG_SUCCESS
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int get_setup_connect_response(char* name){
    // Формирую сообщение для получения данных от сервера setup_connect_response
    struct ControlMessage msg;

    // Жду сообщение со своим именем и приоритетом ("Name|Prio")
    // prio = 2 - получить от сервера (используется перед установлением соединения)
    int res = get_control_message(&msg, 2);
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_control_message() for msg in get_setup_connect_response()");
        #endif
        return res;
    }

    // Разбиваю сообщение на имя и приоритет
    char received_name[MAX_OWN_NAME_SIZE];
    // Получаю имя
    char* token = strtok(msg.content, "|");
    if (token != NULL){
        strncpy(received_name, token, MAX_OWN_NAME_SIZE);
        received_name[strlen(token)] = '\0';
        // Провераю, совпадает ли полученое имя с собственным
        if (strncmp(name, received_name, strlen(received_name)) != 0){
            #ifdef ETRACE_CLIENT_MESSAGING
                // Если имя не сопало, значит пришло чужое сообщение
                perror("Необработанная ошибка принятия чужого сообщения CM_SETUP_CONNECT_RESPONSE in get_setup_connect_response()");
            #endif
            return ECMSG_CRITICAL;
        }
    } else {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("strtok() for name in get_setup_connect_response()");
        #endif
        return ECMSG_CRITICAL;
    }

    // Получаю приоритет в строковом виде
    char received_cprio[MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE];
    token = strtok(NULL, "|");
    if (token != NULL){
        strncpy(received_cprio, token, MAX_CONTENT_SIZE - MAX_OWN_NAME_SIZE);
        received_cprio[strlen(token)] = '\0';
    } else {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("strtok() for prioriry in get_setup_connect_response()");
        #endif
        return ECMSG_CRITICAL;
    }
    // Получаю приоритет в long виде
    PTS = atol((const char*)received_cprio);
    if (PTS == 0) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("atoi() for PTS in get_setup_connect_response()");
        #endif
        return ECMSG_CRITICAL;
    }
    PFS = PTS + 1;
    return ECMSG_SUCCESS;
}

/*Послать запрос на получение истории сообщений CM_GET_HISTORY_REQEST
Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int send_get_history_request(){
    // Формирую сообщение для запроса получения истории сообщений
    struct ControlMessage msg;
    msg.mtype = PTS;
    msg.message = CM_GET_HISTORY_REQEST;
    msg.content[0] = '\0';

    // Отправляю сообщение на сервер
    int res = send_control_message(&msg);
    if (res != ECMSG_SUCCESS){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_get_history_request()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*Получить ответ сервара, содержадий историю сообщений CM_GET_HISTORY_RESPONSE
Принимает 
- пустой указатель на историю сообщений и allocate him, освобождайте самостоятельно

Возвращает
- Возвращает размер массива истории сообщений в байтах, 
- 0 - если истории нет
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int get_history_response(char** history){
    // Формирую сообщение для получения истории сообщений
    struct ControlHistory history_msg;

    // Жду ответное сообщение CM_GET_HISTORY_RESPONSE
    // prio = 2 - получить от сервера (используется перед установлением соединения)
    *history =  NULL;
    msgrcv(control_queue_msqid, &history_msg, sizeof(struct ControlHistory), PFS, MSG_NOERROR);
    int res = check_msgrcv();
    if (res != ECMSG_SUCCESS) {
        *history =  NULL;
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgrcv() for history_msg in get_history_response()");
        #endif
        return res;
    }

    if (history_msg.hist_size > 0) {
        // Выделяю память под историю сообщений
        *history = malloc(history_msg.hist_size);
        if (*history == NULL) {
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("malloc() for history in get_history_response()");
            #endif
            return ECMSG_CRITICAL;
        }
        // Копирую историю сообщений
        memcpy(*history, history_msg.hist, history_msg.hist_size);
        (*history)[history_msg.hist_size-1] = '\0';
    } else{
        // Не ошибка
        *history = NULL;
        return 0;
    }

    return history_msg.hist_size;
}

/*Послать запрос на получение списка пользователей CM_GET_USERS_LIST_REQUEST
Возвращает
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int send_get_users_list_request(){
    // Формирую сообщение для запроса получения истории сообщений
    struct ControlMessage msg;
    msg.mtype = PTS;
    msg.message = CM_GET_USERS_LIST_REQUEST;
    msg.content[0] = '\0';

    // Отправляю сообщение на сервер
    int res = send_control_message(&msg);
    if (res != ECMSG_SUCCESS){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_get_users_list_request()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*Получить ответ запроса списока пользователей 
Принимает 
- пустой указатель на массив списка пользователей и allocate him, освобождайте самостоятельно
Размер имени пользователя MAX_OWN_NAME_SIZE

Возвращает
- Количество пользователей
- 0 - если список пуст
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL*/
static int get_users_list_response(char*** list){
    // Формирую сообщение для получения списка пользователей
    struct ControlUsersList recived;

    // Жду ответное сообщение CM_GET_HISTORY_RESPONSE
    // prio = 2 - получить от сервера (используется перед установлением соединения)
    msgrcv(control_queue_msqid, &recived, sizeof(struct ControlHistory), PFS, MSG_NOERROR);
    int res = check_msgrcv();
    if (res != ECMSG_SUCCESS) {
        *list =  NULL;
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgrcv() for history_msg in get_history_response()");
        #endif
        return res;
    }

    if (recived.list_size > 0) {
        // Выделяю память под массив имён
        *list = (char **)malloc(recived.list_size*sizeof(char*));
        if (*list == NULL) {
            #ifdef ETRACE_CLIENT_MESSAGING
                perror("malloc() for *list in get_history_response()");
            #endif
            return ECMSG_CRITICAL;
        }
        // Создать массив имён
        for (int i = 0; i < recived.list_size; i++){
             // Выделяю память под массив имён
            (*list)[i] = malloc(MAX_OWN_NAME_SIZE);
            // Не ошибка
            if ((*list)[i] != NULL){
                // Получаю и копирую имя пользователя
                char* token;
                
                if (i == 0)
                    token = strtok(recived.list, "|");
                else
                    token = strtok(NULL, "|");

                if (token != NULL){
                    memcpy((*list)[i], token, strlen(token) + 1);
                } 
                // Ошибка
                else{
                    for (int j = 0; j < i; j++){
                        free((*list)[i]);
                    }
                    free(*list);
                    #ifdef ETRACE_CLIENT_MESSAGING
                        perror("strtok() for name [(*list)[i]] in get_history_response()");
                    #endif
                    return ECMSG_CRITICAL;
                }
            }
            // Ошибка
            else {
                for (int j = 0; j < i; j++){
                    free((*list)[i]);
                }
                free(*list);
                #ifdef ETRACE_CLIENT_MESSAGING
                    perror("malloc() for name [(*list)[i]] in get_history_response()");
                #endif
                return ECMSG_CRITICAL;
            }
        }
    } else{
        // Не ошибка
        *list = NULL;
        return 0;
    }

    return recived.list_size;
}

//////////////////////////////////////////////////////////////////////
////////////////////////// PUBLIC FUNCTIONS //////////////////////////

/*Отправить управляющее сообщение
- msg - указатель на сообщение

Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int send_control_message(struct ControlMessage* msg){
    // Отправляю сообщение на сервер
    msgsnd(control_queue_msqid, msg, sizeof(struct ControlMessage), 0);
    int res = check_msgsnd();
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgsnd() for msg in send_control_message()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*Ждать управляющее сообщение
- msg - указатель на сообщение

Возвращает 
- ECMSG_SUCCESS
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int get_control_message(struct ControlMessage* msg, long priority){
    // Жду сообщение
    msgrcv(control_queue_msqid, msg, sizeof(struct ControlMessage), priority, 0);
    int res = check_msgrcv();
    if (res != ECMSG_SUCCESS){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("msgrcv() for msg in get_control_message()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*
Послать сообщение разрыва соединения
Устанавливает flg_connection_established = 0 в случае успеха
Возвращает 
- ECMSG_SUCCESS
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int send_break_connect(){
    // Формирую сообщение для отправки
    struct ControlMessage msg;
    msg.mtype = PTS;
    msg.message = CM_BREAK_CONNECT;
    msg.content[0] = '\0';

    // Отправляю сообщение CM_BREAK_CONNECT на сервер
    int res = send_control_message(&msg);
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_break_connect()");
        #endif
        return res;
    }
    return ECMSG_SUCCESS;
}

/*
Получить историю сообщений
Принимает 
- пустой указатель на историю сообщений и allocate him, освобождайте самостоятельно

Возвращает
- Возвращает размер массива истории сообщений в байтах
- 0 - если истории нет
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int get_history(char** history){
    // Отправить запрос истории
    int res = send_get_history_request();
    if (res != ECMSG_SUCCESS){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_get_history_request() in get_history()");
        #endif
        return res;
    }
    // Получить ответ на запрос истории сообщений
    *history = NULL;
    int history_size = get_history_response(history);
    if (
        history_size == ECMSG_NOMSG ||
        history_size == ECMSG_SIGNIT ||
        history_size == ECMSG_CRITICAL
        )
    {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_break_connect()");
        #endif
        return res;
    }
    return history_size;
}

/*
Получить список 
Принимает 
- пустой указатель на массив списка пользователей и allocate him, освобождайте самостоятельно
Размер имени пользователя MAX_OWN_NAME_SIZE

Возвращает
- Количество пользователей
- 0 - если список пуст
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int get_users_list(char*** list) {
    // Отправить запрос списка пользователей
    int res = send_get_users_list_request();
    if (res != ECMSG_SUCCESS){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_get_history_request() in get_history()");
        #endif
        return res;
    }
    // Получить ответ на запрос
    **list = NULL;
    int list_size = get_users_list_response(list);
    if (
        list_size == ECMSG_NOMSG ||
        list_size == ECMSG_SIGNIT ||
        list_size == ECMSG_CRITICAL
        ) 
    {

        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_control_message() for msg in send_break_connect()");
        #endif
        return res;
    }

    return list_size;
}


/*Установить соединение и получить историю сообщений
- name - Имя клиента
- name_size - Размер имени клиента
- history - Пустой указатель на историю сообщений и allocate him, освобождайте самостоятельно
- history_size - Размер массива истории сообщений в байтах, 0 - если истории нет
- list - Пустой указатель на массив списка пользователей и allocate him, освобождайте самостоятельно,
Размер имени пользователя MAX_OWN_NAME_SIZE
- list_size - Коичество пользователей, 0 - если список имён пуст

Возвращает
- ECMSG_SUCCESS
- ECMSG_NOMSG
- ECMSG_SIGNIT
- ECMSG_CRITICAL
- ECMSG_QUEUE_EXISTS

Требуется обработать возвращаемое значение, в случае неудачи
послать send_break_connect()*/
int establish_connection(char* name, int name_size, 
                        char** history, int* history_size, 
                        char*** list, int* list_size) {
    int res;
    // Подключиться к обеим очередям
    res = connect_all_queue();
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("connect_all_queue() in establish_connection()");
        #endif
        return res;
    }
    // Отправить запрос на установление соединения
    res = send_setup_connect_request(name, name_size);
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("send_setup_connect_request() in establish_connection()");
        #endif
        return res;
    }

    // Получить ответ установления соединения
    res = get_setup_connect_response(name);
    if (res != ECMSG_SUCCESS) {
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_setup_connect_response() in establish_connection()");
        #endif
        return res;
    }

    // Получить историю
    *history_size = get_history(history);
    if (*history_size < 0){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_history_response() in establish_connection()");
        #endif
        return res;
    }

    // Получить список пользователей

    *list = NULL;
    *list_size = get_users_list_response(list);
    if (*list_size < 0){
        #ifdef ETRACE_CLIENT_MESSAGING
            perror("get_users_list_response() in establish_connection()");
        #endif
        return res;
    }

    flg_connection_established = 1;

    return ECMSG_SUCCESS;
}
