#include "mqtt.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include "power_datapoints.h"
#include "uart_power.h"
#include "debug.h"
#include "lisp.h"
#include "sensor_DS18B20.h"
#include "cli.h"
#include "log.h"
#include "hal.h"
#include <MQTT_helpers.h>
#ifdef WATCHDOG_TIME
#include <IWatchdog.h>
#endif

static EthernetClient ethClient;
static void MQTTcallback(char* topic, byte* payload, unsigned int length);
PubSubClient MQTTClient(ethClient);

bool eth_skip = false;
bool MQTT_skip = true;
bool DHCP_mode = true;
uint_fast8_t MQTT_reconnect_count = 0;
unsigned long MQTT_last_init_ms = 0;

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
    MQTT_reconnect_count = 0;
#ifdef WATCHDOG_TIME
    IWatchdog.reload();
#endif
    ETH_reset();
    return MQTT_init();
}


uint8_t MQTT_init()
{
    //Ethernet.init(pin_ETH_SS);

    log_add_event(kEthernetBegin);
    INFO->print(F("Eth begin: "));
    // 0.0.0.0 means "use DHCP"
    for (uint8_t i = 0; i < sizeof settings.ETH_IP; i++)
        if (settings.ETH_IP[i] != 0) DHCP_mode = false;
    uint8_t result = 1;
    if (DHCP_mode)
        result = Ethernet.begin(settings.ETH_MAC, settings.DHCP_timeout_s * 1000UL);
    else
        Ethernet.begin(settings.ETH_MAC, settings.ETH_IP);
    MQTTClient.setSocketTimeout(settings.MQTT_timeout_s);

    INFO->println(result);
    log_add_event(kEthernetIP);
    INFO->print(F("Eth IP: "));
    INFO->println(Ethernet.localIP());

    eth_skip = (result == 0);
    // TODO eth_skip if ifconfig reports HW: 0, status: 0

    for (uint8_t i = 0; i < sizeof settings.MQTTserver; i++)
        if (settings.MQTTserver[i] != 0) MQTT_skip = false;

    MQTTClient.setCallback(MQTTcallback);
    MQTTClient.setServer(settings.MQTTserver, MQTTport);

    MQTT_last_init_ms = millis();
    return result;
}

static uint32_t log_skipped;
static uint32_t log_succeeded;
static uint_fast8_t log_id;

static void publish_log(const char * topic, const char * payload, bool retain=true)
{
    log_skipped &= ~(1<<log_id);
    bool success = MQTTClient.publish(topic, payload, retain);
    log_succeeded |= ((success ? 1 : 0)<<log_id);
}

static void publish_log_prefix(const char * topic_prefix, const char * topic_suffix, const char * payload, bool retain=true)
{
    char topic[80];
    snprintf(topic, sizeof topic, "%s%s", topic_prefix, topic_suffix);
    publish_log(topic, payload, retain);
}


