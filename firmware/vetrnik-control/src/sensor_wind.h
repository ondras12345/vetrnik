#pragma once

#define SENSOR_WIND_READ_TIMEOUT 350UL

/**
 * Delay before sending data after DE is asserted,
 * and before releasing DE after data is sent.
 * This needs to be long enough to ensure the last byte is sent, as the timer
 * is started as soon as it enters the transmit buffer.
 */
#define SENSOR_WIND_DE_WAIT 5UL


void sensor_wind_init();
void sensor_wind_loop();
float sensor_wind_read();
