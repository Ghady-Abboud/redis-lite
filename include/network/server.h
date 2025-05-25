#ifndef SERVER_H
#define SERVER_H

#include "network/common.h"

void socket_init();
int bind_socket(int fd);
void listen_socket(int fd, int rv);
void server_do_something(int connfd);
#endif