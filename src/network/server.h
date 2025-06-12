#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "commonSocket.h"

#define MAX_CONNECTIONS 100

void socket_init();
int bind_socket(int fd);
void listen_socket(int fd, int rv);
void fd_set_nonblocking(int fd);
struct Conn *handle_accept(int fd);
void handle_write(struct Conn *conn);
void handle_read(struct Conn *conn);
bool try_one_request(struct Conn *conn);
void free_connection(struct Conn *conn);

#endif