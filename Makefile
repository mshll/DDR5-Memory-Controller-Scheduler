CC = gcc
CFLAGS = -Wall -g -Iinclude
TARGET = main
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HEADERS := $(wildcard include/*.h)
TARGET_EXEC = $(BIN_DIR)/$(TARGET)

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

debug: CFLAGS += -DDEBUG
debug: $(TARGET_EXEC)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all debug clean
