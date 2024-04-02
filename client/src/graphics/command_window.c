#include "command_window.h"

const char* promt = ":>";
const char* cmd_exit = "/exit";

/*Ввести имя (до max_name_size-1 символов)
Возыращает:
- 0 - всё ок
- -1 - запрошена команда cmd_exit
- -2 - если строка содержит недопустимые символы ('[' или ']')
- EG_CRIRICAL - критическая ошибка*/
int request_name(Window* wnd, char* promt_text, char* name, int max_name_size){
    // Очищаем окно
    wclear(wnd->subwnd);
    wrefresh(wnd->subwnd);
    
    wmove(wnd->subwnd, 0, strlen(promt_text)-1);
    mvwprintw(wnd->subwnd, 0, 0, "%s%s ", promt_text, promt);
    if (wgetnstr(wnd->subwnd, name, max_name_size) == ERR){
        #ifdef ETRACE_CLIENT_GRAPHICS
            perror("wgetnstr() for input in request_name()");
        #endif
        return EG_CRIRICAL;
    }

    // Проверяю на соответствие команде cmd_exit
    if (strncmp(name, cmd_exit, strlen(cmd_exit)+1) != 0){
        // Ищу недопустимые символы ('[' или ']')
        char *ptr = strpbrk(name, "[]");
        if (ptr == NULL) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -2;
    }

}

void create_window_command_r(Window* wnd){
    create_std_window(wnd);
    refresh_std_window_r(wnd);
}

void delete_window_command_r(Window* wnd){
    delete_std_window_r(wnd);
}

void create_window_command_plank_r(Window* wnd){
    create_std_plank(wnd, "Command/chat window");
    refresh_std_window_r(wnd);
}

void delete_window_command_plank_r(Window *wnd){
    delete_std_window_r(wnd);
}