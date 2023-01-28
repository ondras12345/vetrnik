#include "mqtt.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include "power_datapoints.h"
#include "uart_power.h"
#include "debug.h"
#include "power_board.h"
#include "lisp.h"
#include "control.h"

static EthernetClient ethClient;
static void MQTTcallback(char* topic, byte* payload, unsigned int length);
PubSubClient MQTTClient(ethClient);

bool eth_skip = false;
bool MQTT_skip = true;
bool DHCP_mode = true;


uint8_t MQTT_init()
{
    //Ethernet.init(pin_ETH_SS);

    INFO->print(F("Eth begin: "));
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

    INFO->println(result);
    INFO->print(F("Eth IP: "));
    INFO->println(Ethernet.localIP());

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
            INFO->println("Connecting MQTT...");
            if (MQTTClient.connect(MQTTclientID, settings.MQTTuser, settings.MQTTpassword,
                        MQTTtopic_availability, 2, true, "offline")
               )
            {
                MQTTClient.subscribe(MQTTtopic_cmnd_raw "+");
                MQTTClient.subscribe(MQTTtopic_cmnd_power_board "+");
                MQTTClient.subscribe(MQTTtopic_cmnd_lisp);
                MQTTClient.subscribe(MQTTtopic_cmnd_control "+");

                MQTTClient.publish(MQTTtopic_availability, "online", true);

                force_report = true;

                INFO->println("MQTT connected");
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
        char top[sizeof MQTTtopic_tele_raw_RX + 1] = MQTTtopic_tele_raw_RX;
        top[sizeof MQTTtopic_tele_raw_RX - 1] = c;
        top[sizeof MQTTtopic_tele_raw_RX] = '\0';
        if (settings.report_raw)
        {
            RX_datapoint_t dp = RX_datapoints_get(c);
            if (dp.changed || force_report)
            {
                char tmp[3*sizeof(RX_datapoint_t) + 1];  // >= number of digits required + null
                snprintf(tmp, sizeof tmp, "%lu", dp.value);
                MQTTClient.publish(top, tmp, true);
                dp.changed = false;
                RX_datapoints_set(c, dp);
            }
        }
        else
        {
            if (force_report)
            {
                // clean up retained messages
                MQTTClient.publish(top, "", true);
            }
        }
    }

    if (power_text_message_complete)
    {
        INFO->print("Raw text message: ");
        INFO->println(power_text_message);
        MQTTClient.publish(MQTTtopic_tele_raw_errors, power_text_message);
        power_text_message_complete = false;
    }


    static power_board_status_t prev_pb_status = {0};

#define PB(name, topic, maketmp) \
    if (power_board_status.name != prev_pb_status.name || force_report) \
    { \
        prev_pb_status.name = power_board_status.name; \
        maketmp \
        MQTTClient.publish(MQTTtopic_tele_power_board topic, tmp, true); \
    }
#define maketmp_uint16(v) \
    char tmp[3*sizeof(v) + 1]; \
    snprintf(tmp, sizeof tmp, "%u", v);
#define PB_uint16(name, topic) \
    PB(name, topic, maketmp_uint16(power_board_status.name))
#define PB_bool(name, topic) \
    PB(name, topic, char tmp[2]; tmp[1] = '\0'; tmp[0] = (power_board_status.name ? '1' : '0');)
#define maketmp_decimal(v, dp) \
    char tmp[3*sizeof(v) + 1 + 1]; /* int + decimal point + null*/ \
    /* Type cast of the whole value because of cppcheck. I don't think it is really needed. */ \
    snprintf(tmp, sizeof tmp, "%u.%0" #dp "u", uint16_t(v / uint16_t(1E ## dp)), uint16_t(v % uint16_t(1E ## dp)));
#define PB_decimal(name, topic, dp) \
    PB(name, topic, maketmp_decimal(power_board_status.name, dp))

    PB_bool(valid, "valid")
    PB_uint16(time, "time")
    PB_uint16(mode, "mode")
    PB_uint16(duty, "duty")
    PB_uint16(RPM, "RPM")
    PB_decimal(voltage, "voltage", 1)
    PB_decimal(current, "current", 3)
    PB_bool(enabled, "enabled")
    PB_decimal(temperature_heatsink, "temperature_heatsink", 1)
    PB_uint16(fan, "fan");
    PB_uint16(error_count, "error_count");

#undef PB
#undef maketmp_uint16
#undef PB_uint16
#undef PB_bool
#undef maketmp_decimal
#undef PB_decimal

    // TODO publish supported power_board modes ??

    static control_strategy_t prev_control_strategy = control_shorted;
    control_strategy_t control_strategy = control_get_strategy();
    if (control_strategy != prev_control_strategy || force_report)
    {
        prev_control_strategy = control_strategy;
        MQTTClient.publish(MQTTtopic_tele_control "strategy",
                           control_strategies[control_strategy], true);
    }
}


void MQTTcallback(char* topic, byte* payload, unsigned int length)
{
    DEBUG_MQTT->printf("MQTT R l:%u t:%s\r\n", length, topic);
    if (DEBUG_MQTT != DEBUG_noprint)
    {
        DEBUG_MQTT->print(" p:");
        for (unsigned int i = 0; i < length; i++)
        {
            DEBUG_MQTT->print(payload[i], HEX);
            DEBUG_MQTT->print(' ');
        }
        DEBUG_MQTT->println();
    }

    if (length == 0) return;

    if (strstr(topic, MQTTtopic_cmnd_raw) != NULL)
    {
        // 1 character longer
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

    if (strcmp(topic, MQTTtopic_cmnd_power_board "duty") == 0)
    {
        if (length > 3) return;
        char buff[4];
        memcpy(buff, payload, length);
        buff[length] = '\0';
        unsigned int duty;
        sscanf(buff, "%u", &duty);
        if (duty > 255) return;
        power_board_set_duty(duty);
        return;
    }

    if (strcmp(topic, MQTTtopic_cmnd_power_board "clear_errors") == 0)
    {
        if (payload[0] != '1') return;
        power_board_clear_errors();
        return;
    }

    if (strcmp(topic, MQTTtopic_cmnd_power_board "mode") == 0)
    {
        for (size_t i = 0; power_board_modes[i] != nullptr; i++)
        {
            if (length == strlen(power_board_modes[i]) &&
                strncmp((const char *)payload, power_board_modes[i], length) == 0)
            {
                power_board_mode_t mode = (power_board_mode_t)i;
                power_board_set_mode(mode);
                return;
            }
        }
        return;
    }

    if (strcmp(topic, MQTTtopic_cmnd_lisp) == 0)
    {
        lisp_run_blind((char*)payload, length);
        return;
    }

    if (strcmp(topic, MQTTtopic_cmnd_control "strategy") == 0)
    {
        for (size_t i = 0; control_strategies[i] != nullptr; i++)
        {
            if (length == strlen(control_strategies[i]) &&
                strncmp((const char *)payload, control_strategies[i], length) == 0)
            {
                control_strategy_t strategy = (control_strategy_t)i;
                control_set_strategy(strategy);
                return;
            }
        }
        return;
    }
}
