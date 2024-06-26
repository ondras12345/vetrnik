#include "flash_tools.h"

/**
 * Fill a section of a flash file with constant byte.
 * @param v value to fill the file with
 * @param l number of bytes to be filled
 * @return number of bytes written (can be lower if end of file is reached)
 */
uint32_t flash_fill_file(SerialFlashFile * f, uint8_t v, uint32_t l)
{
    uint32_t written = 0;

    uint8_t buf[256];
    memset(buf, v, sizeof buf);

    while (l)
    {
        uint32_t wrlen = l;
        if (wrlen > sizeof buf) wrlen = sizeof buf;
        l -= wrlen;
        written += f->write(buf, wrlen);
    }

    return written;
}
