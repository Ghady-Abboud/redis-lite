#include <arpa/inet.h>
#include "network/commonSocket.h"

struct sockaddr_in create_address(int use_loopback)
{
    struct sockaddr_in addr = {};
    addr.sin_family = DOMAIN;
    addr.sin_port = htons(PORT);

    if (use_loopback)
    {
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    else
    {
        addr.sin_addr.s_addr = htonl(0);
    }

    return addr;
}
