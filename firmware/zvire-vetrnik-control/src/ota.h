#pragma once

/*
 * Over-the-air firmware upgrade.
 * Firmware can be uploaded like this:
 * curl -v --request POST --data-binary @.pio/build/blackpill_f401cc/firmware.bin http://arduino:password@IP:65280/sketch
 *
 * This uses flash InternalStorage, so only firmware images smaller than 1/2 of
 * flash size can be uploaded OTA.
 *
 * TODO OTA with SerialFlash
 * - Would need to write custom ramfunc to copy SerialFlash data to internal
 *   flash.
 * - Not sure if there is a simple way to fit SerialFlash lib in RAM.
 * - Maybe we could find the start of the file using the lib while still
 *   running from flash and then use manual HAL SPI in our ramfunc.
 *   - Actually, HAL won't be available in ramfunc. We would have to set up the
 *     registers manually.
 */

void ota_init();
void ota_loop();
