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
    send_multiple_requests(fd);
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

int send_multiple_requests(int fd)
{
    int32_t err = query(fd, "hello1");
    if (err)
        goto L_DONE;

    err = query(fd, "hello2");
    if (err)
        goto L_DONE;
    err = query(fd, "hello3");
    if (err)
        goto L_DONE;
L_DONE:
    close(fd);
    return 0;
}

int32_t query(int fd, const char *text)
{
    uint32_t len = strlen(text);

    int32_t err = read_or_write_full(fd, (char *)&len, 4, WRITE);
    if (err)
        return err;

    err = read_or_write_full(fd, (char *)text, len, WRITE);
    if (err)
        return err;

    char rbuf[4];
    err = read_or_write_full(fd, rbuf, 4, READ);
    if (err)
        return err;

    uint32_t reply_len;
    memcpy(&reply_len, rbuf, 4);

    if (reply_len > 0 && reply_len < 1024)
    {
        char reply[1024];
        err = read_or_write_full(fd, reply, reply_len, READ);
        if (err)
            return err;

        reply[reply_len] = '\0';
        printf("Server replied: %s\n", reply);
    }

    return 0;
}