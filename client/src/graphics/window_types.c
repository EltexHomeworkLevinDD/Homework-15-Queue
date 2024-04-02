#include "window_types.h"

char history[MAX_HISTORY_SIZE];
int history_size = 0;

WINDOW* terminalwnd;
int terminalwnd_h = 0;
int terminalwnd_w = 0;
int terminalwnd_y = 0;
int terminalwnd_x = 0;

Window chatPlankwnd;
Window chatwnd;

Window listwnd;
Window listPlankwnd;

Window commandwnd;
Window coommandPlankwnd;

void set_window_parameters(Window* wnd, int h, int w, int y, int x){
    wnd->main_x = x;
    wnd->main_y = y;
    wnd->main_h = h;
    wnd->main_w = w;

    wnd->sub_h = wnd->main_h - 2;
    wnd->sub_w = wnd->main_w - 2;
    wnd->sub_x = 1; // Изменено
    wnd->sub_y = 1; // Изменено
}

void create_new_window_r(Window* wnd, int h, int w, int y, int x) {
    wnd->mainwnd = newwin(h, w, y, x);
    box(wnd->mainwnd, 0, 0);
    wrefresh(wnd->mainwnd);

    set_window_parameters(wnd, h, w, y, x);

    wnd->subwnd = derwin(wnd->mainwnd, wnd->sub_h, wnd->sub_w, wnd->sub_y, wnd->sub_x);
    wrefresh(wnd->subwnd);
}

void delete_window(Window* wnd){
    wborder(wnd->subwnd, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wborder(wnd->mainwnd, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(wnd->subwnd);
    wrefresh(wnd->mainwnd);
    delwin(wnd->subwnd);
    delwin(wnd->mainwnd);
}

/*Сформировать полное сообщение ([Имя] Текст)
Принимает
- text - текст сообщения
- size - размер сообщения
- fullmessage - пустой указатель на сообщение, и
allocate him, освобождайте самостоятельно

Возвращает
- Размер сообщения
- EG_CRIRICAL в случае критической ошибки
*/
int create_message(char* text, int size, char** fullmessage){
    // Выделяю память под сообщение
    // text_size
    // own_name_length
    // +2 for '[' & ']'
    // +1 for ' ' after ']'
    int fullmessage_size = size+own_name_length+2+1;
    (*fullmessage) = malloc(fullmessage_size);
    if (*fullmessage == NULL){
        return EG_CRIRICAL;
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

int save_message_to_history(char* fullmessage, int size){
    // Если история была пуста
    if (history_size == 0){
        memcpy(history, fullmessage, size);
        history_size = size;
    }
    // Если история уже была
    else {
        int len = size - 1;
        // Если История не заполнилась

        // Сдвигаем историю вправо на len, исключая '\0' в fullmessage
        memmove(history+len, history, history_size-len);
        history_size = MAX_HISTORY_SIZE;

        // Если история + сообщение < MAX_HISTORY_SIZE
        if ((history_size + len) < MAX_HISTORY_SIZE){
            history_size += len;
        } 
        // Если история + сообщение >= MAX_HISTORY_SIZE
        else{
            history_size = MAX_HISTORY_SIZE;
        }

        // Добавляем '\0' в конце history
        history[history_size] = '\0';
        // Копируем сообщение fullmessage в начало history
        memcpy(history, fullmessage, len);
    }
    return 0;
}

void create_main_window_r(WINDOW** wnd, int* h, int* w, int y, int x){
    *wnd = newwin(*h, *w, y, x);
    box(*wnd, 0,0);

    // Если был передан 0,0, то получаю размеры окна
    if (y == 0 && x == 0) {
        getmaxyx(*wnd, *h, *w);
    }

    wrefresh(*wnd);
}

void resize_main_window_r(WINDOW* wnd, int h, int w){
    wclear(wnd);
    wresize(wnd, h, w);
    box(wnd, 0,0);
    wrefresh(wnd);
}

void set_text_centered(Window* plank, char* text){
    int len = strlen(text);
    int center = plank->sub_w/2 - len/2;
    mvwprintw(plank->subwnd, 0, center, "%s", text);
}