void MQTT_loop()
{
    unsigned long now = millis();
    static unsigned long MQTT_last_full_loop = 0;

    if (eth_skip)
    {
        // retry
        if (now - MQTT_last_init_ms >= MQTTWaitBeforeEthernetReset)
        {
            log_add_event_and_println(kMqttReinitTime, INFO);
            MQTT_reinit();
            MQTT_last_full_loop = now;  // prevent second reinit
        }
        return;
    }

    if (Ethernet.localIP() == IPAddress(0, 0, 0, 0) &&
        Ethernet.subnetMask() == IPAddress(0, 0, 0, 0)
        )
    {
        static unsigned long prev_reinit = 0;
        if (now - prev_reinit < 60*1000UL) return;  // rate limit
        log_add_event_and_println(kEthernetStuck, INFO);
        MQTT_reinit();
        prev_reinit = now;
    }

    // ENC28J60: this seems to do more than just renew the DHCP lease
    // W5500: only DHCP
    if (DHCP_mode) Ethernet.maintain();

    if (MQTT_skip) return;

    static int prev_MQTT_state = -1000;
    int MQTT_state = MQTTClient.state();
    if (MQTT_state != prev_MQTT_state)
    {
        prev_MQTT_state = MQTT_state;
        log_add_record_mqtt_state(MQTT_state);
        INFO->print("MQTT state: ");
        INFO->println(MQTT_state_to_str(MQTT_state));
    }

    static unsigned long MQTTLastReconnect = 0;

    now = millis();
    if (MQTT_reconnect_count > 6)
    {
        if (now - MQTTLastReconnect < MQTTReconnectRate*3)
            return;
    }

    if (now - MQTT_last_full_loop >= MQTTWaitBeforeEthernetReset)
    {
        log_add_event_and_println(kMqttReinitTime, INFO);
        MQTT_last_full_loop = now;
        MQTT_reinit();
        return;  // do not waste any more time in this loop()
    }

    // force reporting of values after successful MQTT connection
    bool force_report = false;

    if (!MQTTClient.connected())
    {
        now = millis();
        if (now - MQTTLastReconnect >= MQTTReconnectRate &&
            now - MQTT_last_full_loop >= MQTTWaitBeforeReconnect)
        {
            INFO->println("Connecting MQTT...");
            if (MQTTClient.connect(MQTTclientID, settings.MQTTuser, settings.MQTTpassword,
                        MQTTtopic_availability, 2, true, "offline")
               )
            {
                const char * subscribe_topics[] = {
                    "raw/+",
                    "power_board/+",
                    "lisp",
                    "cli",
                    "control/+",
                    "pump",
                    "display/backlight",
                };
                for (uint_fast8_t i = 0; i < sizeof(subscribe_topics)/sizeof(subscribe_topics[0]); i++)
                {
                    char topic[64];
                    snprintf(topic, sizeof topic, "%s%s", MQTTtopic_cmnd, subscribe_topics[i]);
                    MQTTClient.subscribe(topic);
                }

                MQTTClient.publish(MQTTtopic_availability, "online", true);

                force_report = true;

                log_add_event_and_println(kMqttConnected, INFO);
            }

            MQTTLastReconnect = millis();
            MQTT_reconnect_count++;
            // don't overflow
            if (MQTT_reconnect_count == 0) MQTT_reconnect_count--;
        }
    }
    if (!MQTTClient.connected()) return;

    // if everything seems good, switch back to normal rate of reconnecting
    if((unsigned long)(millis() - MQTTLastReconnect) >= MQTTReconnectRate*5UL)
        MQTT_reconnect_count = 0;


    MQTTClient.loop();

    // report values here, use force_report to detect reconnection
    log_skipped = -1;
    log_succeeded = 0;

    log_id = 0;
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
                publish_log(top, tmp);
                dp.changed = false;
                RX_datapoints_set(c, dp);
            }
        }
        else
        {
            if (force_report)
            {
                // clean up retained messages
                publish_log(top, "");
            }
        }
    }

    if (power_text_message_complete)
    {
        // TODO only printed if MQTT connected
        INFO->print("Raw text message: ");
        INFO->println(power_text_message);
        log_id = 1;
        publish_log_prefix(MQTTtopic_tele, "raw/errors", power_text_message, false);
        power_text_message_complete = false;
    }


    power_board_status_t pb_stat = wt_hal.pwr_get_status();
    static power_board_status_t prev_pb_stat = {0};

#define PB_c(name, topic, maketmp, cond)                                    \
    if ((cond) || force_report)                                             \
    {                                                                       \
        prev_pb_stat.name = pb_stat.name;                                   \
        maketmp                                                             \
        publish_log_prefix(MQTTtopic_tele_power_board, topic, tmp);         \
    }

/// Report a uint16_t power board datapoint, COND_NEQ
#define PB_uint16(name, topic)                                              \
    PB_c(name, topic, MAKETMP_UINT(pb_stat.name),                           \
         COND_NEQ(pb_stat.name)                                             \
         )

