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


/**
 * Find first / last byte equal to v using binary search
 *
 * @param last toggle between returning address of first match and last match
 * @return address of first/last match. -1 if no match is found.
 */
uint32_t flash_find_byte(SerialFlashFile * f, const uint8_t v, const bool last)
{
    uint32_t left = 0;
    uint32_t right = f->size()-1;
    uint32_t first_match = -1;
    uint32_t last_match = -1;
    while (left <= right)
    {
        uint32_t mid = (left + right) / 2;
        f->seek(mid);
        char c;
        f->read(&c, 1);
        if (c == v)
        {
            if (mid > last_match || last_match == (uint32_t)-1) last_match = mid;
            if (mid < first_match) first_match = mid;
        }

        if ((c == v) == last)
            left = mid+1;
        else
        {
            if (mid == 0) break;  // prevent infinite loop caused by right=-1 overflow
            right = mid-1;
        }
    }
    return last ? last_match : first_match;
}
