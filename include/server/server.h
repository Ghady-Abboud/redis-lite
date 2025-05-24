#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef SERVER_H
#define SERVER_H

#define PORT 1234
#define DOMAIN AF_INET

void main_socket();
int socket_init();
int bind_socket(int *fd);
void listen_socket(int *fd, int *rv);
#endif