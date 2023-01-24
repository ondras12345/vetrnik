/*!
    @file
    @brief  A simple error manager
    You should do something like
    ```
    const byte ERRM_COUNT_MAX=5;
    errm_error errm_errors[ERRM_COUNT_MAX];
    ```
    after you include this file.
*/

#pragma once

#include <stdint.h>


//! An error has a weight and text from it's template + it's own code and time
typedef struct
{
    /*!
     * At least one of errors with weight >= 10 is guaranteed to stay in the
     * array until manually cleared.
     */
    uint8_t weight;
    const char *text;
} errm_template;


//! An error
typedef struct
{
    //! Pointer to the template. @see errm_template
    const errm_template *errortemplate;
    //! `millis()` taken when the error occurred
    unsigned long when;
    //! code of the error
    uint8_t code;
} errm_error;


extern const uint8_t ERRM_COUNT_MAX;
extern errm_error errm_errors[];
extern uint8_t errm_count;

extern void (*errm_create_callback)(const errm_error *err);

errm_error errm_create(const errm_template *errortemplate, uint8_t code = 255);

void errm_add(errm_error err);

void errm_clear(void);
