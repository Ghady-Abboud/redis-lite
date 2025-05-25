#ifndef CLIENT_H
#define CLIENT_H

#include "network/common.h"

void client_socket_init();
void client_connect_socket(int fd);
void client_do_something(int fd);

#endif