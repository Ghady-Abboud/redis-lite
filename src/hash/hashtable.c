#include "hash/hashtable.h"

void initializeHashMap(HashMap *hashMap)
{
    hashMap->numOfElements = 0;
    hashMap->capacity = 100;

    hashMap->arr = (Node **)malloc(sizeof(Node *) * hashMap->capacity);
    return;
};