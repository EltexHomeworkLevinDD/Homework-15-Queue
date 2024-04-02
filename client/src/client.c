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
#include "graphics/text_window.h"
#include "graphics/list_window.h"
#include "threads/client_threads.h"

char own_name[MAX_OWN_NAME_SIZE];
int own_name_length;

void sig_winch(int signo) { 
    struct winsize size; 
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); 
    resizeterm(size.ws_row, size.ws_col); 

    resize_main_window_r(terminalwnd, size.ws_row, size.ws_col);
    //wresize(mainwnd.mainwnd, size.ws_row, size.ws_col);
    // Обновляем содержимое окна после изменения размеров
    //wclear(mainwnd.mainwnd);
    //mvwprintw(mainwnd.mainwnd, 0, 0, "Resize detected! New size: %d rows, %d columns", size.ws_row, size.ws_col);
    //wrefresh(mainwnd.mainwnd);
    (void)signo; // For no warnings
}


int main() {

    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    start_color();
    keypad(stdscr, false);
    refresh();

    // ======== Main Window
    create_main_window_r(&terminalwnd, 
        &terminalwnd_h, &terminalwnd_w, 
        terminalwnd_y, terminalwnd_x);

    // ======== Command Window
    // === Command
    create_new_window_r(&commandwnd, 
        COMMAND_WINDOW_HEIGHT, terminalwnd_w, 
        terminalwnd_h-COMMAND_WINDOW_HEIGHT, 0);
    // === Plank
    create_new_window_r(&coommandPlankwnd, 
        PLANK_HEIGHT, terminalwnd_w, 
        commandwnd.main_y - PLANK_HEIGHT, 0);
    set_text_centered(&coommandPlankwnd, "Command/chat window");
    wrefresh(coommandPlankwnd.subwnd);

    // ======== List Window
    // === Plank
    create_new_window_r(&listPlankwnd, 
        PLANK_HEIGHT, USERS_LIST_WIDTH, 
        0, terminalwnd_w-USERS_LIST_WIDTH);
    set_text_centered(&listPlankwnd, "Users List");
    wrefresh(listPlankwnd.subwnd);
    // === List
    create_new_window_r(&listwnd, 
         terminalwnd_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2, 
         USERS_LIST_WIDTH, 
         PLANK_HEIGHT, terminalwnd_w-USERS_LIST_WIDTH);

    // ======== Chat Window
    // === Chat Plank
    create_new_window_r(&chatPlankwnd, 
         PLANK_HEIGHT, terminalwnd_w - listPlankwnd.main_w, 
         0, 0);
    set_text_centered(&chatPlankwnd, "Chat history");
    wrefresh(chatPlankwnd.subwnd);
    // === Chat
    create_new_window_r(&chatwnd, 
         terminalwnd_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2, 
         terminalwnd_w - listPlankwnd.main_w, 
         PLANK_HEIGHT, 0);

    char ch = getch();

    delwin(terminalwnd);
    endwin();
    return 0;
}