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
#include "stats.h"
#include "pump.h"
#include "sensor_DS18B20.h"
#include <MQTT_helpers.h>

static EthernetClient ethClient;
static void MQTTcallback(char* topic, byte* payload, unsigned int length);
PubSubClient MQTTClient(ethClient);

bool eth_skip = false;
bool MQTT_skip = true;
bool DHCP_mode = true;

/**
 * millis() time when last valid MQTT command for power_board or control
 * was received
 */
unsigned long MQTT_last_command_ms = 0;


void ETH_reset()
{
    digitalWrite(PIN_ETH_RST, LOW);
    delayMicroseconds(1000);
    digitalWrite(PIN_ETH_RST, HIGH);
}


uint8_t MQTT_reinit()
{
    ETH_reset();
    return MQTT_init();
}


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
    // TODO eth_skip if ifconfig reports HW: 0, status: 0

    for (uint8_t i = 0; i < sizeof settings.MQTTserver; i++)
        if (settings.MQTTserver[i] != 0) MQTT_skip = false;

    MQTTClient.setCallback(MQTTcallback);
    MQTTClient.setServer(settings.MQTTserver, MQTTport);

    return result;
}


void MQTT_loop()
{
    if (eth_skip) return;

    unsigned long now = millis();
    if (Ethernet.localIP() == IPAddress(0, 0, 0, 0) &&
        Ethernet.subnetMask() == IPAddress(0, 0, 0, 0)
        )
    {
        static unsigned long prev_reinit = 0;
        if (now - prev_reinit < 60*1000UL) return;  // rate limit
        INFO->println("ETH stuck, rst");
        MQTT_reinit();
        prev_reinit = now;
    }

    // ENC28J60: this seems to do more than just renew the DHCP lease
    // W5500: only DHCP
    if (DHCP_mode) Ethernet.maintain();

    if (MQTT_skip) return;

    static byte MQTTReconnectCount = 0;
    static unsigned long MQTTLastReconnect = 0;

    if (MQTTReconnectCount > 6)
    {
        if ((unsigned long)(millis() - MQTTLastReconnect) < MQTTReconnectRate*3)
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
                MQTTClient.subscribe(MQTTtopic_cmnd_pump);

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


    static power_board_status_t prev_power_board_status = {0};

#define PB_c(name, topic, maketmp, cond)                                    \
    if ((cond) || force_report)                                             \
    {                                                                       \
        prev_power_board_status.name = power_board_status.name;             \
        maketmp                                                             \
        MQTTClient.publish(MQTTtopic_tele_power_board topic, tmp, true);    \
    }

/// Report a uint16_t power board datapoint, COND_NEQ
#define PB_uint16(name, topic)                                              \
    PB_c(name, topic, MAKETMP_UINT(power_board_status.name),                \
         COND_NEQ(power_board_status.name)                                  \
         )

/// Report a uint16_t power board datapoint, COND_HYST
#define PB_uint16_h(name, topic, hysteresis)                                \
    PB_c(name, topic, MAKETMP_UINT(power_board_status.name),                \
         COND_HYST(power_board_status.name, hysteresis))

/// Report a bool power board datapoint
#define PB_bool(name, topic)                                                \
    PB_c(name, topic, MAKETMP_BOOL(power_board_status.name),                \
         COND_NEQ(power_board_status.name)                                  \
         )
#define PB_decimal(name, topic, dp) \
    PB_c(name, topic, MAKETMP_DECIMAL(power_board_status.name, dp),         \
         COND_NEQ(power_board_status.name)                                  \
         )
#define PB_decimal_h(name, topic, dp, hysteresis)                           \
    PB_c(name, topic, MAKETMP_DECIMAL(power_board_status.name, dp),         \
         COND_HYST(power_board_status.name, hysteresis))

    if (COND_NEQ(power_board_status.voltage)
        || COND_NEQ(power_board_status.current)
        || force_report)
    {
        // W * 10
        uint16_t power =
            ((uint32_t)(power_board_status.voltage) *
             power_board_status.current
            ) / 1000;
        MAKETMP_DECIMAL(power, 1)
        MQTTClient.publish(MQTTtopic_tele_power_board "power", tmp, true);
    }

    PB_bool(valid, "valid")
    PB_uint16(time, "time")
    PB_uint16(mode, "mode")
    PB_uint16(duty, "duty")
    PB_uint16(OCP_max_duty, "OCP_max_duty")
    PB_uint16_h(RPM, "RPM", 2)
    PB_decimal(voltage, "voltage", 1)
    PB_decimal(current, "current", 3)
    PB_bool(enabled.overall, "enabled")
    PB_bool(enabled.hardware, "enabled/hardware")
    PB_bool(enabled.software, "enabled/software")
    PB_bool(emergency, "emergency")
    PB_decimal_h(temperature_heatsink, "temperature/heatsink", 1, 2)
    PB_decimal_h(temperature_rectifier, "temperature/rectifier", 1, 2)
    PB_uint16(fan, "fan");
    PB_uint16(error_count, "error_count");
    PB_bool(last5m, "last5m");

#undef PB_c
#undef PB_uint16
#undef PB_uint16_h
#undef PB_bool
#undef PB_decimal
#undef PB_decimal_h

    // TODO publish supported power_board modes ??

    static control_strategy_t prev_control_strategy = control_shorted;
    control_strategy_t control_strategy = control_get_strategy();
    if (control_strategy != prev_control_strategy || force_report)
    {
        prev_control_strategy = control_strategy;
        MQTTClient.publish(MQTTtopic_tele_control "strategy",
                           control_strategies[control_strategy], true);
    }

    static stats_t prev_stats = {0};
    if (stats.energy != prev_stats.energy || force_report)
    {
        prev_stats.energy = stats.energy;
        MAKETMP_DECIMAL(stats.energy, 3);
        MQTTClient.publish(MQTTtopic_tele_stats "energy", tmp, true);
    }

    static uint16_t prev_DS18B20_readings[SENSOR_DS18B20_COUNT] = { 0 };
    for (uint_fast8_t i = 0; i < SENSOR_DS18B20_COUNT; i++)
    {
        if (!sensor_DS18B20_enabled(i)) continue;
        uint16_t reading = sensor_DS18B20_readings[i];
        if (reading == prev_DS18B20_readings[i] && !force_report) continue;
        prev_DS18B20_readings[i] = reading;

        char topic[sizeof(MQTTtopic_tele_temperature)+sizeof(settings.DS18B20s[0].name)];
        snprintf(topic, sizeof topic,
                 "%s%s",
                 MQTTtopic_tele_temperature, settings.DS18B20s[i].name);

        MAKETMP_DECIMAL(reading, 2);
        MQTTClient.publish(topic, tmp, true);
    }

    static bool prev_pump = false;
    bool pump = pump_get();
    if (pump != prev_pump || force_report)
    {
        prev_pump = pump;
        MQTTClient.publish(MQTTtopic_tele_pump, (pump ? "1" : "0"), true);
    }

}


void MQTTcallback(char* topic, byte* payload, unsigned int length)
{
    DEBUG_MQTT->printf("MQTT R l:%u t:%s\r\n", length, topic);
    if (DEBUG_MQTT != &DEBUG_buffer)
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

        MQTT_last_command_ms = millis();
        return;
    }

    if (strstr(topic, MQTTtopic_cmnd_power_board) != NULL ||
        strcmp(topic, MQTTtopic_cmnd_control "strategy") == 0)
    {
        MQTT_last_command_ms = millis();
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

    if (strcmp(topic, MQTTtopic_cmnd_power_board "sw_enable") == 0)
    {
        power_board_set_software_enable(payload[0] == '1');
        return;
    }

    if (strcmp(topic, MQTTtopic_cmnd_power_board "command") == 0)
    {
        if (strncmp((const char *)payload, "clear_errors", length) == 0)
            power_board_clear_errors();
        else if (strncmp((const char *)payload, "reset", length) == 0)
            power_board_command(PCOMMAND_RESET);
        else if (strncmp((const char *)payload, "WDT_test", length) == 0)
            power_board_command(PCOMMAND_WDT_TEST);
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

    if (strcmp(topic, MQTTtopic_cmnd_pump) == 0)
    {
        pump_set(payload[0] == '1');
        return;
    }
}
