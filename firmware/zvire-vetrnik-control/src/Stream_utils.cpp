#include "Stream_utils.h"
#include <IPAddress.h>

void stream_print_MAC(Stream *response, uint8_t MAC[6])
{
    response->printf(
        "%02x:%02x:%02x:%02x:%02x:%02x",
        MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]
    );
}


void stream_print_settings(Stream *response, settings_t s)
{
    response->print("ETH_MAC: ");
    stream_print_MAC(response, s.ETH_MAC);
    response->println();

    response->print("ETH_IP: ");
    response->println(IPAddress(s.ETH_IP));

    response->print("MQTTserver: ");
    response->println(IPAddress(s.MQTTserver));

    response->printf("MQTTuser: \"%s\"\r\n", s.MQTTuser);

    response->printf("MQTTpassword: \"%s\"\r\n", s.MQTTpassword);

}
