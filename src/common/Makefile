CC ?= 
CFLAGS ?= 
MAKE_CD_OPTIONS ?= 

SRC_DIR := $(CURDIR)
INC_DIR := $(subst src,include,$(SRC_DIR))
OBJ_DIR := $(subst src,obj,$(SRC_DIR))

.PHONY: all clean
all: $(OBJ_DIR)/messaging.o $(OBJ_DIR)/userslist.o

$(OBJ_DIR)/messaging.o: $(SRC_DIR)/messaging.c \
	$(INC_DIR)/messaging.h \
	$(INC_DIR)/protocol.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/userslist.o: $(SRC_DIR)/userslist.c \
	$(INC_DIR)/userslist.h \
	$(INC_DIR)/protocol.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/messaging.o
	rm -f $(OBJ_DIR)/userslist.o
