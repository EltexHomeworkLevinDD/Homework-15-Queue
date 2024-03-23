# Compiler
CC := gcc
# Compiler flags
CFLAGS := -Wall -Wextra -g
# Source directory
SRC_DIR := ./src
# Object directory
OBJ_DIR := ./obj
# Binary directory
BIN_DIR := ./bin
# Target executable name
TARGET := main

# Find all source files in the source directory
SRCS := $(wildcard $(SRC_DIR)/*.c)
# Generate object file names from source files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

-include $(OBJS:.o=.d)

# $(OBJS) - это переменная в Makefile, которая содержит список всех объектных файлов вашего проекта.
# $(OBJS:.o=.d) - это выражение замены (substitution), которое говорит Make, чтобы для каждого элемента 
#	в $(OBJS) заменить расширение .o на .d.
# Таким образом, если $(OBJS) содержит файлы "file1.o", "file2.o" и т.д., то $(OBJS:.o=.d) превратит их 
#	в "file1.d", "file2.d" и т.д.
# "-include" - это директива Make, которая указывает на то, что нужно включить файлы в Makefile, если 
#	они существуют. Если файл не существует или произойдет ошибка при его включении, Make продолжит 
#	выполнение без ошибки.

#-MMD: Эта опция указывает компилятору создавать файлы зависимостей (обычно с расширением .d) во время 
#	компиляции. Эти файлы содержат информацию о зависимостях между исходными файлами и заголовочными 
#	файлами. Таким образом, если какой-либо из заголовочных файлов изменится, Make будет знать, какие 
#	файлы нужно перекомпилировать. Файлы зависимостей обычно используются совместно с опцией -include 
#	в Makefile для автоматического включения в Makefile.

#-MP: Эта опция указывает компилятору включать дополнительные фиктивные (dummy) цели для каждого 
#	заголовочного файла. Эти фиктивные цели обеспечивают, что Make не будет выдавать ошибку, если 
#	какой-либо из заголовочных файлов удален или перемещен. Вместо этого Make будет просто игнорировать 
#	отсутствующий файл. Это полезно для обеспечения надежной сборки проекта при изменениях в структуре 
#	проекта.

.PHONY: clean
# Default target - builds the binary file
all: $(BIN_DIR)/$(TARGET)

# Rule to build the target binary file
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build object files from source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Rule to create the object directory
$(OBJ_DIR):
	mkdir -p $@

# Rule to create the binary directory
$(BIN_DIR):
	mkdir -p $@

# Clean rule - removes object and binary directories
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)