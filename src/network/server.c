#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

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

        for (size_t i = 1; i < poll_count; ++i)
        {
            uint32_t ready = poll_args[i].revents;
            struct Conn *conn = fd_to_conn[poll_args[i].fd];
            if (ready && POLLIN)
                handle_read(conn);
            if (ready && POLLOUT)
                handle_write(conn);

            if ((ready && POLLERR) || conn->want_close)
            {
                (void)close(conn->fd);
                fd_to_conn[conn->fd] = NULL;
                free(conn);
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

    const char reply[] = "Hello from server";
    char wbuf[K_MAX_HEADER + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[K_MAX_HEADER], reply, len);

    return read_or_write_full(connfd, wbuf, K_MAX_HEADER + len, WRITE);
}

void fd_set_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

struct Conn *handle_accept(int fd)
{
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0)
        return NULL;

    fd_set_nonblocking(connfd);

    struct Conn *conn = malloc(sizeof(struct Conn));
    conn->fd = connfd;
    conn->want_read = true;
    conn->want_write = false;
    conn->want_close = false;

    if (init_buffer(&conn->incoming, 1024) != 0)
    {
        free(conn);
        close(connfd);
        return NULL;
    }

    if (init_buffer(&conn->outgoing, 1024) != 0)
    {
        free_buffer(&conn->incoming);
        free(conn);
        close(connfd);
        return NULL;
    }

    return conn;
}

bool try_one_request(struct Conn *conn)
{
    if (conn->incoming.size < K_MAX_HEADER)
        return false;

    uint32_t len = 0;
    memcpy(&len, conn->incoming.data, K_MAX_HEADER);
    if (len > K_MAX_MSG)
    {
        conn->want_close = true;
        return false;
    }

    if (K_MAX_HEADER + len > conn->incoming.size)
        return false;

    const uint8_t *request = &conn->incoming.data[K_MAX_HEADER];

    buf_append(&conn->outgoing, (const uint8_t *)&len, K_MAX_HEADER);
    buf_append(&conn->outgoing, request, (size_t)len);
    buf_consume(&conn->incoming, K_MAX_HEADER + len);
    return true;
}

void handle_read(struct Conn *conn)
{
    uint8_t buf[64 * 1024];
    ssize_t rv = read(conn->fd, buf, sizeof(buf));
    if (rv <= 0)
    {
        conn->want_close = true;
        return;
    }

    buf_append(&conn->incoming, buf, (size_t)rv);
    try_one_request(conn);

    if (conn->outgoing.size > 0)
    {
        conn->want_read = false;
        conn->want_write = true;
    }
}

void handle_write(struct Conn *conn)
{
    assert(conn->outgoing.size > 0);
    ssize_t rv = write(conn->fd, conn->outgoing.data, conn->outgoing.size);
    if (rv < 0)
    {
        conn->want_close = true;
        return;
    }

    buf_consume(&conn->outgoing, (size_t)rv);

    if (conn->outgoing.size == 0)
    {
        conn->want_read = true;
        conn->want_write = false;
    }
}