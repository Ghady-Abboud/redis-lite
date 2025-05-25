#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network/server.h"

void socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    int rv = bind_socket(fd);
    listen_socket(fd, rv);

    while (1)
    {
        struct sockaddr_in client_addr = {};
        socklen_t addr_len = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
        if (connfd < 0)
        {
            continue;
        }
        do_something(connfd);
        close(connfd);
        printf("Connection closed\n");
    }
}

int bind_socket(int fd)
{
    struct sockaddr_in addr = {};

    addr.sin_family = DOMAIN;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(0);
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        perror("bind()");
        exit(1);
    }
    return rv;
}

void listen_socket(int fd, int rv)
{
    rv = listen(fd, SOMAXCONN);
    if (rv)
    {
        perror("listen()");
        exit(1);
    }
}

void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        perror("read() error");
        return;
    }
    printf("Client says: %s\n", rbuf);

    char wbuf[64] = "Hello from server!\n";
    write(connfd, wbuf, strlen(wbuf));
}