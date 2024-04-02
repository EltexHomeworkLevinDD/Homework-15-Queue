#ifndef WINDOW_TYPES_H
#define WINDOW_TYPES_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <malloc.h>
#include "../../../include/messaging_types.h"

#define EG_CRIRICAL -3

#define PLANK_HEIGHT 3
// Должен быть больше или равен MAX_OWN_NAME_SIZE+2
#define USERS_LIST_WIDTH (MAX_OWN_NAME_SIZE+2+5)
#define COMMAND_WINDOW_HEIGHT 5

typedef struct {
    WINDOW* mainwnd;
    WINDOW* subwnd;
    int main_x;
    int main_y;
    int main_h;
    int main_w;
    int sub_x;
    int sub_y;
    int sub_h;
    int sub_w;
} Window;

struct Message {
    char text[MAX_TEXT_MSG_SIZE]; // Максимальный размер текста в передаваемом сообщении (не включая собственное имя)
    int text_size; // Реальный размер строки до '\0' включительно
    int lines; // Количество строк, занимаемых сообщением
};

extern char own_name[];
extern int own_name_length;
extern int fullmessage_size;
extern char history[];
extern int history_size;

extern WINDOW* terminalwnd;
extern int terminalwnd_h;
extern int terminalwnd_w;
extern int terminalwnd_y;
extern int terminalwnd_x;

extern Window chatPlankwnd;
extern Window chatwnd;
extern Window listwnd;
extern Window listPlankwnd;
extern Window commandwnd;
extern Window coommandPlankwnd;

void create_new_window_r(Window* wnd, int h, int w, int y, int x);
void delete_window(Window* wnd);

void create_main_window_r(WINDOW** wnd, int* h, int* w, int y, int x);
void resize_main_window_r(WINDOW* wnd, int h, int w);

int create_message(char* text, int size, char** fullmessage);
int save_message_to_history(char* fullmessage, int size);

void set_text_centered(Window* plank, char* text);
void set_window_parameters(Window* wnd, int h, int w, int y, int x);


#endif//WINDOW_TYPES_H