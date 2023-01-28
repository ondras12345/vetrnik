#include "debug.h"


size_t BufferPrint::write(uint8_t c)
{
    uint8_t buff = c;
    return write(&buff, 1);
}


size_t BufferPrint::write(const uint8_t *data, size_t size)
{
    if (size == 0) return size;

    size_t remaining = size % capacity;
    data += size - remaining;

    do
    {
        size_t copy = capacity - write_addr;
        if (copy > remaining) copy = remaining;

        // If this write passes the read pointer, move it
        // If rp is between wp (not inclusive) and wp+copy (inclusive)
        // We can assume that wp+copy-1 < capacity (no wraparound)
        if ((write_addr < read_addr || (write_addr + copy == capacity && read_addr == 0)) &&
            write_addr + copy >= read_addr)
        {
            read_addr = write_addr + copy + 1;
            read_addr %= capacity;
        }

        memcpy(buf + write_addr, data, copy);
        write_addr += copy;
        write_addr %= capacity;
        data += copy;
        remaining -= copy;
    } while (remaining > 0);

    return size;  // always return the requested size
}

BufferPrint DEBUG_buffer;

Print * DEBUG_noprint = &DEBUG_buffer;

Print * INFO = DEBUG_noprint;
Print * DEBUG_PB = DEBUG_noprint;
Print * DEBUG_MQTT = DEBUG_noprint;