/// Report a uint16_t power board datapoint, COND_HYST
#define PB_uint16_h(name, topic, hysteresis)                                \
    PB_c(name, topic, MAKETMP_UINT(pb_stat.name),                           \
         COND_HYST(pb_stat.name, hysteresis))

/// Report a bool power board datapoint
#define PB_bool(name, topic)                                                \
    PB_c(name, topic, MAKETMP_BOOL(pb_stat.name),                           \
         COND_NEQ(pb_stat.name)                                             \
         )
#define PB_decimal(name, topic, dp) \
    PB_c(name, topic, MAKETMP_DECIMAL(pb_stat.name, dp),                    \
         COND_NEQ(pb_stat.name)                                             \
         )
#define PB_decimal_h(name, topic, dp, hysteresis)                           \
    PB_c(name, topic, MAKETMP_DECIMAL(pb_stat.name, dp),                    \
         COND_HYST(pb_stat.name, hysteresis))

    if (COND_NEQ(pb_stat.voltage)
        || COND_NEQ(pb_stat.current)
        || force_report)
    {
        // W * 10
        uint16_t power =
            ((uint32_t)(pb_stat.voltage) *
             pb_stat.current
            ) / 1000;
        MAKETMP_DECIMAL(power, 1)
        log_id = 2;
        publish_log_prefix(MQTTtopic_tele_power_board, "power", tmp);
    }

    log_id = 3;
    PB_bool(valid, "valid")
    log_id = 4;
    PB_uint16(time, "time")
    log_id = 5;
    PB_uint16(mode, "mode")
    log_id = 6;
    PB_uint16(duty, "duty")
    log_id = 7;
    PB_uint16(OCP_max_duty, "OCP_max_duty")
    log_id = 8;
    PB_uint16_h(RPM, "RPM", 2)
    log_id = 9;
    PB_decimal(voltage, "voltage", 1)
    log_id = 10;
    PB_decimal(current, "current", 3)
    log_id = 11;
    PB_bool(enabled.overall, "enabled")
    log_id = 12;
    PB_bool(enabled.hardware, "enabled/hardware")
    log_id = 13;
    PB_bool(enabled.software, "enabled/software")
    log_id = 14;
    PB_bool(emergency, "emergency")
    log_id = 15;
    PB_decimal_h(temperature_heatsink, "temperature/heatsink", 1, 2)
    log_id = 16;
    PB_decimal_h(temperature_rectifier, "temperature/rectifier", 1, 2)
    log_id = 17;
    PB_uint16(fan, "fan");
    log_id = 18;
    PB_uint16(error_count, "error_count");
    log_id = 19;
    PB_bool(last5m, "last5m");

