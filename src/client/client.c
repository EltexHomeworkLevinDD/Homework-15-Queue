#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <string.h>
#include "../../include/client/transiver.h"
#include "../../include/client/threads.h"

#define PLANK_HEIGHT 3
// Должен быть больше или равен MAX_OWN_NAME_SIZE+2
#define USERS_LIST_WIDTH (MAX_USER_NAME_SZ+2+5)
#define COMMAND_WINDOW_HEIGHT 5

WINDOW* wnd_terminal;
int wnd_terminal_h = 0;
int wnd_terminal_w = 0;

Window wnd_plank_chat;
Window wnd_chat;

Window wnd_list;
Window wnd_plank_list;

Window wnd_command;
Window wnd_plank_command;

void calc_positions();
void create_interface();
void destroy_interface();

int main(){

    // Получить ключ очереди
    key_t queue_key = ftok(queue_path, 0);
    if (queue_key == (key_t)(-1)) {
        perror("ftok() for control_queue_key in establish_connection()");
        return -1;
    }
    #ifdef TRACE_MSG
        fprintf(stderr, "Key: %d\n", (int)queue_key);
    #endif
    // Постараться подключиться к очереди
    int msqid = msgget(queue_key, 0);
    if (msqid == -1){
        perror("msgget() for connection in establish_connection()");
        return -1;
    }
    #ifdef TRACE_MSG
        fprintf(stderr, "msqid: %d\n", msqid);
    #endif

    const char* cmd_exit = "/exit";
    const char* msg_promt = ">: ";

    // Запустить графику
    initscr();
    cbreak();
    start_color();
    keypad(stdscr, false);
    refresh();

    create_interface();

    // Формирую строку приглашения
    char promt_text[100];
    snprintf(promt_text, sizeof(promt_text), "Exit command: '/exit', your text must exclude '[' and ']'\nEnter your name (Max %d symbols) :> ", MAX_USER_NAME_SZ-1);
    char own_name[MAX_USER_NAME_SZ];

    // Ввожу имя
    int result; 
    char* own_name_ptr = own_name;
    while ((result = request_name(&wnd_command, promt_text, &own_name_ptr, MAX_USER_NAME_SZ, (char*)msg_promt)) != 0){
        // Запрошен выход
        if (result == -1){
            destroy_interface();
            endwin();
            return 0;
        // Строка содержит недопустымые символы, повторить ввод
        } else {
            continue;
        }
    }

    // Очистить окно
    werase(wnd_command.subwnd); // Очистка указанного окна
    wrefresh(wnd_command.subwnd); // Обновление указанного окна

    struct List users_list;
    initialize_list(&users_list);
    char history[MAX_HISTORY_SZ];
    history[0] = '\0';

    long PTS;
    long PFS;

    // Установить соединение
    if (process_msg_procedure_connect(msqid, own_name, &users_list, history, &PTS, &PFS) != 0){
        freeList(&users_list);
        perror("Critical error in main()");
        return -1;
    }

    //wmove(wnd_list.subwnd, 0, 0);
    print_users_list(&wnd_list, &users_list);
    //wmove(wnd_chat.subwnd, 0, 0);
    print_history(&wnd_chat, history);
    //wmove(wnd_command.subwnd, 0, 0);

    #ifdef TRACE_MSG
        fprintf(stderr, "Connection established\n");
    #endif

    // Создать поток Uplink
    pthread_t uplink_tid;
    int* uplink_status;
    struct UplinkArgs uplink_args = {
        .msqid = msqid,
        .own_name = own_name,
        .PTS = PTS,
        .cmd_exit = cmd_exit,
        .msg_promt = msg_promt,
        .wnd_command = &wnd_command
    };

    if (pthread_create(&uplink_tid, NULL, uplink, (void*)&uplink_args) != 0){
        perror("Creating uplink() thread");
        freeList(&users_list);
        destroy_interface();
        endwin();
        return -1;
    }

    // Создать поток Downlink
    pthread_t downlink_tid;
    int* downlink_status;
    struct DownlinkArgs downlink_args = {
        .msqid = msqid,
        .history = history,
        .PFS = PFS,
        .own_name = own_name,
        .users_list = &users_list,
        .wnd_command = &wnd_command,
        .wnd_chat = &wnd_chat,
        .wnd_list = &wnd_list,
        .uplink_tid = uplink_tid
    };

    if (pthread_create(&downlink_tid, NULL, downlink, (void*)&downlink_args) != 0){
        perror("Creating uplink() thread");
        freeList(&users_list);
        destroy_interface();
        endwin();
        return -1;
    }

    // Ждать завершение потока Downlink
    pthread_join(downlink_tid, (void**)&downlink_status);
    if (downlink_status != NULL){
        freeList(&users_list);
        destroy_interface();
        endwin();
        perror("downlink() has broken");
        return -1;
    }

    // Ждать завершение потока Uplink
    pthread_join(uplink_tid, (void**)&uplink_status);
    if (uplink_status != NULL){
        // Если поток был завершён насильно из uplink()
        if (uplink_status == PTHREAD_CANCELED){
            // ничего не делать
        }
        // Если завершися с ошибкой
        else {
            freeList(&users_list);
            destroy_interface();
            endwin();
            perror("Uplink() has broken");
            return -1;
        }
    }
    
    destroy_interface();
    endwin();
    return 0;
}

