#include <stdlib.h>

#include "network/buffer.h"

int init_buffer(struct Buffer *buffer, size_t initial_capacity)
{
    buffer->data = malloc(initial_capacity);
    if (!buffer->data)
        return -1;

    buffer->size = 0;
    buffer->capacity = initial_capacity;
    return 0;
}

void free_buffer(struct Buffer *buffer)
{
    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
}

int resize_buffer(struct Buffer *buffer, size_t new_capacity)
{
    if (new_capacity <= buffer->capacity)
        return -1;

    uint8_t *new_data = realloc(buffer->data, new_capacity);
    if (!new_data)
        return -1;

    buffer->data = new_data;
    buffer->capacity = new_capacity;

    return 0;
}