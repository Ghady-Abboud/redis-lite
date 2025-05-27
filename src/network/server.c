#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network/server.h"
#include "network/protocol.h"

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
        while (1)
        {
            int32_t err = one_request(connfd);
            if (err)
            {
                break;
            }
        }
        close(connfd);
        printf("Connection closed\n");
    }
}

int bind_socket(int fd)
{
    struct sockaddr_in addr = create_address(0);
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

int32_t one_request(int connfd)
{
    // 4 bytes header
    char rbuf[K_MAX_HEADER + K_MAX_MSG];
    errno = 0;

    int32_t err = read_or_write_full(connfd, rbuf, K_MAX_HEADER, READ);
    if (err)
    {
        if (errno == 0)
        {
            perror("Reached EOF");
        }
        else
        {
            perror("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4);
    if (len > K_MAX_MSG)
    {
        perror("Message too long");
        return -1;
    }

    err = read_or_write_full(connfd, &rbuf[K_MAX_HEADER], len, READ);
    if (err)
    {
        perror("read() error");
        return err;
    }

    printf("Client says: %.*s\n", len, &rbuf[K_MAX_HEADER]);

    // Reply
    const char reply[] = "Hello from server";
    char wbuf[K_MAX_HEADER + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[K_MAX_HEADER], reply, len);

    return read_or_write_full(connfd, wbuf, K_MAX_HEADER + len, WRITE);
}