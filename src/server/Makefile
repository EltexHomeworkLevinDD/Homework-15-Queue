CC ?= gcc
CFLAGS ?= -Wall -Wextra -g
MAKE_CD_OPTIONS ?= 
MAIN_DIR ?= 

SERVER_SRC_DIR := $(MAIN_DIR)/src/server
SERVER_INC_DIR := $(MAIN_DIR)/include/server
SERVER_OBJ_DIR := $(MAIN_DIR)/obj/server
SERVER_BIN_DIR := $(MAIN_DIR)/bin

.PHONY: all clean common_call

all: $(SERVER_BIN_DIR)/server

common_call: 
	$(MAKE) --no-print-directory -C $(MAIN_DIR) common

$(SERVER_OBJ_DIR)/transiver.o: $(SERVER_SRC_DIR)/transiver.c \
	$(SERVER_INC_DIR)/transiver.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/messaging.h \
	$(MAIN_DIR)/include/common/userslist.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ_DIR)/threads.o: $(SERVER_SRC_DIR)/threads.c \
	$(SERVER_INC_DIR)/threads.h \
	$(SERVER_INC_DIR)/transiver.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/messaging.h \
	$(MAIN_DIR)/include/common/userslist.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ_DIR)/server.o: $(SERVER_SRC_DIR)/server.c \
	$(SERVER_INC_DIR)/transiver.h \
	$(SERVER_INC_DIR)/threads.h \
	$(MAIN_DIR)/include/common/protocol.h \
	$(MAIN_DIR)/include/common/messaging.h \
	$(MAIN_DIR)/include/common/userslist.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_BIN_DIR)/server: \
	$(SERVER_OBJ_DIR)/transiver.o \
	$(SERVER_OBJ_DIR)/threads.o \
	$(SERVER_OBJ_DIR)/server.o | common_call
	$(CC) $(CFLAGS) $^ $(wildcard $(MAIN_DIR)/obj/common/*.o) -o $@ -lpthread

clean:
	rm -f $(SERVER_OBJ_DIR)/transiver.o
	rm -f $(SERVER_OBJ_DIR)/threads.o
	rm -f $(SERVER_OBJ_DIR)/server.o
	rm -f $(SERVER_BIN_DIR)/server
	rm -f /tmp/queue_key_file