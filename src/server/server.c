#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "server/server.h"

void main_socket()
{
    int fd = socket_init();
    int rv = bind_socket(&fd);

    listen_socket(&fd, &rv);

    while (1)
    {
        struct sockaddr_in client_addr = {};
        socklen_t addr_len = sizeof(client_addr);
    }
}

int socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    return fd;
}

int bind_socket(int *fd)
{
    struct sockaddr_in addr = {};

    addr.sin_family = DOMAIN;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(0);
    int rv = bind(*fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        perror("bind()");
        exit(1);
    }
    return rv;
}

void listen_socket(int *fd, int *rv)
{
    *rv = listen(*fd, SOMAXCONN);
    if (rv)
    {
        perror("listen()");
        exit(1);
    }
}