#ifndef NETWORK_COMMON_H
#define NETWORK_COMMON_H

#include <netinet/in.h>

#define PORT 1234
#define DOMAIN AF_INET

struct sockaddr_in create_address(int use_loopback);

#endif
