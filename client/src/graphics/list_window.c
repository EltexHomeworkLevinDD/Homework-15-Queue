#include "list_window.h"

void create_window_list_r(Window* wnd){
    create_std_window(wnd);
    refresh_std_window_r(wnd);
}

void delete_window_list_r(Window* wnd){
    delete_std_window_r(wnd);
}

void create_window_list_plank_r(Window* wnd){
    create_std_plank(wnd, "Users");
    refresh_std_window_r(wnd);
}

void delete_window_list_plank_r(Window *wnd){
    delete_std_window_r(wnd);
}