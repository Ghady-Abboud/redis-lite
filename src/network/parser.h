#ifndef PARSER_H
#define PARSER_H

#include "hashtable.h"

void parse_line(char *line, HashTable *ht);
void set_command(char *line, HashTable *ht);
void get_command(char *line, HashTable *ht);
void del_command(char *line, HashTable *ht);


#endif