#undef PB_c
#undef PB_uint16
#undef PB_uint16_h
#undef PB_bool
#undef PB_decimal
#undef PB_decimal_h

    // TODO publish supported power_board modes ??

    static control_strategy_t prev_control_strategy = control_emergency;
    control_strategy_t control_strategy = wt_hal.ctrl_get_strategy();
    if (COND_NEQ(control_strategy) || force_report)
    {
        prev_control_strategy = control_strategy;
        log_id = 20;
        publish_log_prefix(MQTTtopic_tele_control, "strategy",
                           control_strategies[control_strategy]);
    }

    // minutes remaining before contactor switches off, 255 ==> contactor is
    // off
    static uint8_t prev_control_contactor_min = 0;
    unsigned long cs = wt_hal.ctrl_contactor_get();
    uint8_t control_contactor_min = 255;
    if (cs != (unsigned long)-1) control_contactor_min = cs / 60000UL;
    if (COND_NEQ(control_contactor_min) || force_report)
    {
        prev_control_contactor_min = control_contactor_min;
        MAKETMP_UINT(control_contactor_min);
        log_id = 21;
        publish_log_prefix(MQTTtopic_tele_control, "contactor", tmp);
    }

    static stats_t prev_stats = {0};
    stats_t stats = wt_hal.stats_get();
    if (COND_NEQ(stats.energy) || force_report)
    {
        prev_stats.energy = stats.energy;
        MAKETMP_DECIMAL(stats.energy, 3);
        log_id = 22;
        publish_log_prefix(MQTTtopic_tele_stats, "energy", tmp);
    }

    if (COND_NEQ(stats.energy_Ws10) || force_report)
    {
        prev_stats.energy_Ws10 = stats.energy_Ws10;
        MAKETMP_DECIMAL(stats.energy_Ws10, 1);
        log_id = 23;
        publish_log_prefix(MQTTtopic_tele_stats, "energy_Ws", tmp);
    }

    static uint16_t prev_DS18B20_readings[SENSOR_DS18B20_COUNT] = { 0 };
    for (uint_fast8_t i = 0; i < SENSOR_DS18B20_COUNT; i++)
    {
        if (!sensor_DS18B20_enabled(i)) continue;
        uint16_t reading = sensor_DS18B20_readings[i];

        const uint16_t prev_reading = prev_DS18B20_readings[i];
        // readings have to differ by 0.1 degrees or more
        if (!COND_HYST(reading, 9) && !force_report) continue;

        prev_DS18B20_readings[i] = reading;

        // reduce resolution (+5 for mathematical round instead of truncate)
        reading = (reading + 5) / 10;
        MAKETMP_DECIMAL(reading, 1);
        // Not ideal, will show up as succeeded if at least one call succeeded.
        log_id = 24;
        publish_log_prefix(MQTTtopic_tele_temperature, settings.DS18B20s[i].name, tmp);
    }

    static bool prev_pump = false;
    bool pump = wt_hal.pump_get();
    if (COND_NEQ(pump) || force_report)
    {
        prev_pump = pump;
        MAKETMP_BOOL(pump);
        log_id = 25;
        publish_log_prefix(MQTTtopic_tele, "pump", tmp);
    }


    static bool prev_backlight = false;
    bool backlight = lcd_hal.backlight_get();
    if (COND_NEQ(backlight) || force_report)
    {
        prev_backlight = backlight;
        MAKETMP_BOOL(backlight);
        log_id = 26;
        publish_log_prefix(MQTTtopic_tele, "display/backlight", tmp);
    }

    static float prev_vwind = 0.0;
    float vwind = wt_hal.vwind();
    if (((vwind != prev_vwind) && !(isnan(prev_vwind) && isnan(vwind)) ) || force_report)
    {
        prev_vwind = vwind;
        log_id = 27;
        char tmp[sizeof "123.4"];
        snprintf(tmp, sizeof tmp, "%.1f", vwind);
        publish_log_prefix(MQTTtopic_tele_control, "vwind", tmp);
    }

    // Only log if there was a publish that wasn't skipped and did not succeed.
    // cppcheck-suppress knownConditionTrueFalse
    if ((uint32_t)(~(log_skipped | log_succeeded)))
        log_add_record_mqtt_publish(log_skipped, log_succeeded);

    MQTT_last_full_loop = millis();
}


