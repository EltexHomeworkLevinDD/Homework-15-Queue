CC ?= gcc
CFLAGS ?= -Wall -Wextra -g
MAKE_CD_OPTIONS ?= 
MAIN_DIR ?= 

CLIENT_SRC_DIR := $(MAIN_DIR)/src/client
CLIENT_INC_DIR := $(MAIN_DIR)/include/client
CLIENT_OBJ_DIR := $(MAIN_DIR)/obj/client
CLIENT_BIN_DIR := $(MAIN_DIR)/bin

.PHONY: all clean common_call

all: $(CLIENT_BIN_DIR)/client

common_call: 
	$(MAKE) --no-print-directory -C $(MAIN_DIR) common

$(CLIENT_OBJ_DIR)/transiver.o: $(CLIENT_SRC_DIR)/transiver.c \
	$(CLIENT_INC_DIR)/transiver.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/userslist.h \
	$(MAIN_DIR)/include/common/messaging.h
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/threads.o: $(CLIENT_SRC_DIR)/threads.c \
	$(CLIENT_INC_DIR)/threads.h \
	$(CLIENT_INC_DIR)/transiver.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/userslist.h \
	$(MAIN_DIR)/include/common/messaging.h \
	$(CLIENT_INC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/graphics.o: $(CLIENT_SRC_DIR)/graphics.c \
	$(CLIENT_INC_DIR)/graphics.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/userslist.h
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/client.o: $(CLIENT_SRC_DIR)/client.c $(CLIENT_INC_DIR)/transiver.h
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_BIN_DIR)/client: \
	$(CLIENT_OBJ_DIR)/transiver.o \
	$(CLIENT_OBJ_DIR)/threads.o \
	$(CLIENT_OBJ_DIR)/graphics.o \
	$(CLIENT_OBJ_DIR)/client.o | common_call
	$(CC) $(CFLAGS) $^ $(wildcard $(MAIN_DIR)/obj/common/*.o) -o $@ -lpthread -lncurses

clean:
	rm -f $(CLIENT_OBJ_DIR)/threads.o
	rm -f $(CLIENT_OBJ_DIR)/transiver.o
	rm -f $(CLIENT_OBJ_DIR)/graphics.o
	rm -f $(CLIENT_OBJ_DIR)/client.o
	rm -f $(CLIENT_BIN_DIR)/client