CC = gcc
CFLAGS = -std=c11 -Wall -g -I src/hash -I src/network -I src

# Separate source files
SERVER_SOURCES = src/network/server.c src/network/protocol.c src/network/commonSocket.c src/network/buffer.c src/network/parser.c src/hash/crc.c src/hash/hashtable.c src/server_main.c src/metrics.c

CLIENT_SOURCES = src/network/client.c src/network/protocol.c src/network/commonSocket.c src/client_main.c

SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

all: redis-server redis-client

redis-server: $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

redis-client: $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJECTS) $(CLIENT_OBJECTS) redis-server redis-client

server: redis-server
	./redis-server

client: redis-client
	./redis-client

.PHONY: all clean server client