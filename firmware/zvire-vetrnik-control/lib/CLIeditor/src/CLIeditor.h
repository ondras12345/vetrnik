#pragma once
#include <stdint.h>
#include <stddef.h>


class CLIeditor {
    public:
        /** Create a useless object that always returns true from process() */
        // cppcheck-suppress uninitMemberVar symbolName=CLIeditor::buf
        CLIeditor() : write_response_(nullptr), write_file_(nullptr) { };

        // cppcheck-suppress uninitMemberVar symbolName=CLIeditor::buf
        CLIeditor(void (*write_response)(char c), void(*write_file)(void *buf, size_t len))
            : write_response_(write_response)
            , write_file_(write_file)
            {};

        bool process(char c);

    protected:
        bool backspace();

        void(*write_file_)(void *buf, size_t len) {};
        void (*write_response_)(char c);
        static constexpr size_t WRITE_BUF_SIZE = 256;
        char buf[2*WRITE_BUF_SIZE];
        size_t index = 0;  ///< index into the current section of the buffer
        uint8_t section = 0;  ///< section of the buffer -- 0 or 1
        uint8_t last_write = 1;  ///< section that was last written
        static constexpr char end_sequence[] = {'\n', '.', '\n'};
        uint8_t end_sequence_i = 0;  ///< index into end_sequence
};
