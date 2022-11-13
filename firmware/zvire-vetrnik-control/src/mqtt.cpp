#include "mqtt.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include "power_datapoints.h"
#include "uart_power.h"
#include "debug.h"

static EthernetClient ethClient;
static void MQTTcallback(char* topic, byte* payload, unsigned int length);
PubSubClient MQTTClient(ethClient);

bool eth_skip = false;
bool MQTT_skip = true;
bool DHCP_mode = true;


uint8_t MQTT_init()
{
    //Ethernet.init(pin_ETH_SS);

    DEBUG.print(F("Eth begin: "));
    // 0.0.0.0 means "use DHCP"
    for (uint8_t i = 0; i < sizeof settings.ETH_IP; i++)
        if (settings.ETH_IP[i] != 0) DHCP_mode = false;
    uint8_t result = 1;
    if (DHCP_mode)
        result = Ethernet.begin(settings.ETH_MAC);
    else
        Ethernet.begin(settings.ETH_MAC, settings.ETH_IP);
    // Default timeout is 15 seconds.
    MQTTClient.setSocketTimeout(7);

    DEBUG.println(result);
    DEBUG.print(F("Eth IP: "));
    DEBUG.println(Ethernet.localIP());

    if (result == 0) eth_skip = true;

    for (uint8_t i = 0; i < sizeof settings.MQTTserver; i++)
        if (settings.MQTTserver[i] != 0) MQTT_skip = false;

    MQTTClient.setCallback(MQTTcallback);
    MQTTClient.setServer(settings.MQTTserver, MQTTport);

    return result;
}


void MQTT_loop()
{
    if (eth_skip) return;

    // ENC28J60: this seems to do more than just renew the DHCP lease
    // W5500: it should be OK to only call this every 10 minutes TODO
    if (DHCP_mode) Ethernet.maintain();

    if (MQTT_skip) return;

    static byte MQTTReconnectCount = 0;
    static unsigned long MQTTLastReconnect = 0;

    if (MQTTReconnectCount > 6)
    {
        if ((unsigned long)(millis() - MQTTLastReconnect) < MQTTReconnectRate*10UL)
            return;
    }

    // force reporting of values after successful MQTT connection
    bool force_report = false;

    if (!MQTTClient.connected())
    {
        if((unsigned long)(millis() - MQTTLastReconnect) >= MQTTReconnectRate)
        {
            DEBUG.println("Connecting MQTT...");
            if (MQTTClient.connect(MQTTclientID, settings.MQTTuser, settings.MQTTpassword,
                        MQTTtopic_availability, 2, true, "offline")
               )
            {
                MQTTClient.subscribe(MQTTtopic_cmnd_raw "+");

                MQTTClient.publish(MQTTtopic_availability, "online", true);

                force_report = true;

                DEBUG.println("MQTT connected");
            }

            MQTTLastReconnect = millis();
            MQTTReconnectCount++;
        }
    }
    if (!MQTTClient.connected()) return;

    // if everything seems good, switch back to normal rate of reconnecting
    if((unsigned long)(millis() - MQTTLastReconnect) >= MQTTReconnectRate*5UL)
        MQTTReconnectCount = 0;


    MQTTClient.loop();

    // report values here, use force_report to detect reconnection

    // Starting with c = '!' is not OK because MQTT does not seem to like #, $
    // and + in topic names
    for (char c = ','; c <= '~'; c++)
    {
        RX_datapoint_t dp = RX_datapoints_get(c);
        if (dp.changed || force_report)
        {
            char tmp[3*sizeof(RX_datapoint_t) + 1];  // >= number of digits required + null
            snprintf(tmp, sizeof tmp, "%lu", dp.value);
            char top[sizeof MQTTtopic_tele_raw_RX + 1] = MQTTtopic_tele_raw_RX;
            top[sizeof MQTTtopic_tele_raw_RX - 1] = c;
            top[sizeof MQTTtopic_tele_raw_RX] = '\0';
            MQTTClient.publish(top, tmp, true);
            dp.changed = false;
            RX_datapoints_set(c, dp);
        }
    }

    if (power_text_message_complete)
    {
        INFO.print("Raw text message: ");
        INFO.println(power_text_message);
        MQTTClient.publish(MQTTtopic_tele_raw_errors, power_text_message);
        power_text_message_complete = false;
    }
}


void MQTTcallback(char* topic, byte* payload, unsigned int length)
{
    DEBUG.printf("MQTTrecv l: %u t:%s", length, topic);
    DEBUG.print(" p:");
    for (unsigned int i = 0; i < length; i++)
    {
        DEBUG.print(payload[i], HEX);
        DEBUG.print(' ');
    }
    DEBUG.println();
    if (length == 0) return;

    if (strstr(topic, MQTTtopic_cmnd_raw) != NULL)
    {
        if (strlen(topic) != sizeof MQTTtopic_cmnd_raw)
            return;

        char name = topic[sizeof(MQTTtopic_cmnd_raw) - 1];
        uint8_t value;
        char buff[3+1];
        if (length >= sizeof buff)
            return;
        memcpy(buff, payload, length);
        buff[length] = '\0';
        unsigned int value_ss = 0;
        sscanf(buff, "%u", &value_ss);
        value = (uint8_t) value_ss;

        TX_datapoints_set(name, value);

        return;
    }
}
