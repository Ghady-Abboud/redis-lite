#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include "crc.h"

#define HASH_TABLE_SIZE 1000

typedef struct Node
{
    char *key;
    char *value;
    struct Node *next;

} Node;

typedef struct HashMap
{
    int numOfElements, capacity;
    Node **arr;

} HashMap;

void initializeHashMap(HashMap *hashMap);
void insert(HashMap *hashMap, char *key, char *value);
void search(HashMap *hashMap, char *key);
#endif