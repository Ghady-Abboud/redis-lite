#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "parser.h"

void parse_line(char *line, HashTable *ht);
void set_command(char *line, HashTable *ht);
// void clean_line(char *line);

void parse_line(char *line, HashTable *ht) {
    if (line == NULL || strlen(line) == 0)
        return;
    // clean_line(line);
    char *cmd = strtok(line, " ");
    if (strcmp(cmd, "set") == 0) {
        set_command(line, ht);
    } else if (strcmp(cmd, "get") == 0) {
        get_command(line, ht);
    } else if (strcmp(cmd, "del") == 0) {
        del_command(line, ht);
    }
}

void set_command(char *line, HashTable *ht) {
    char *key = strtok(NULL, " ");
    char *val = strtok(NULL, " ");

    if (key && val) {
        ht_insert(ht, key, val);
    } else {
        printf("Error: 'set' command requires a key and a value.\n");
        return;
    }
    printf("\\OK\\\n");
}

void get_command(char *line, HashTable *ht) {
    char *key = strtok(NULL, " ");
    if (!key) {
        printf("Error: 'get' command requires a key.\n");
        return;
    }
    char *value = ht_search(ht, key);
    printf("\\%s\\\n", value ? value : "NULL");
}

void del_command(char *line, HashTable *ht) {
    char *key = strtok(NULL, " ");
    if (!key) {
        printf("Error: 'del' command requires a key.\n");
        return;
    }
    ht_delete(ht, key);
    printf("\\OK\\\n");
}

/*
void clean_line(char *line) {
    char *start = line;
    while (*start && isspace(*start)) start++;

    char *end = start + strlen(start) - 1;
}
*/