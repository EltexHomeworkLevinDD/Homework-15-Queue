CFLAGS := -Wall -Wextra -g
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := ../bin

# Файлы исходного кода
MSGTYPES_H := ../msgtypes.h
MSG1 := client_messaging
MSG2 := client_control_messaging
MSG3 := client_text_messaging
CLIENT := client
THREADS := client_threads

.PHONY: all clean

all: $(BIN_DIR)/$(CLIENT)

# Создание директорий
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Компиляция MSG
$(OBJ_DIR)/$(MSG1).o: $(SRC_DIR)/$(MSG1).c $(SRC_DIR)/$(MSG1).h $(MSGTYPES_H) | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(MSG2).o: $(SRC_DIR)/$(MSG2).c $(SRC_DIR)/$(MSG2).h | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(MSG3).o: $(SRC_DIR)/$(MSG3).c $(SRC_DIR)/$(MSG3).h | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@

# Компиляция THREADS
$(OBJ_DIR)/$(THREADS).o: $(SRC_DIR)/$(THREADS).c $(SRC_DIR)/$(THREADS).h | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@ -lpthread

# Компиляция CLIENT
$(OBJ_DIR)/$(CLIENT).o: $(SRC_DIR)/$(CLIENT).c | $(OBJ_DIR)
	gcc $(CFLAGS) -c $< -o $@

# Сборка
$(BIN_DIR)/$(CLIENT): $(OBJ_DIR)/$(MSG1).o $(OBJ_DIR)/$(MSG2).o $(OBJ_DIR)/$(MSG3).o $(OBJ_DIR)/$(THREADS).o $(OBJ_DIR)/$(CLIENT).o | $(BIN_DIR)
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm $(BIN_DIR)/$(CLIENT)
