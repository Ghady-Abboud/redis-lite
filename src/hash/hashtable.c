#include <stdio.h>
#include <string.h>

#include "crc.h"
#include "hashtable.h"


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

void ht_insert(HashTable *table, char *key, char *value) {
    Ht_item *item = create_item(key,value);
    int index = hash_string(0,key,strlen(key));

    Ht_item *current_item = table->items[index];
    if (current_item == NULL) {
        if (table->count == table->size) {
            printf("Insert Error: Hash Table is full\n");
            free_item(current_item);
            return;
        }

        table->items[index] = item;
        table->count++;
    } else {
        if (strcmp(current_item->key, key) == 0) {
            strcpy(table->items[index] -> value, value);
            return;
        } else {
            handle_collision(table, item);
        }
    }
}

void handle_collision(HashTable *table, Ht_item *item) {
    return ;
}

char *ht_search(HashTable *table, char *key) {
    int index = hash_string(0,key, strlen(key));
    Ht_item *item = table->items[index];

    if (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            return item->value;
        }
    }
    return NULL;
}

void print_search(HashTable *table, char *key) {
    char *val = ht_search(table, key);
    if (val == NULL) {
        printf("Key:%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Value:%s\n", key, val);
    }
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

LinkedList *allocate_list() {
    LinkedList *list = malloc(sizeof(LinkedList));
    return list;
}

LinkedList *linked_list_insert(LinkedList *list, Ht_item *item) {
    if (!list) {
        LinkedList *head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } else if (list->next == NULL) {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    return list;
}

Ht_item *linked_list_remove(LinkedList *list) {
    // Remove head node
    if (!list || !list->next) {
        return NULL;
    }

    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    Ht_item *item = NULL;
    memcpy(temp->item, item, sizeof(Ht_item));
    free(temp->item->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return item;
}

void free_linked_list(LinkedList *list) {
    LinkedList *temp;

    while (list) {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}