void MQTTcallback(char* topic, byte* payload, unsigned int length)
{
    log_add_record_mqtt_receive(length);
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

    // advance past topic_prefix
    const char * topic_prefix = MQTTtopic_cmnd;
    while (*topic != '\0' && *(topic++) == *(topic_prefix++));
    if (*topic_prefix != '\0') return;  // did not start with prefix

    if (strstr(topic, "raw/") != NULL)
    {
        // 1 character longer
        if (strlen(topic) != sizeof "raw/")
            return;

        char name = topic[sizeof("raw/") - 1];
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

    if (strstr(topic, "power_board/") != NULL ||
        strcmp(topic, "control/strategy") == 0)
    {
        MQTT_last_command_ms = millis();
    }

    if (strcmp(topic, "power_board/duty") == 0)
    {
        if (length > 3) return;
        char buff[4];
        memcpy(buff, payload, length);
        buff[length] = '\0';
        unsigned int duty;
        sscanf(buff, "%u", &duty);
        if (duty > 255) return;
        wt_hal.pwr_set_duty(duty);
        return;
    }

    if (strcmp(topic, "power_board/sw_enable") == 0)
    {
        wt_hal.pwr_set_sw_enable(payload[0] == '1');
        return;
    }

    if (strcmp(topic, "power_board/command") == 0)
    {
        if (strncmp((const char *)payload, "clear_errors", length) == 0)
            wt_hal.pwr_clear_errors();
        else if (strncmp((const char *)payload, "reset", length) == 0)
            wt_hal.pwr_reset();
        else if (strncmp((const char *)payload, "WDT_test", length) == 0)
            wt_hal.pwr_test_WDT();
        return;
    }

    if (strcmp(topic, "power_board/mode") == 0)
    {
        char buf[32];
        if (length >= sizeof buf) return;
        memcpy(buf, payload, length);
        buf[length] = '\0';
        wt_hal.pwr_set_mode_str(buf);
        return;
    }

    if (strcmp(topic, "lisp") == 0)
    {
        lisp_run_blind((char*)payload, length);
        return;
    }

    if (strcmp(topic, "control/strategy") == 0)
    {
        char buf[32];
        if (length >= sizeof buf) return;
        memcpy(buf, payload, length);
        buf[length] = '\0';
        wt_hal.ctrl_set_strategy_str(buf);
        return;
    }

    if (strcmp(topic, "control/contactor") == 0)
    {
        if (payload[0] == '1') wt_hal.ctrl_contactor_set();
        return;
    }

    if (strcmp(topic, "pump") == 0)
    {
        wt_hal.pump_set(payload[0] == '1');
        return;
    }

    if (strcmp(topic, "display/backlight") == 0)
    {
        lcd_hal.backlight_set(payload[0] == '1');
        return;
    }

    if (strcmp(topic, "cli") == 0)
    {
        // payload is not null terminated
        // TODO commander currently has a bug that causes commands
        // with length >= COMMANDER_MAX_COMMAND_SIZE to overflow an internal
        // buffer: https://github.com/dani007200964/Commander-API/issues/19
        // As a workaround, I will make our buff smaller
        //char buff[COMMANDER_MAX_COMMAND_SIZE + 1];
        char buff[COMMANDER_MAX_COMMAND_SIZE];
        size_t command_length = sizeof(buff) - 1;
        if (length < command_length) command_length = length;
        memcpy(buff, payload, command_length);
        buff[command_length] = '\0';
        CLI_execute(buff);
        // TODO capture command response
        // https://github.com/JAndrassy/StreamLib
        return;
    }
}


const char * MQTT_state_to_str(int state)
{
    switch (state)
    {
        case MQTT_CONNECTION_TIMEOUT:
            return "CONNECTION_TIMEOUT";

        case MQTT_CONNECTION_LOST:
            return "CONNECTION_LOST";

        case MQTT_CONNECT_FAILED:
            return "CONNECT_FAILED";

        case MQTT_DISCONNECTED:
            return "DISCONNECTED";

        case MQTT_CONNECTED:
            return "CONNECTED";

        case MQTT_CONNECT_BAD_PROTOCOL:
            return "CONNECT_BAD_PROTOCOL";

        case MQTT_CONNECT_BAD_CLIENT_ID:
            return "CONNECT_BAD_CLIENT_ID";

        case MQTT_CONNECT_UNAVAILABLE:
            return "CONNECT_UNAVAILABLE";

        case MQTT_CONNECT_BAD_CREDENTIALS:
            return "CONNECT_BAD_CREDENTIALS";

        case MQTT_CONNECT_UNAUTHORIZED:
            return "CONNECT_UNAUTHORIZED";

        default:
            return "?";
    }
}
