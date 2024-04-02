CFLAGS := -Wall -Wextra -g
OBJ_DIR := ../../obj

# Файлы исходного кода
MSGTYPES_H := ../../../include/messaging_types.h
MSG := window_types.c chat_window.c command_window.c list_window.c

.PHONY: all clean

all: $(OBJ_DIR)/window_types.o \
	$(OBJ_DIR)/chat_window.o \
	$(OBJ_DIR)/command_window.o \
	$(OBJ_DIR)/list_window.o

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/window_types.o: window_types.c window_types.h $(MSGTYPES_H) | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

$(OBJ_DIR)/chat_window.o: chat_window.c chat_window.h window_types.h | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

$(OBJ_DIR)/command_window.o: command_window.c command_window.h window_types.h | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

$(OBJ_DIR)/list_window.o: list_window.c list_window.h window_types.h | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ_DIR)/window_types.o
	rm -f $(OBJ_DIR)/chat_window.o
	rm -f $(OBJ_DIR)/command_window.o
	rm -f $(OBJ_DIR)/list_window.o