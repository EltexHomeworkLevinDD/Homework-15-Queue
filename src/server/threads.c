#include "../../include/server/threads.h"

volatile int terminate_flag = 0;

void* serverthread(void* serverthread_args){
    int result;
    int type;
    struct Node* current;
    struct ServerThreadArgs* args = (struct ServerThreadArgs*)serverthread_args;

    while (1) {
        // Получена команда завершения потока
        if (terminate_flag != 0){
            // Послать сообщение всем, чтобы они отсоеденены и выйти из потока
            current = args->users_list->head;
            struct TransactionUser trans;
            while (current != NULL){
                // Послать уведомление
                if ((result = send_notification(args->msqid, current->user->PFS, NT_USER_DISCONNECTED, 0)) != 0) {
                    if (result == -1){
                        perror("send_notification() for NT_USER_CONNECTED");
                    }
                    pthread_exit((void*)-1);
                }
                // Послать транзакцию
                strncpy(trans.name, current->user->name, MAX_USER_NAME_SZ);
                trans.name[MAX_USER_NAME_SZ-1] = '\0';

                // Послать транзакцию
                if ((result = send_transaction(args->msqid, current->user->PFS, (void*)&trans, sizeof(trans.name), 0)) != 0) {
                    if (result == -1){
                        perror("send_transaction() for transConnectSize");
                    }
                    pthread_exit((void*)-1);
                }
                current = current->next;
            }
            // Завершить поток
            return NULL;
        }
        // Слушать 1 приоритет (подключение) в неблокирующем режиме
        // Получить уведомление о желании подключиться (на приоритете 1)
        result = receive_notification(args->msqid, 1, &type, IPC_NOWAIT);
        switch (result)
        {
        // Ошибка
        case -1:
            perror("receive_notification() for NT_CONNECT_REQUEST");
            pthread_exit((void*)-1);
            break;
        // Нет сообщения - ничего не делать
        case 1:
            break;
        // Сообщение получено, установить оединение
        case 0:
            #ifdef TRACE_MSG
                fprintf(stderr, "mtype[1] Received Notification [NT_CONNECT_REQUEST]\n");
            #endif

            if (process_procedure_connect(args->msqid, args->users_list, args->history) != 0){
                perror("Critical error in main()");
                pthread_exit((void*)-1);
            }

            if (process_procedure_user_connected(args->msqid, (args->users_list->tail)->user->name, args->users_list) != 0){
                perror("process_procedure_user_connected()");
                pthread_exit((void*)-1);
            }
            fprintf(stderr, "Connection established\n");
            break;
        }

        // Слушать приоритеты PTS всех пользователей в неблокирующем режиме
        current = args->users_list->head;
        while (current != NULL){
            // Слушать уведомление на PTS пользователя в неблокирующем режиме
            result = receive_notification(args->msqid,  current->user->PTS, &type, IPC_NOWAIT);
            switch (result)
            {
            // Ошибка
            case -1:
                perror("receive_notification() for listening user PTS");
                pthread_exit((void*)-1);
                break;
            // Нет сообщения - ничего не делать
            case 1:
                break;
            // Сообщение получено, реагировать
            case 0:
                // Определить тип сообщения
                switch (type)
                {
                // Новое сообщение от пользователя
                case NT_NEW_MESSAGE:
                    #ifdef TRACE_MSG
                        fprintf(stderr, "mtype[%ld] Received Notification [NT_NEW_MESSAGE]\n", current->user->PTS);
                    #endif

                    if (process_procedure_new_message_from_user(args->msqid, current->user->PTS, args->users_list, args->history) != 0){
                        perror("process_procedure_new_message_from_user() for [NT_NEW_MESSAGE] user PTS");
                        pthread_exit((void*)-1);
                    }
                    break;
                // Пользователь отсоединился
                case NT_USER_DISCONNECTED:
                    #ifdef TRACE_MSG
                        fprintf(stderr, "mtype[%ld] Received Notification [NT_USER_DISCONNECTED]\n", current->user->PTS);
                    #endif

                    if (process_procedure_user_disconnected(args->msqid, current->user->PTS, args->users_list) != 0){
                        perror("process_procedure_new_message_from_user() for [NT_NEW_MESSAGE] user PTS");
                        pthread_exit((void*)-1);
                    }
                    break;
                }
                break;
            }

            current = current->next;
        }
    }

    return NULL;
}