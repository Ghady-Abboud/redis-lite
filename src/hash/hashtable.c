#include "hashtable.h"
#include <stdio.h>
#include <string.h>

Ht_item *create_item(const char *key, const char *value) {
    Ht_item *item = malloc(sizeof(Ht_item));
    item->key = malloc(strlen(key) + 1);
    item->value = malloc(strlen(value) + 1);
    strcpy(item->key, key);
    strcpy(item->value, value);
    return item;
}

HashTable *create_table(int size) {
    HashTable *table = malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = calloc(table->size, sizeof(Ht_item *));

    for (int i =0;i<table->size;i++)
        table->items[i] = NULL;
    return table;
}

void free_item(Ht_item *item) {
    free(item->key);
    free(item->value);
    free(item);
}

void free_table(HashTable *table) {
    for (int i =0;i<table->size;i++) {
        Ht_item *item = table->items[i];
        
        if (item != NULL) {
            free_item(item);
        }
    }
    free(table->items);
    free(table);
}

void print_table(HashTable *table) {
    printf("\nHash Table\n------------------\n");

    for (int i =0;i<table->size;i++) {
        Ht_item *item = table->items[i];

        if (item != NULL) {
            printf("Index:%d, Key:%s, Value:%s\n", i, item->key, item->value);   
        }
    }
    printf("------------------\n");
}