CFLAGS := -Wall -Wextra -g
OBJ_DIR := ../../obj

# Файлы исходного кода
MSGTYPES_H := ../../../include/messaging_types.h
MSG := client_threads.c

.PHONY: all clean

all: $(OBJ_DIR)/client_threads.o

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/client_threads.o: client_threads.c | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ_DIR)/client_threads.o