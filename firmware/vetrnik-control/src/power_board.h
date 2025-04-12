#pragma once
#include <wt_hal.h>


typedef enum {
    PCOMMAND_RESET = 170,
    PCOMMAND_WDT_TEST = 171,
} power_board_command_t;



extern power_board_status_t power_board_status;


power_board_status_t power_board_status_read();


void power_board_set_duty(uint8_t duty);
void power_board_set_mode(power_board_mode_t mode);
bool power_board_set_mode(const char * str);
void power_board_set_software_enable(bool value);
void power_board_clear_errors();
void power_board_command(power_board_command_t command);
bool power_board_REL_write(uint8_t pin, bool value);
uint8_t power_board_REL_read(uint8_t pin);
