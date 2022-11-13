#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "settings.h"

// boolean is deprecated. This does not suppress all warnings after
// pio run -t clean, but at least some of the ones that occur during normal
// pio run.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <PubSubClient.h>
#pragma GCC diagnostic pop


#if true
#define MQTTpublish(x, y) MQTTClient.publish(x, y)
#else
#define MQTTpublish(x, y)
#endif

extern PubSubClient MQTTClient;

extern bool eth_skip;
extern bool MQTT_skip;
extern bool DHCP_mode;

uint8_t MQTT_init();
void MQTT_loop();
