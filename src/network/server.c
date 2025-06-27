#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"
#include "buffer.h"
#include "parser.h"
#include "hashtable.h"
#include "metrics.h"

struct Conn
{
    int fd;

    bool want_read;
    bool want_write;
    bool want_close;

    struct Buffer incoming;
    struct Buffer outgoing;
};

static HashTable *ht;

void socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    int rv = bind_socket(fd);
    listen_socket(fd, rv);

    struct Conn **fd_to_conn = calloc(MAX_CONNECTIONS, sizeof(struct Conn *));
    size_t fd_to_conn_size = 0;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        fd_to_conn[i] = NULL;
    }

    struct pollfd *poll_args = NULL;
    size_t poll_count = 0;
    size_t poll_capacity = 0;

    ht = create_table(65530);
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

                    for (size_t j = fd_to_conn_size; j < new_size; j++)
                    {
                        fd_to_conn[j] = NULL;
                    }

                    fd_to_conn_size = new_size;
                }
                fd_to_conn[conn->fd] = conn;
            }
        }

        for (size_t i = 1; i < poll_count; ++i)
        {
            uint32_t ready = poll_args[i].revents;
            int fd = poll_args[i].fd;

            if (fd < 0 || fd >= (int)fd_to_conn_size)
                continue;

            struct Conn *conn = fd_to_conn[fd];
            if (ready & POLLIN)
                handle_read(conn);
            if (ready & POLLOUT)
                handle_write(conn);

            if ((ready & POLLERR) || conn->want_close)
            {
                fd_to_conn[conn->fd] = NULL;
                free_connection(conn);
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

void fd_set_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

bool try_one_request(struct Conn *conn)
{
    if (conn->incoming.size < 4)
        return false;

    uint32_t len = 0;
    memcpy(&len, conn->incoming.data, 4);
    if (len > 4096)
    {
        conn->want_close = true;
        return false;
    }

    if (4 + len > conn->incoming.size)
        return false;

    double start_time = get_time_ms();
    const uint8_t *request = &conn->incoming.data[4];

    char command[len + 1];
    memcpy(command, request, len);
    command[len] = '\0';

    parse_line(command, ht);

    buf_append(&conn->outgoing, (const uint8_t *)&len, 4);
    buf_append(&conn->outgoing, request, (size_t)len);

    buf_consume(&conn->incoming, 4 + len);
    conn->want_read = true;
    double end_time = get_time_ms();
    metrics_add_request(end_time - start_time);

    static uint64_t request_cnt = 0;
    request_cnt++;
    if (request_cnt % 2 == 0)
        metrics_print();
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
        free_connection(conn);
        return NULL;
    }

    if (init_buffer(&conn->outgoing, 1024) != 0)
    {
        free_connection(conn);
        return NULL;
    }

    return conn;
}

void free_connection(struct Conn *conn)
{
    if (conn == NULL)
        return;
    
    free_buffer(&conn->incoming);
    free_buffer(&conn->outgoing);
    close(conn->fd);
    free(conn);
}