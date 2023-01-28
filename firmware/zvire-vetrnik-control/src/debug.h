#pragma once
#include <Print.h>
#include <stdint.h>


class BufferPrint: public Print {
    public:
        virtual size_t write(uint8_t);
        virtual size_t write(const uint8_t *data, size_t size);

        void print_out(Print * response);

    protected:
        static constexpr size_t capacity = 512;
        char buf[capacity];
        size_t read_addr = 0;
        size_t write_addr = 0;
};


extern BufferPrint DEBUG_buffer;

void log_add_INFO_backend(Print * printer);

void log_set_DEBUG_PB(bool v);
void log_set_DEBUG_MQTT(bool v);

// All Print pointers in this file are guaranteed not to be nullptr.

/// Error messages that should always be printed.
/// Also used if user hasn't had enough time to enable debug in CLI yet.
extern Print * INFO;
/// Power board debug log
extern Print * DEBUG_PB;
/// MQTT debug log
extern Print * DEBUG_MQTT;
