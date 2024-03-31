#ifndef WINDOW_TYPES_H
#define WINDOW_TYPES_H

#include <stdlib.h>
#include <ncurses.h>
#include "../../../include/messaging_types.h"

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
    int max_lines;
} Window;

struct Message {
    char text[MAX_TEXT_MSG_SIZE]; // Максимальный размер текста в передаваемом сообщении (не включая собственное имя)
    char text_size; // Реальный размер строки до '\0' включительно
    int lines; // Количество строк, занимаемых сообщением
};

void create_new_window_r(Window* wnd, int h, int w, int y, int x, int main_pair, int sub_pair);
void delete_window(Window* wnd);

#endif//WINDOW_TYPES_H