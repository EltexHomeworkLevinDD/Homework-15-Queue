#include "chat_window.h"

void create_window_chat_r(Window* wnd){
    create_std_window(wnd);
    refresh_std_window_r(wnd);
}

void delete_window_chat_r(Window* wnd){
    delete_std_window_r(wnd);
}

void create_window_chat_plank_r(Window* wnd){
    create_std_plank(wnd, "Chat");
    refresh_std_window_r(wnd);
}

void delete_window_chat_plank_r(Window *wnd){
    delete_std_window_r(wnd);
}