void create_interface(){
    //  Создать окно терминала
    wnd_terminal = newwin(0, 0, 0, 0);
    getmaxyx(wnd_terminal, wnd_terminal_h, wnd_terminal_w);
    calc_positions();

    // Создать командную плашку и окно
    create_std_plank(&wnd_plank_command, "Command window");
    wrefresh(wnd_plank_command.mainwnd);
    create_std_window(&wnd_command);
    wrefresh(wnd_command.mainwnd);
    // Создать чат плашку и окно
    create_std_plank(&wnd_plank_chat, "Chat window");
    wrefresh(wnd_plank_chat.mainwnd);
    create_std_window(&wnd_chat);
    wrefresh(wnd_chat.mainwnd);
    // Создать список плашку и окно
    create_std_plank(&wnd_plank_list, "Users list");
    wrefresh(wnd_plank_list.mainwnd);
    create_std_window(&wnd_list);
    wrefresh(wnd_list.mainwnd);
}

void calc_positions(){
    // Command Window
    wnd_command.main_h = COMMAND_WINDOW_HEIGHT;
    wnd_command.main_w = wnd_terminal_w;
    wnd_command.main_y = wnd_terminal_h-COMMAND_WINDOW_HEIGHT;
    wnd_command.main_x = 0;
    wnd_command.sub_h = wnd_command.main_h - 2;
    wnd_command.sub_w = wnd_command.main_w - 2;
    // Command Plank
    wnd_plank_command.main_h = PLANK_HEIGHT;
    wnd_plank_command.main_w = wnd_terminal_w;
    wnd_plank_command.main_y = wnd_command.main_y - PLANK_HEIGHT;
    wnd_plank_command.main_x = 0;
    wnd_plank_command.sub_h = wnd_plank_command.main_h - 2;
    wnd_plank_command.sub_w = wnd_plank_command.main_w - 2;
    // Users list Plank
    wnd_plank_list.main_h = PLANK_HEIGHT;
    wnd_plank_list.main_w = USERS_LIST_WIDTH;
    wnd_plank_list.main_y = 0;
    wnd_plank_list.main_x = wnd_terminal_w-USERS_LIST_WIDTH;
    wnd_plank_list.sub_h = wnd_plank_list.main_h - 2;
    wnd_plank_list.sub_w = wnd_plank_list.main_w - 2;
    // Users list Window
    wnd_list.main_h = wnd_terminal_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    wnd_list.main_w = USERS_LIST_WIDTH;
    wnd_list.main_y = PLANK_HEIGHT;
    wnd_list.main_x = wnd_terminal_w-USERS_LIST_WIDTH;
    wnd_list.sub_h = wnd_list.main_h - 2;
    wnd_list.sub_w = wnd_list.main_w - 2;
    // Chat Plank
    wnd_plank_chat.main_h = PLANK_HEIGHT;
    wnd_plank_chat.main_w = wnd_terminal_w - wnd_plank_list.main_w;
    wnd_plank_chat.main_y = 0;
    wnd_plank_chat.main_x = 0;
    wnd_plank_chat.sub_h = wnd_plank_chat.main_h - 2;
    wnd_plank_chat.sub_w = wnd_plank_chat.main_w - 2;
    // Chat Window
    wnd_chat.main_h = wnd_terminal_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    wnd_chat.main_w = wnd_terminal_w - wnd_plank_list.main_w;
    wnd_chat.main_y = PLANK_HEIGHT;
    wnd_chat.main_x = 0;
    wnd_chat.sub_h = wnd_chat.main_h - 2;
    wnd_chat.sub_w = wnd_chat.main_w - 2;
}

void destroy_interface(){
    delete_std_window_r(&wnd_plank_command);
    delete_std_window_r(&wnd_command);
    delete_std_window_r(&wnd_plank_chat);
    delete_std_window_r(&wnd_chat);
    delete_std_window_r(&wnd_plank_list);
    delete_std_window_r(&wnd_list);
}