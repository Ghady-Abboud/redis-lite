
#include "crc.h"

uint16_t crc16(uint16_t crc, uint8_t *buffer, size_t len)
{
    while (len--)
        crc = crc16_byte(crc, *buffer++);
    return crc;
}