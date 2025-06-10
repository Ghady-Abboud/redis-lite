#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define K_MAX_MSG 4096
#define K_MAX_HEADER 4
#define READ 1
#define WRITE 0

int32_t read_or_write_full(int fd, char *buf, size_t n, int is_read);

#endif