#ifndef NETWORK_COMMON_H
#define NETWORK_COMMON_H

#include <netinet/in.h>

#define PORT 6379
#define DOMAIN AF_INET

struct sockaddr_in create_address(int use_loopback);

#endif
