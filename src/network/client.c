#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <network/client.h>

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
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n == -1)
    {
        perror("read()");
        exit(1);
    }
    printf("Server says: %s\n", rbuf);
    close(fd);
}