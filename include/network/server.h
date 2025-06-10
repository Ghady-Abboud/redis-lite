#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "network/commonSocket.h"

#define MAX_CONNECTIONS 100
#define MAX_REQUEST_SIZE 4096

struct Commands
{
    char **commands;
    size_t count;
    size_t capacity;
};

void socket_init();
int bind_socket(int fd);
void listen_socket(int fd, int rv);
int32_t one_request(int connfd);
void fd_set_nonblocking(int fd);
struct Conn *handle_accept(int fd);
void handle_write(struct Conn *conn);
void handle_read(struct Conn *conn);
bool try_one_request(struct Conn *conn);
int32_t parse_req(const uint8_t *data, size_t size, struct Commands *commands);
bool read_u32(const uint8_t **data, const uint8_t *end, uint32_t *out);
bool read_str(const uint8_t **data, const uint8_t *end, size_t len, char *out);

#endif