#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <network/client.h>
#include <network/protocol.h>

void client_socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    client_connect_socket(fd);
    client_do_something(fd);
}

void client_connect_socket(int fd)
{
    struct sockaddr_in addr = create_address(1);

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        perror("client_connect_socket()");
        exit(1);
    }
}

void client_do_something(int fd)
{
    const char msg[] = "hello";
    uint32_t len = strlen(msg);

    // Send length first (4 bytes)
    int32_t err = read_or_write_full(fd, (char *)&len, 4, WRITE);
    if (err)
    {
        perror("write length error");
        return;
    }

    // Send message data
    err = read_or_write_full(fd, (char *)msg, len, WRITE);
    if (err)
    {
        perror("write message error");
        return;
    }

    // Read server's response length
    char rbuf[4];
    err = read_or_write_full(fd, rbuf, 4, READ);
    if (err)
    {
        perror("read response length error");
        return;
    }

    uint32_t reply_len;
    memcpy(&reply_len, rbuf, 4);

    // Read server's response message
    if (reply_len > 0 && reply_len < 1024)
    {
        char reply[1024];
        err = read_or_write_full(fd, reply, reply_len, READ);
        if (err)
        {
            perror("read response message error");
            return;
        }
        reply[reply_len] = '\0';
        printf("Server says: %s\n", reply);
    }

    close(fd);
}