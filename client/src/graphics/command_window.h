#ifndef COMMAND_WINDOW_H
#define COMMAND_WINDOW_H

#include "window_types.h"

extern const char* promt;

int request_name(Window* wnd, char* promt_text, char* name, int max_name_size);

void create_window_command_r(Window* wnd);
void delete_window_command_r(Window* wnd);

void create_window_command_plank_r(Window* wnd);
void delete_window_command_plank_r(Window *wnd);

#endif//COMMAND_WINDOW_H