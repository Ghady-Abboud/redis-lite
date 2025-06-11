#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include "crc.h"

#define HASH_TABLE_SIZE 65535

typedef struct Ht_item {
    char *key;
    char *value;
} Ht_item;

typedef struct LinkedList {
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;

typedef struct HashTable {
    Ht_item **items;
    LinkedList **overflow_buckets;
    int size;
    int count;
} HashTable;


Ht_item *create_item(const char *key, const char *value);
HashTable *create_table(int size);
void free_item(Ht_item *item);
void free_table(HashTable *table);
void print_table(HashTable *table);
void handle_collision (HashTable *table, unsigned long, Ht_item *item);
char *ht_search(HashTable *table, char *key);
void print_search(HashTable *table, char *key);
LinkedList *allocate_list();
LinkedList *linked_list_insert(LinkedList *list, Ht_item *item);
Ht_item *linked_list_remove(LinkedList *list);
void free_linked_list(LinkedList *list);
LinkedList **create_overflow_buckets(HashTable *table);
void free_overflow_buckets(HashTable *table);

#endif