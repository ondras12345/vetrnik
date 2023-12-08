#include "CLIeditor.h"

constexpr char CLIeditor::end_sequence[];


bool CLIeditor::backspace()
{
    if (index > 0)
    {
        index--;
        return true;
    }

    // Index is 0. Can we backspace past section boundary?
    if (section == last_write)
    {
        index = WRITE_BUF_SIZE-1;
        section = (section == 1) ? 0 : 1;
        return true;
    }

    return false;
}


/**
 * Process a single character of user input.
 *
 * @return true if done, else false
 */
bool CLIeditor::process(char c)
{
    if (write_file_ == nullptr || write_response_ == nullptr) return true;

    switch (c)
    {
        // backspace
        case '\b':
        case 0x7F:
            if (backspace()) write_response_('\b');
            return false;

        // Ignore '\0'. It seems to come from TelnetStream sometimes.
        case '\0':
            return false;

        // translate carriage returns to newlines
        case '\r':
            c = '\n';
            write_response_('\r');
            break;
    }

    write_response_(c);

    if (c == end_sequence[end_sequence_i])
    {
        end_sequence_i++;
    }
    else
    {
        // '.' should work after any number of '\r' | '\n'
        // This will only work properly for our simple end_sequence.
        end_sequence_i = 0;
        if (c == end_sequence[0])
        {
            end_sequence_i = 1;
        }
    }

    if (end_sequence_i >= sizeof(end_sequence)/sizeof(end_sequence[0]))
    {
        // End sequence detected.

        // We have incremented after the final character
        end_sequence_i--;

        // The final character wasn't written yet -- off-by-one intentional
        for (; end_sequence_i > 0; end_sequence_i--)
        {
            // we are dropping the character at end_sequence[end_sequence_i-1]
            backspace();
        }

        // Write everything
        if (section == last_write)
        {
            write_file_(buf+((section == 1) ? 0 : 1)*WRITE_BUF_SIZE, WRITE_BUF_SIZE);
            last_write = section;
        }
        if (index != 0) write_file_(buf+section*WRITE_BUF_SIZE, index);
        return true;  // done, the editor should no longer be used
    }

    buf[section*WRITE_BUF_SIZE + index] = c;
    index++;

    if (index >= WRITE_BUF_SIZE)
    {
        // switch to the other section
        section = (section == 1) ? 0 : 1;
        // start overwriting
        index = 0;
        // if the new section contains data
        if (section != last_write)
        {
            write_file_(buf+section*WRITE_BUF_SIZE, WRITE_BUF_SIZE);
            last_write = section;
        }
        return false;
    }
    return false;
}
