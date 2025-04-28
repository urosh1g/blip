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

clean:
	- rm -rf $(OBJ_DIR) $(EXECUTABLE) .cache/ compile_commands.json
