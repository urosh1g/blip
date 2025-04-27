CC = gcc

CFLAGS = -Wall -Wextra -Werror -Wfatal-errors -std=c11
LDARGS = -lglfw3 -lm

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
INCLUDE_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXECUTABLE = main

.PHONY: all

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) -L$(LIB_DIR) $^ -o $@ $(LDARGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -L$(LIB_DIR) -c $< -o $@ $(LDARGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	- rm -rf $(OBJ_DIR)/* $(EXECUTABLE)
