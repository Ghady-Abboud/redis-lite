#ifndef CLIENT_H
#define CLIENT_H

#include "commonSocket.h"

void client_socket_init();
void client_connect_socket(int fd);
int send_to_server(int fd);
int32_t query(int fd, const char *text);

#endif