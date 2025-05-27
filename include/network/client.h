#ifndef CLIENT_H
#define CLIENT_H

#include "network/common.h"

void client_socket_init();
void client_connect_socket(int fd);
int send_multiple_requests(int fd);
int32_t query(int fd, const char *text);

#endif