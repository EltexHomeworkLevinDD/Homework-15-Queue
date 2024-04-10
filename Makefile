CC := gcc
CFLAGS := -Wall -Wextra -g

MAIN_DIR := $(CURDIR)
INC_DIR := $(CURDIR)/include
SRC_DIR := $(CURDIR)/src
OBJ_DIR := $(CURDIR)/obj
BIN_DIR := $(CURDIR)/bin

.PHONY: all common server client

$(OBJ_DIR):

subdirs:
	mkdir -p $(OBJ_DIR)/server
	mkdir -p $(OBJ_DIR)/client
	mkdir -p $(OBJ_DIR)/common
	mkdir -p $(BIN_DIR)

all: common server client

common: subdirs
	$(MAKE) --no-print-directory -C $(SRC_DIR)/common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_common:
	$(MAKE) --no-print-directory -C $(SRC_DIR)/common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

server:
	$(MAKE) --no-print-directory -C $(SRC_DIR)/server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_server:
	$(MAKE) --no-print-directory -C $(SRC_DIR)/server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

client:
	$(MAKE) --no-print-directory -C $(SRC_DIR)/client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

client_server:
	$(MAKE) --no-print-directory -C $(SRC_DIR)/client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean


clean: clean_common clean_server client_server