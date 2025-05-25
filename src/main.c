#include <stdio.h>
#include <string.h>
#include "hash/crc.h"
#include "network/server.h"
#include "network/client.h"

#define MAX_LINE_LENGTH 256

char *command;
char *commandType;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s [server|client]\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "server") == 0)
    {
        printf("Starting server...\n");
        socket_init();
    }
    else if (strcmp(argv[1], "client") == 0)
    {
        printf("Starting client...\n");
        client_socket_init();
    }
    else
    {
        printf("Invalid argument. Use 'server' or 'client'.\n");
        return 1;
    }
    client_socket_init();
}