#ifndef SERVER_H
#define SERVER_H

#include "network/commonSocket.h"

void socket_init();
int bind_socket(int fd);
void listen_socket(int fd, int rv);
int32_t one_request(int connfd);

#endif