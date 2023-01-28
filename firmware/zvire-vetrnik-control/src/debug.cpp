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


void BufferPrint::print_out(Print * response)
{
    if (response == nullptr) return;
    while (read_addr != write_addr)
    {
        response->write(buf[read_addr]);
        read_addr++;
        read_addr %= capacity;
    }
}


class MultiPrint : public Print {
    public:
        /**
         * Create a new MultiPrint
         * @param printers nullptr terminated array of pointers to Print
         *                 objects
         */
        explicit MultiPrint (Print * printers[]) : _printers(printers) { }

        size_t write(uint8_t c)
        {
            uint8_t buff = c;
            return write(&buff, 1);
        }

        size_t write(const uint8_t *data, size_t size)
        {
            for (Print ** prt = _printers; *prt != nullptr; prt++)
            {
                (*prt)->write(data, size);
            }
            return size;
        }

    protected:
        Print ** _printers;
};


/*
 * At boot, everything is logged to DEBUG_buffer.
 * After Serial and telnet backends are added, INFO is directed both to them
 * and to DEBUG_buffer.
 * When debug is enabled for either one of DEBUG_PB and DEBUG_MQTT,
 * it is no longer logged to DEBUG_buffer, but instead to the backends
 * registered for INFO (but not INFO itself, as that would also log to
 * DEBUG_buffer).
 */


BufferPrint DEBUG_buffer;

static Print * INFO_backends[5] = {
    nullptr,
};

static MultiPrint INFO_multiprint(INFO_backends);  // without buffer

static Print * INFO_final[] = {
    &INFO_multiprint,
    &DEBUG_buffer,
    nullptr,
};

static MultiPrint INFO_final_mp(INFO_final);  // with buffer


void log_add_INFO_backend(Print * printer)
{
    if (printer == nullptr) return;
    uint8_t i = 0;
    for (; INFO_backends[i] != nullptr; i++);
    if (i + 1 >= sizeof(INFO_backends)/sizeof(INFO_backends[0])) return;
    INFO_backends[i] = printer;
    INFO_backends[i+1] = nullptr;
}


Print * INFO = &INFO_final_mp;
Print * DEBUG_PB = &DEBUG_buffer;
Print * DEBUG_MQTT = &DEBUG_buffer;



void log_set_DEBUG_PB(bool v)
{
    if (v) DEBUG_PB = &INFO_multiprint;
    else DEBUG_PB = &DEBUG_buffer;
}


void log_set_DEBUG_MQTT(bool v)
{
    if (v) DEBUG_MQTT = &INFO_multiprint;
    else DEBUG_MQTT = &DEBUG_buffer;
}
