CFLAGS := -Wall -Wextra -g
MAKE_CD_OPTIONS := --no-print-directory

BIN_DIR := ../bin
OBJ_DIR := obj
SRC_DIR := src

TARGET := client

MESSAGING_DIR := messaging
GRAPHICS_DIR := graphics
THREADS_DIR := threads

.PHONY: all clean makeobj $(BIN_DIR)/$(TARGET)

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(MESSAGING_DIR):
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(MESSAGING_DIR) all

$(GRAPHICS_DIR):
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(GRAPHICS_DIR) all

$(THREADS_DIR):
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(THREADS_DIR) all

makeobj: $(MESSAGING_DIR) $(GRAPHICS_DIR) $(THREADS_DIR)

$(BIN_DIR)/$(TARGET): $(SRC_DIR)/$(TARGET).c | $(BIN_DIR) makeobj 
	gcc $(CFLAGS) $< $(wildcard $(OBJ_DIR)/*.o) -o $@ -lncurses -lpthread

clean:
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(MESSAGING_DIR) clean
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(GRAPHICS_DIR) clean
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(THREADS_DIR) clean
	rm -f $(BIN_DIR)/$(TARGET)