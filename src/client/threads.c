#include "../../include/client/threads.h"

volatile int uplink_is_dead = 0;

void* uplink(void* uplink_args){
    //int result;
    struct UplinkArgs* args = (struct UplinkArgs*)uplink_args;
    char input[MAX_MESSAGE_SZ-3-MAX_USER_NAME_SZ];
    //struct TransactionMessage trans;

    // Непрерывно работать
    while (1){    
        // Ввожу сообщение
        wmove(args->wnd_command->subwnd, 0, 0);
        werase(args->wnd_command->subwnd);

        mvwprintw(args->wnd_command->subwnd, 0, 0, "%s", args->msg_promt);
        wrefresh(args->wnd_command->subwnd);
        if (wgetnstr(args->wnd_command->subwnd, input, MAX_MESSAGE_SZ-3-MAX_USER_NAME_SZ) == ERR){
            perror("wgetnstr() for message in uplink()");
            pthread_exit((void*)-2);
        }
        // Удаляю символ новой строки 
        input[strcspn(input, "\n")] = '\0';
        
        // Проверить текст на соответствие команде выхода
        if (strncmp(input, args->cmd_exit, strlen(args->cmd_exit)+1) != 0){
            // Не команда выхода
            // Ищу недопустимые символы ('[' или ']')
            char *ptr = strpbrk(input, "[]");
            if (ptr != NULL) {
                //perror("strpbrk(), message contains '[' or ']'");
                continue;
            }
        } else {
            // Команда выхода, Выйти немедлено
            process_msg_procedure_i_am_disconnected(args->msqid, args->PTS);
            uplink_is_dead = 1;
            // Завершить поток
            return NULL;
        }

        if (process_msg_procedure_new_message_to_server(args->msqid, input, args->own_name, args->PTS) != 0){
            perror("process_msg_procedure_new_message_to_server() in uplink()'");
            pthread_exit((void*)-2);
        }
    }

    return NULL;
}

void* downlink(void* downlink_args){
    int old_y, old_x;
    int type;
    struct DownlinkArgs* args = (struct DownlinkArgs*)downlink_args;

    // Проверяем, инициализированы ли окна
    if (args->wnd_command == NULL || args->wnd_list == NULL || args->wnd_command->subwnd == NULL || args->wnd_list->subwnd == NULL) {
        perror("Error: Windows are not properly initialized");
        pthread_exit((void*)-2);
    }

    while(1){
        // Ждать уведомление от сервера
        if (receive_notification(args->msqid, args->PFS, &type, 0) != 0){
            perror("receive_notification() in downlink()");
            pthread_exit((void*)-2);
        }

        // Получаем текущие координаты курсора в окне команд
        getyx(args->wnd_command->subwnd, old_y, old_x);

        // Определить тип сообщения
        switch (type)
        {
        // Пользователь подключился
        case NT_USER_CONNECTED:
            #ifdef TRACE_MSG
                fprintf(stderr, "Received Notification [NT_USER_CONNECTED]\n");
            #endif

            if (process_msg_procedure_user_connected(args->msqid, args->PFS, args->users_list) != 0){
                perror("process_msg_procedure_user_connected() for NT_USER_CONNECTED in downlink()");
                pthread_exit((void*)-2);
            }

            // Отрисовываем список пользователей
            print_users_list(args->wnd_list, args->users_list);

            // Перемещаем курсор обратно на старые координаты
            wmove(args->wnd_command->subwnd, old_y, old_x);
            wrefresh(args->wnd_command->subwnd);
            break;
        // Пользователь отключился
        case NT_USER_DISCONNECTED:
            #ifdef TRACE_MSG
                fprintf(stderr, "Received Notification [NT_USER_DISCONNECTED]\n");
            #endif

            switch (process_msg_procedure_user_disconnected(args->msqid, args->PFS, args->users_list, args->own_name))
            {
            // Отсоеденился я
            case 2:
                // Ждать 10 милисекунд
                usleep(10000);
                if (uplink_is_dead == 1){
                    // Если поток uplink уже мёртв, то была введена команда выхода 
                } else {
                    // Если поток uplink ещё жив, то сервер умер, поток надо завершить насильно
                    if (pthread_cancel(args->uplink_tid) != 0){
                        printf("pthread_cancel() for uplink() in downlink()");
                        pthread_exit((void*)-2);
                    }
                }
                // Завершить поток
                uplink_is_dead = 1;
                return NULL;
                break;
            // Отсоеденился другой пользователь
            case 0:
                // Отрисовываем список пользователей
                print_users_list(args->wnd_list, args->users_list);

                // Перемещаем курсор обратно на старые координаты
                wmove(args->wnd_command->subwnd, old_y, old_x);
                wrefresh(args->wnd_command->subwnd);
                break;
            // Критическая ошибка
            default:
                perror("process_msg_procedure_user_disconnected() for NT_USER_DISCONNECTED in downlink()");
                pthread_exit((void*)-2);
                break;
            }
            break;
        
        // Новое сообщение от сервера
        case NT_NEW_MESSAGE:
            #ifdef TRACE_MSG
                fprintf(stderr, "Received Notification [NT_NEW_MESSAGE]\n");
            #endif

            if (process_msg_procedure_new_message_from_server(args->msqid, args->PFS, args->history) != 0){
                perror("process_msg_procedure_new_message_from_server() for NT_NEW_MESSAGE in downlink()");
                pthread_exit((void*)-2);
            }

            // Отрисовываем историю
            print_history(args->wnd_chat, args->history);

            // Перемещаем курсор обратно на старые координаты
            wmove(args->wnd_command->subwnd, old_y, old_x);
            wrefresh(args->wnd_command->subwnd);
            break;
        }
    }

    return NULL;
}