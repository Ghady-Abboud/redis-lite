#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include "crc.h"

#define HASH_TABLE_SIZE 1000

typedef struct Ht_item {
    char *key;
    char *value;
} Ht_item;


typedef struct HashTable {
    Ht_item **items;
    int size;
    int count;
} HashTable;

Ht_item *create_item(const char *key, const char *value);
HashTable *create_table(int size);
void free_item(Ht_item *item);
void free_table(HashTable *table);
void print_table(HashTable *table);

#endif