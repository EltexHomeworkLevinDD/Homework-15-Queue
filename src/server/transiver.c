#include "../../include/server/transiver.h"

/*Установить соединение
Возвращает 
- msqid - Идентификатор очереди
- users_list - указатель на список пользоваталей (пользователь будет добавлен в конец)
- history - указатель на историю сообщений

- 0 - Если процедура завершена
- 1 - Если сообщения нет
- -1 - Если возникла критическая ошибка*/
int process_procedure_connect(int msqid, struct List* users_list, char* history){
    int result;

    // Получить транзакцию с именем (на приоритете 2) в блокирующем режиме
    struct TransactionUser transUser;
    if ((result = receive_transaction(msqid, 2, (void*)&transUser, sizeof(transUser.name), 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionUser");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[2] Received TransactionUser, name: '%s'\n", transUser.name);
    #endif

    // Создать пользователя, выдать PTS и PFS 
    struct User* user = create_new_user_auto(users_list, transUser.name);
    if (user == NULL){
        perror("create_new_user_auto() User* = NULL");
        return -1;
    }
    // Добавить в список
    struct Node* node = append(users_list, user);
    if (node == NULL){
        perror("create_new_user_auto() Node* = NULL");
        return -1;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "User added, name = '%s', PTS: %ld, PFS: %ld\n", transUser.name, user->PTS, user->PFS);
    #endif

    // Отправить транзакцию TransactionConnect
    // Сформировать транзакцию
    struct TransactionConnect transConnect;
    transConnect.PFS = user->PFS;
    transConnect.PTS = user->PTS;
    // Скопировать историю
    strncpy(transConnect.history, history, MAX_HISTORY_SZ);
    transConnect.history[MAX_HISTORY_SZ-1] = '\0';
    for (int i = 0; i < MAX_USERS_COUNT; i++){
        memset(transConnect.users_list[i], '\0', MAX_USER_NAME_SZ);
    }
    
    // Пройтись по всем пользователям кроме текущего и добавить их в список
    struct Node* current = users_list->head;
    int index = 0;
    while (current != NULL){
        // Исключаем текущего пользователя
        if (strncmp(current->user->name, user->name, MAX_USER_NAME_SZ) != 0){
            // Скопировать имя
            strncpy(transConnect.users_list[index], current->user->name, MAX_USER_NAME_SZ);
            transConnect.users_list[index][MAX_USER_NAME_SZ-1] = '\0';
            index++;
        }
        current = current->next;
    }

    // Отправить сформированную транзакцию на приоритет 3
    size_t transConnectSize = sizeof(long)*2 + MAX_HISTORY_SZ + MAX_USERS_COUNT*MAX_USER_NAME_SZ;
    if ((result = send_transaction(msqid, 3, (void*)&transConnect, transConnectSize, 0)) != 0) {
        if (result == -1){
            perror("send_transaction() for transConnectSize");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[3] Sended TransactionConnect\n");
    #endif

    return 0;
}

/*Послать сообщение всем, что новый пользователь появился
- 0 - Если процедура завершена
- -1 - Если возникла критическая ошибка*/
int process_procedure_user_connected(int msqid, char* name, struct List* users_list){
    int result;
    // Создать транзакцию
    struct TransactionUser trans;
    strncpy(trans.name, name, MAX_USER_NAME_SZ);
    trans.name[MAX_USER_NAME_SZ-1] = '\0';

    // Отправить всем пользователям
    struct Node* current = users_list->head;
    while (current != NULL){
        // Отправить уведомление о новом пользователе
        if ((result = send_notification(msqid, current->user->PFS, NT_USER_CONNECTED, 0)) != 0) {
            if (result == -1){
                perror("send_notification() for NT_USER_CONNECTED");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended Notification [NT_USER_CONNECTED]\n", current->user->PFS);
        #endif

        // Отправить транзакцию имя нового пользователя
        if ((result = send_transaction(msqid, current->user->PFS, (void*)&trans, sizeof(trans.name), 0)) != 0) {
            if (result == -1){
                perror("send_transaction() for transConnectSize");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended TransactionUser, name: '%s'\n", current->user->PFS, name);
        #endif

        current = current->next;
    }
    return 0;
}

/*Получить сообщение, сохранить в истории и отправить его всем пользователям*/
int process_procedure_new_message_from_user(int msqid, long from, struct List* users_list, char* history){
    int result;

    // Получить транзакцию сообщения
    struct TransactionMessage received_trans;    
    if ((result = receive_transaction(msqid, from, (void*)&received_trans, sizeof(received_trans.message), 0)) != 0) {
        if (result == -1){
            perror("receive_transaction() for TransactionMessage");
        }
        return result;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "mtype[%ld] Received TransactionMessage: message: '%s'\n", from, received_trans.message);
    #endif

    // Сохранить в истории
    if (save_message_to_history(received_trans.message, history, MAX_HISTORY_SZ) != 0){
        perror("save_message_to_history() in process_msg_procedure_new_message_new_message_from_server()");
        return -1;
    }

    #ifdef TRACE_MSG
        fprintf(stderr, "history: '%s'\n", history);
    #endif

    // Отправить всем пользователям
    struct Node* current = users_list->head;
    while (current != NULL){
        // Отправить уведомление о новом пользователе
        if ((result = send_notification(msqid, current->user->PFS, NT_NEW_MESSAGE, 0)) != 0) {
            if (result == -1){
                perror("send_notification() for NT_NEW_MESSAGE");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended Notification [NT_NEW_MESSAGE]\n", current->user->PFS);
        #endif

        // Отправить транзакцию имя нового пользователя
        if ((result = send_transaction(msqid, current->user->PFS, (void*)&received_trans, sizeof(received_trans.message), 0)) != 0) {
            if (result == -1){
                perror("send_transaction() for transConnectSize");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended TransactionMessage, message: '%s'\n", current->user->PFS, received_trans.message);
        #endif

        current = current->next;
    }

    return 0;
}

/*Пользователь отсоеденился, удалить из списка и отправить сообщение всем*/
int process_procedure_user_disconnected(int msqid, long from, struct List* users_list){
    int result;
    // Сформировать транзакцию
    struct TransactionUser trans;
    struct Node* node = search_by_PTS(users_list, from);
    struct User* user = node->user;
    strncpy(trans.name, user->name, MAX_USER_NAME_SZ);
    trans.name[MAX_USER_NAME_SZ-1] = '\0';

    // Отправить всем пользователям
    struct Node* current = users_list->head;
    while (current != NULL){
        // Отправить уведомление о новом пользователе
        if ((result = send_notification(msqid, current->user->PFS, NT_USER_DISCONNECTED, 0)) != 0) {
            if (result == -1){
                perror("send_notification() for NT_USER_DISCONNECTED");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended Notification [NT_USER_DISCONNECTED]\n", current->user->PFS);
        #endif

        // Отправить транзакцию имя нового пользователя
        if ((result = send_transaction(msqid, current->user->PFS, (void*)&trans, sizeof(trans.name), 0)) != 0) {
            if (result == -1){
                perror("send_transaction() for transConnectSize");
            }
            return result;
        }

        #ifdef TRACE_MSG
            fprintf(stderr, "mtype[%ld] Sended TransactionUser, name: '%s'\n", current->user->PFS, trans.name);
        #endif

        current = current->next;
    }

    // Удалить пользователя
    remove_node(users_list, node);

    return 0;
}