#include "window_types.h"

char history[MAX_HISTORY_SIZE];
int history_size = 0;

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

void set_text_centered(Window* plank, char* text){
    int len = strlen(text);
    int center = plank->sub_w/2 - len/2;
    mvwprintw(plank->subwnd, 0, center, "%s", text);
}

void create_std_window(Window* wnd){
    wnd->mainwnd = newwin(wnd->main_h, wnd->main_w, wnd->main_y, wnd->main_x);
    wnd->subwnd = derwin(wnd->mainwnd, wnd->sub_h, wnd->sub_w, 1, 1);
    box(wnd->mainwnd, 0,0);
}

void create_std_plank(Window* wnd, char* text){
    create_std_window(wnd);
    set_text_centered(wnd, text);
}

void refresh_std_window_r(Window* wnd){
    wrefresh(wnd->mainwnd);
    wrefresh(wnd->subwnd);
}

void delete_std_window_r(Window* wnd){
    wclear(wnd->mainwnd);
    wclear(wnd->subwnd);

    refresh_std_window_r(wnd);

    delwin(wnd->subwnd);
    delwin(wnd->mainwnd);
}