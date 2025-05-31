#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network/server.h"
#include "network/protocol.h"
#include "network/buffer.h"

struct Conn
{
    int fd;

    bool want_read;
    bool want_write;
    bool want_close;

    struct Buffer incoming;
    struct Buffer outgoing;
};

void socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    int rv = bind_socket(fd);
    listen_socket(fd, rv);

    struct Conn **fd_to_conn = malloc(sizeof(struct Conn *) * MAX_CONNECTIONS);
    size_t fd_to_conn_size = 0;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        fd_to_conn[i] = NULL;
    }

    struct pollfd *poll_args;
    size_t poll_count = 0;
    size_t poll_capacity = 0;

    while (1)
    {
        poll_count = 0;
        if (poll_capacity == 0)
        {
            poll_capacity = 16;
            poll_args = realloc(poll_args, sizeof(struct pollfd) * poll_capacity);
        }

        poll_args[poll_count++] = (struct pollfd){fd, POLLIN, 0};

        for (size_t i = 0; i < fd_to_conn_size; i++)
        {
            if (fd_to_conn[i] == NULL)
            {
                continue;
            }

            if (poll_count >= poll_capacity)
            {
                poll_capacity *= 2;
                poll_args = realloc(poll_args, sizeof(struct pollfd) * poll_capacity);
            }

            struct Conn *conn = fd_to_conn[i];
            struct pollfd pfd = {conn->fd, POLLERR, 0};

            if (conn->want_read)
                pfd.events |= POLLIN;
            if (conn->want_write)
                pfd.events |= POLLOUT;

            poll_args[poll_count++] = pfd;
        }

        int rv = poll(poll_args, (nfds_t)poll_count, -1);
        if (rv < 0 && errno == EINTR)
            continue;
        if (rv < 0)
        {
            perror("poll()");
            exit(1);
        }

        if (poll_args[0].revents)
        {
            struct Conn *conn = handle_accept(fd);
            if (conn)
            {
                if (fd_to_conn_size <= (size_t)conn->fd)
                {
                    size_t new_size = conn->fd + 1;
                    fd_to_conn = realloc(fd_to_conn, sizeof(struct Conn *) * new_size);
                    fd_to_conn_size = new_size;
                }
                fd_to_conn[conn->fd] = conn;
            }
        }
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