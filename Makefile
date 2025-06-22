CC = gcc
CFLAGS = -std=c11 -Wall -g -I src/hash -I src/network

# Find all .c files and create corresponding .o files
SOURCES = $(shell find src -name '*.c')
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: redis-lite

# Link the executable
redis-lite: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) redis-lite

# Run targets
server: redis-lite
	./redis-lite server

client: redis-lite
	./redis-lite client

.PHONY: all clean server client