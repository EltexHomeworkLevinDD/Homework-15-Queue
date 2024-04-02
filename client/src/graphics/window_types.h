#ifndef WINDOW_TYPES_H
#define WINDOW_TYPES_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <malloc.h>
#include "../../../include/messaging_types.h"

#define ETRACE_CLIENT_GRAPHICS

#define EG_CRIRICAL -3

typedef struct {
    WINDOW* mainwnd;
    WINDOW* subwnd;
    int main_x;
    int main_y;
    int main_h;
    int main_w;
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


int create_message(char* text, int size, char** fullmessage);
int save_message_to_history(char* fullmessage, int size);

void set_text_centered(Window* plank, char* text);

void create_std_window(Window* wnd);
void create_std_plank(Window* wnd, char* text);
void delete_std_window_r(Window* wnd);
void refresh_std_window_r(Window* wnd);

#endif//WINDOW_TYPES_H