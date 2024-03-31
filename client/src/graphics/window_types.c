#include "window_types.h"

void create_new_window_r(Window* wnd, int h, int w, int y, int x, int main_pair, int sub_pair){
    wnd->main_h = h;
    wnd->main_w = w;
    wnd->main_x = x;
    wnd->main_y = y;
    wnd->sub_h = h-2;
    wnd->sub_h = w-2;
    wnd->sub_h = x-2;
    wnd->sub_h = y-2;

    wnd->mainwnd = newwin(h, w, y, x);
    box(wnd->mainwnd, 0,0);
    if (main_pair != 0)
        wbkgd(wnd->mainwnd, COLOR_PAIR(main_pair));

    wnd->subwnd = derwin(wnd->mainwnd, h-2, w-2, y+1, x+1);
    if (sub_pair != 0)
        wbkgd(wnd->mainwnd, COLOR_PAIR(sub_pair));
    wmove(wnd->subwnd, 0,0);

    wrefresh(wnd->mainwnd);
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