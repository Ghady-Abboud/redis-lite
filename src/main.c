#include <stdio.h>
#include <string.h>
#include "hash/crc.h"

#define MAX_LINE_LENGTH 256

char *command;
char *commandType;

int main()
{
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        command = strtok(buffer, " ");
        strcpy(commandType, &command[0]);
        if (strcmp(commandType, "g") == 0)
        {
        }
        else if (strcmp(commandType, "s") == 0)
        {
        }
    }
}