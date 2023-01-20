#pragma once

#include <stdbool.h>
#include <Arduino.h>


#define PSerial Serial1  // uart_power Serial

extern Stream * print_RX;
extern char power_text_message[];
extern bool power_text_message_complete;

void uart_power_init();

bool uart_power_loop();
