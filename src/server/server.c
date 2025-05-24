#include <sys/socket.h>
#include "server/server.h"

int socket_init()
{
    // AF_INET - IPv4
    // SOCK_STREAM - TCP
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

void bind_socket(int *fd)
{
    struct sockaddr_in addr = {};

    addr.sin_family = DOMAIN;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(0);
    int rv = bind(*fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("bind()");
    }
}