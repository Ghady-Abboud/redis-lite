#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "protocol.h"

int32_t read_or_write_full(int fd, char *buf, size_t n, int is_read)
{
    while (n > 0)
    {
        ssize_t rv;
        if (is_read)
        {
            rv = read(fd, buf, n);
        }
        else
        {
            rv = write(fd, buf, n);
        }

        if (rv <= 0)
        {
            return -1; // Error or EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}