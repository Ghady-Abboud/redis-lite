#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

struct Buffer
{
    uint8_t *data;
    size_t size;
    size_t capacity;
};

int init_buffer(struct Buffer *buffer, size_t initial_capacity);
void free_buffer(struct Buffer *buffer);
int resize_buffer(struct Buffer *buffer, size_t new_capacity);
void buf_append(struct Buffer *buffer, const uint8_t *data, size_t len);
void buf_consume(struct Buffer *buffer, size_t len);

#endif