#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include "crc.h"

#define HASH_TABLE_SIZE 65535

typedef struct Ht_item {
    char *key;
    char *value;
} Ht_item;


typedef struct HashTable {
    Ht_item **items;
    LinkedList **overflow_buckets;
    int size;
    int count;
} HashTable;

typedef struct LinkedList {
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;

Ht_item *create_item(const char *key, const char *value);
HashTable *create_table(int size);
void free_item(Ht_item *item);
void free_table(HashTable *table);
void print_table(HashTable *table);
void handle_collision (HashTable *table, unsigned long, Ht_item *item);

#endif