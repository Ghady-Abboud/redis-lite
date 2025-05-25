#include <stdio.h>
#include <string.h>
#include "hash/crc.h"
#include "network/server.h"
#include "network/client.h"

#define MAX_LINE_LENGTH 256

char *command;
char *commandType;

int main()
{
    printf("Starting Redis-lite server...\n");
    socket_init();
    client_socket_init();
}