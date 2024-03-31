CFLAGS := -Wall -Wextra -g
OBJ_DIR := ../../obj

# Файлы исходного кода
MSGTYPES_H := ../../../include/messaging_types.h
MSG := client_messaging.c client_control_messaging.c client_text_messaging.c

.PHONY: all clean

all: $(OBJ_DIR)/client_messaging.o \
	$(OBJ_DIR)/client_control_messaging.o \
	$(OBJ_DIR)/client_text_messaging.o

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/client_messaging.o: client_messaging.c client_messaging.h $(MSGTYPES_H) | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

$(OBJ_DIR)/client_control_messaging.o: client_control_messaging.c client_control_messaging.h client_messaging.h | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

$(OBJ_DIR)/client_text_messaging.o: client_text_messaging.c client_text_messaging.h client_messaging.h | $(OBJ_DIR)
	gcc $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ_DIR)/client_messaging.o
	rm -f $(OBJ_DIR)/client_control_messaging.o
	rm -f $(OBJ_DIR)/client_text_messaging.o