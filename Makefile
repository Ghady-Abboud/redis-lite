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

# ––– Run server with Valgrind
.PHONY: valgrind-server
valgrind-server: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 $(BUILD)/main server

# ––– Run client with Valgrind
.PHONY: valgrind-client
valgrind-client: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 $(BUILD)/main client

# ––– Run server with Valgrind and save to file
.PHONY: valgrind-server-log
valgrind-server-log: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 --log-file=valgrind-server.txt $(BUILD)/main server

# ––– Run client with Valgrind and save to file
.PHONY: valgrind-client-log
valgrind-client-log: $(BUILD)/main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 --log-file=valgrind-client.txt $(BUILD)/main client

# ––– Convenience targets for normal execution
.PHONY: server client
server: $(BUILD)/main
	$(BUILD)/main server

client: $(BUILD)/main
	$(BUILD)/main client