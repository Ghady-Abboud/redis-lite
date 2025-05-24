# ––– Tools & flags
CC      := gcc
CFLAGS  := -std=c11 -Wall -Iinclude
DEBUG   := -g

# ––– Folders
BUILD   := build
SRC     := src
INCLUDE := include

# ––– Find all source files (including subdirectories)
SRC_FILES := $(shell find $(SRC) -name '*.c')
OBJ_FILES := $(SRC_FILES:$(SRC)/%.c=$(BUILD)/%.o)

# ––– Default target: build the executable "build/main"
all: $(BUILD)/main

# ––– Link step
$(BUILD)/main: $(OBJ_FILES) | $(BUILD)
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $^

# ––– Compile step: src/*.c → build/*.o
$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

# ––– Create build/ dir if missing
$(BUILD):
	mkdir -p $@

# ––– Clean all build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD)

# ––– Run with Valgrind and show output in real-time
.PHONY: valgrind-live
valgrind-live: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./$(BUILD)/main

# ––– Run with Valgrind and save output to file
.PHONY: valgrind
valgrind: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 --log-file=valgrind-out.txt ./$(BUILD)/main