/*!
    @file
*/

#include "errm.h"
#include <Arduino.h>  // millis()



uint8_t errm_count = 0;
void (*errm_create_callback)(const errm_error *err) = NULL;



//! Create an error from a template
errm_error errm_create(const errm_template *errortemplate, uint8_t code)
{
    errm_error err;
    err.errortemplate = errortemplate;
    err.when = millis();
    err.code = code;
    return err;
}


//! Add an error to the array
void errm_add(errm_error err)
{
    if (errm_count < ERRM_COUNT_MAX) errm_count++;
    // shift the array
    for (uint8_t i = 0; i < (errm_count - 1); i++)
    {
        // at least one of the most important errors needs to stay in the array
        if (i < errm_count - 2 || errm_errors[i+1].errortemplate->weight < 10)
            errm_errors[i + 1] = errm_errors[i];
    }
    errm_errors[0] = err;

    if (errm_create_callback) errm_create_callback(&err);
}


//! Clear all errors
void errm_clear(void)
{
    errm_count = 0;  // all saved errors are ignored
}
