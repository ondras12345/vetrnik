#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/// maximum supported number of LCD columns
#define LCD_COLS_MAX 20

/**
 * LCD HAL.
 *
 * Any implementation must implement all functions, there must
 * not be any NULL pointers.
 */
typedef struct
{
    /// Write buffer to the specified column and clear it.
    /// \return true on success
    bool (*commit)(uint8_t row);
    /// Concatenate a string to the buffer.
    void (*print)(const char * str);
    /// Set display cursor position.
    /// \return true on success, false if out of range
    bool (*set_cursor)(uint8_t col);
    void (*backlight_set)(bool s);
    bool (*backlight_get)();
    /// Get number of columns
    uint8_t (*get_cols)();
    /// Get number of rows
    uint8_t (*get_rows)();
} lcd_hal_t;


#ifdef __cplusplus
}
#endif
