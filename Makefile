CC = gcc
CFLAGS = -Wall -g -Iinclude -Ideps/list
TARGET = main
SRC_DIR = src
DEPS_DIR = deps
OBJ_DIR = obj
BIN_DIR = bin
SRCS := $(wildcard $(SRC_DIR)/*.c)
DEPS_SRCS := $(wildcard $(DEPS_DIR)/*/*.c)
SOURCES := $(SRCS) $(DEPS_SRCS)
OBJECTS := $(SOURCES:%.c=$(OBJ_DIR)/%.o)
HEADERS := $(wildcard include/*.h) $(wildcard $(DEPS_DIR)/*/*.h)
TARGET_EXEC = $(BIN_DIR)/$(TARGET)

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: %.c $(HEADERS) | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $@

debug: CFLAGS += -DDEBUG
debug: $(TARGET_EXEC)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all debug clean
