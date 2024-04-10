#include "../../include/client/transiver.h"

/*Сформировать полное сообщение ([Имя] Текст)
Принимает
- text - текст сообщения
- size - размер сообщения
- fullmessage - пустой указатель на сообщение, и
allocate him, освобождайте самостоятельно

Возвращает
- Размер сообщения
- -1 в случае критической ошибки
*/
int create_message(char* text, char* own_name, char** fullmessage){
    int size = strlen(text) + 1;
    int own_name_length = strlen(own_name);
    // Выделяю память под сообщение
    // text_size
    // own_name_length
    // +2 for '[' & ']'
    // +1 for ' ' after ']'
    int fullmessage_size = size+own_name_length+2+1;
    (*fullmessage) = malloc(fullmessage_size);
    if (*fullmessage == NULL){
        return -1;
    }
    int clen = 0;

    // Добавляю открывающую скобку
    (*fullmessage)[clen] = '[';
    clen++;
    // Добавляю имя
    memcpy(*fullmessage + clen, own_name, own_name_length);
    clen += own_name_length;
    // Добавляю закрывающую скобку
    (*fullmessage)[clen++] = ']';
    // Добавляю пробел
    (*fullmessage)[clen++] = ' ';
    // Добавляю текст
    memcpy(*fullmessage + clen, text, size);
    
    return fullmessage_size;
}

