#include <sys/socket.h>

#ifndef SERVER_H
#define SERVER_H

#define PORT 1234
#define DOMAIN AF_INET

struct sockaddr_in
{
    u_int16_t sin_family;
    u_int16_t sin_port;
    struct in_addr sin_addr;
};

struct in_addr
{
    u_int32_t s_addr;
};

int socket_init();
void bind_socket(int *fd);

#endif