
#include "crc.h"

uint16_t hash_string(uint16_t crc, char *buffer, size_t len)
{
    while (len--)
        crc = crc16_byte(crc, *buffer++);
    return crc;
}