/*Установить соединение
Возвращает 
- msqid - Идентификатор очереди
- users_list - указатель на список пользоваталей (пользователь будет добавлен в конец)
- history - указатель на историю сообщений

- 0 - Если процедура завершена
- 1 - Если сообщения нет
- -1 - Если возникла критическая ошибка*/
int process_msg_procedure_connect(int msqid, char* name, struct List* users_list, char* history, long* PTS, long* PFS){
    int result;

    // Отправить уведомление о желании подключиться (на приоритете 1)
    if ((result = send_notification(msqid, 1, NT_CONNECT_REQUEST, 0)) != 0) {
        if (result == -1){
            perror("send_notification() for NT_CONNECT_REQUEST");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[1] Sended Notification [NT_CONNECT_REQUEST]\n");
    #endif

    // Отправить транзакцию с именем (на приоритете 2)
    // Сформировать
    struct TransactionUser transUser;
    strncpy(transUser.name, name, MAX_USER_NAME_SZ);
    // Отправить
    if ((result = send_transaction(msqid, 2, (void*)&transUser, sizeof(transUser.name), 0)) != 0) {
        if (result == -1){
            perror("send_transaction() for TransactionUser");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[2] Sended TransactionUser, name: '%s'\n", transUser.name);
    #endif

    // Получить транзакцию TransactionConnect с историей и текущим списком пользователей на приоритете 3
    struct TransactionConnect transConnect;
    size_t transConnectSize = sizeof(long)*2 + MAX_HISTORY_SZ + MAX_USERS_COUNT*MAX_USER_NAME_SZ;
    if ((result = receive_transaction(msqid, 3, (void*)&transConnect, transConnectSize, 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionConnect");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[3] Received TransactionConnect, \nname: '%s', PTS: %ld, PFS: %ld\n", 
            transUser.name, transConnect.PTS, transConnect.PFS);
        fprintf(stderr, "\nhistory: '%s'\n", transConnect.history);
    #endif

    *PTS = transConnect.PTS;
    *PFS = transConnect.PFS;

    // Копируем историю
    strncpy(history, transConnect.history, MAX_HISTORY_SZ);
    history[MAX_HISTORY_SZ-1] = '\0';
    // Копируем пользователей
    struct User* user;
    for (int i = 0; transConnect.users_list[i][0] != '\0'; i++){
        #ifdef TRACE_MSG
            fprintf(stderr, "User: '%s'\n", transConnect.users_list[i]);
        #endif
        // Создаём пользователя
        user = create_new_user(transConnect.users_list[i], 0, 0);
        if (user == NULL){
            perror("create_new_user()");
            return -1;
        }
        // Создаём пользователя добавляем пользователя
        if (append(users_list, user) == NULL){
            perror("create_new_user()");
            return -1;
        }
    }
    
    return 0;
}

/*Отправить сообщение на сервер
Форирует полное сообщение из простого текстового*/
int process_msg_procedure_new_message_to_server(int msqid, char* simple_message, char* your_name, long to){
    int result;
    struct TransactionMessage trans;

    // --------- Сформировать и отправить сообщение
    char* full_message;
    int full_message_size = create_message(simple_message, your_name, &full_message);
    if (full_message_size == -1){
        perror("create_message(), uplink()");
        return -1;
    }

    // Отправить уведомление о новом сообщении
    if ((result = send_notification(msqid, to, NT_NEW_MESSAGE, 0)) != 0) {
        if (result == -1){
            free(full_message);
            perror("send_notification() for NT_NEW_MESSAGE");
        }
        return -1;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Sended Notification [NT_NEW_MESSAGE]\n", to);
    #endif

    // Отправить транзакцию новое сообщение
    strncpy(trans.message, full_message, MAX_MESSAGE_SZ);
    if ((result = send_transaction(msqid, to, (void*)&trans, sizeof(trans.message), 0)) != 0) {
        if (result == -1){
            perror("send_transaction() for transConnectSize");
        }
        return -1;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Sended TransactionMessage, message: '%s'\n", to, trans.message);
    #endif

    return 0;
}

/*Отправить сообщение об отключении*/
int process_msg_procedure_i_am_disconnected(int msqid, long to){
    int result;

    // Отправить уведомление об отключении на сервер
    if ((result = send_notification(msqid, to, NT_USER_DISCONNECTED, 0)) != 0) {
        if (result == -1){
            perror("send_notification() for NT_USER_DISCONNECTED");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Sended Notification [NT_USER_DISCONNECTED]\n", to);
    #endif

    return 0;
}

/*Получено новое сообщение
Сохраняет сообщение в историю*/
int process_msg_procedure_new_message_from_server(int msqid, long from, char* history){
    int result;
    // Получить транзакцию сообщения
    struct TransactionMessage trans;    
    if ((result = receive_transaction(msqid, from, (void*)&trans, sizeof(trans.message), 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionMessage");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Received TransactionMessage: message: '%s'\n", from, trans.message);
    #endif

    if (save_message_to_history(trans.message, history, MAX_HISTORY_SZ) != 0){
        perror("save_message_to_history() in process_msg_procedure_new_message_new_message_from_server()");
        return -1;
    }

    return 0;
}

/*Пользователь приеденился, добавить в список*/
int process_msg_procedure_user_connected(int msqid, long from, struct List* users_list){
    int result;
    // Получить транзакцию сообщения
    struct TransactionUser trans;    
    if ((result = receive_transaction(msqid, from, (void*)&trans, sizeof(trans.name), 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionUser");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Received TransactionUser: name: '%s'\n", from, trans.name);
    #endif


    // Копируем пользователя
    struct User* user;
    user = create_new_user(trans.name, 0, 0);
    if (user == NULL){
        perror("create_new_user()");
        return -1;
    }
    // Добавляем пользователя
    if (append(users_list, user) == NULL){
        perror("create_new_user()");
        return -1;
    }

    return 0;
}

/*Пользователь отсоеденился, удалить из списка
Возвраащет 
- 0 - в случае успеха
- 2 - в случае, если отсоеденился данный пользователь
- остальное в случае критической ошибки*/
int process_msg_procedure_user_disconnected(int msqid, long from, struct List* users_list, char* your_name){
    int result;
    // Получить транзакцию сообщения
    struct TransactionUser trans;    
    if ((result = receive_transaction(msqid, from, (void*)&trans, sizeof(trans.name), 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionUser");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Received TransactionUser: name: '%s'\n", from, trans.name);
    #endif

    // Сравниваем имя пользователя с полученным
        // Проверить текст на соответствие команде выхода
    if (strncmp(trans.name, your_name, MAX_USER_NAME_SZ) == 0){
        // Имя совпало
        return 2;
    } else {
        // Имя не совпало
        // Удаляем пользователя
        struct Node* node = search_by_name(users_list, trans.name);
        if (node == NULL){
            perror("search_by_name() in process_msg_procedure_user_disconnected()");
            return -1;
        }

        remove_node(users_list, node);
    return 0;
    }
}