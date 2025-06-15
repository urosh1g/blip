CC = gcc

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
INCLUDE_DIR = include
TEST_DIR = tests

DEBUG ?= 1

CFLAGS  = -Wall -Wextra -Werror -Wfatal-errors -std=c11
LDFLAGS = -Wl,-rpath=./$(LIB_DIR) -L$(LIB_DIR)
LDARGS  = -lcimgui -lglfw3 -lm

ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
endif

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_EXECUTABLES = $(TEST_FILES:$(TEST_DIR)/%.c=$(TEST_DIR)/%.out)
EXECUTABLE = main

.PHONY: all

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDARGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@ $(LDFLAGS) $(LDARGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

compile_commands.json:
	@if command -v bear >/dev/null 2>&1; then \
		echo "Generating compile_commands.json with bear..."; \
		bear -- make clean all; \
	else \
		echo "ERROR: 'bear' not installed. Please install 'bear' from your package manager or https://github.com/rizsotto/Bear"; \
		exit 1; \
	fi

clang-format:
	@if [ ! -f .clang-format ]; then \
		echo "Creating .clang-format..."; \
		printf "BasedOnStyle: llvm\nIndentWidth: 4\nTabWidth: 4\nUseTab: Never\nSortIncludes: false\nPointerAlignment: Left" > .clang-format; \
	fi

configure: compile_commands.json clang-format

tests: $(TEST_EXECUTABLES) $(OBJ_DIR)/entity.o 

$(TEST_DIR)/%.out: $(TEST_DIR)/%.c $(OBJ_DIR)/entity.o $(OBJ_DIR)/glb_parser.o $(OBJ_DIR)/model_loader.o $(OBJ_DIR)/logger.o 
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -L$(LIB_DIR) $^ -o $@ $(LDARGS)

clean:
	- rm -rf $(OBJ_DIR) $(EXECUTABLE) $(TEST_EXECUTABLES) .cache/ compile_commands.json
