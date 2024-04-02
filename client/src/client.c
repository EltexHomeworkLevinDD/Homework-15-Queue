#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <malloc.h>
#include <string.h>
#include "messaging/client_control_messaging.h"
#include "messaging/client_text_messaging.h"
#include "graphics/command_window.h"
#include "graphics/chat_window.h"
#include "graphics/list_window.h"
#include "threads/client_threads.h"

char own_name[MAX_OWN_NAME_SIZE];
int own_name_length;

WINDOW* terminalwnd;
int terminalwnd_h = 0;
int terminalwnd_w = 0;

Window chatPlankwnd;
Window chatwnd;

Window listwnd;
Window listPlankwnd;

Window commandwnd;
Window commandPlankwnd;

void sig_winch(int signo) { 
    struct winsize size; 
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); 
    resizeterm(size.ws_row, size.ws_col); 

    (void)signo; // For no warnings
}

#define PLANK_HEIGHT 3
// Должен быть больше или равен MAX_OWN_NAME_SIZE+2
#define USERS_LIST_WIDTH (MAX_OWN_NAME_SIZE+2+5)
#define COMMAND_WINDOW_HEIGHT 5

void calc_positions();
void create_interface();
void destroy_interface();

int main() {

    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    start_color();
    keypad(stdscr, false);
    refresh();

    //  Создать окно терминала
    terminalwnd = newwin(0, 0, 0, 0);
    getmaxyx(terminalwnd, terminalwnd_h, terminalwnd_w);
    calc_positions();

    // Создать командную плашку и окно
    create_window_command_plank_r(&commandPlankwnd);
    create_window_command_r(&commandwnd);
    // Создать чат плашку и окно
    create_window_chat_plank_r(&chatPlankwnd);
    create_window_chat_r(&chatwnd);
    // Создать список плашку и окно
    create_window_list_plank_r(&listPlankwnd);
    create_window_list_r(&listwnd);

    // Формирую текст приглашения
    char promt_text[60];
    snprintf(promt_text, sizeof(promt_text), "Enter your name (Max %d symbols, excluding '[' and ']') ", MAX_OWN_NAME_SIZE-1);
    // Запрашиваю имя
    int res;
    while ((res = request_name(&commandwnd, promt_text, own_name, MAX_OWN_NAME_SIZE)) != 0){
        // Выход из приложения
        if (res == -1){
            destroy_interface();
            endwin();
            return 0;
        }
        // Критическая ошибка
        else if (EG_CRIRICAL) {
            perror("request_name() for own_name in main()");
            destroy_interface();
            endwin();
            return -1;
        }
        // Строка содержит недопустимые символы, повторить
    }

    //char ch = getch();

    delete_window_command_plank_r(&commandPlankwnd);
    delete_window_command_r(&commandwnd);
    delete_window_chat_plank_r(&chatPlankwnd);
    delete_window_chat_r(&chatwnd);
    delete_window_list_plank_r(&listPlankwnd);
    delete_window_list_r(&listwnd);
    endwin();
    return 0;
}

void calc_positions(){
    // Command Window
    commandwnd.main_h = COMMAND_WINDOW_HEIGHT;
    commandwnd.main_w = terminalwnd_w;
    commandwnd.main_y = terminalwnd_h-COMMAND_WINDOW_HEIGHT;
    commandwnd.main_x = 0;
    commandwnd.sub_h = commandwnd.main_h - 2;
    commandwnd.sub_w = commandwnd.main_w - 2;
    // Command Plank
    commandPlankwnd.main_h = PLANK_HEIGHT;
    commandPlankwnd.main_w = terminalwnd_w;
    commandPlankwnd.main_y = commandwnd.main_y - PLANK_HEIGHT;
    commandPlankwnd.main_x = 0;
    commandPlankwnd.sub_h = commandPlankwnd.main_h - 2;
    commandPlankwnd.sub_w = commandPlankwnd.main_w - 2;
    // Users list Plank
    listPlankwnd.main_h = PLANK_HEIGHT;
    listPlankwnd.main_w = USERS_LIST_WIDTH;
    listPlankwnd.main_y = 0;
    listPlankwnd.main_x = terminalwnd_w-USERS_LIST_WIDTH;
    listPlankwnd.sub_h = listPlankwnd.main_h - 2;
    listPlankwnd.sub_w = listPlankwnd.main_w - 2;
    // Users list Window
    listwnd.main_h = terminalwnd_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    listwnd.main_w = USERS_LIST_WIDTH;
    listwnd.main_y = PLANK_HEIGHT;
    listwnd.main_x = terminalwnd_w-USERS_LIST_WIDTH;
    listwnd.sub_h = listwnd.main_h - 2;
    listwnd.sub_w = listwnd.main_w - 2;
    // Chat Plank
    chatPlankwnd.main_h = PLANK_HEIGHT;
    chatPlankwnd.main_w = terminalwnd_w - listPlankwnd.main_w;
    chatPlankwnd.main_y = 0;
    chatPlankwnd.main_x = 0;
    chatPlankwnd.sub_h = chatPlankwnd.main_h - 2;
    chatPlankwnd.sub_w = chatPlankwnd.main_w - 2;
    // Chat Window
    chatwnd.main_h = terminalwnd_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    chatwnd.main_w = terminalwnd_w - listPlankwnd.main_w;
    chatwnd.main_y = PLANK_HEIGHT;
    chatwnd.main_x = 0;
    chatwnd.sub_h = chatwnd.main_h - 2;
    chatwnd.sub_w = chatwnd.main_w - 2;
}

void destroy_interface(){
    delete_window_command_plank_r(&commandPlankwnd);
    delete_window_command_r(&commandwnd);
    delete_window_chat_plank_r(&chatPlankwnd);
    delete_window_chat_r(&chatwnd);
    delete_window_list_plank_r(&listPlankwnd);
    delete_window_list_r(&listwnd);
}