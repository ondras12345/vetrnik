#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <Print.h>
#include "control.h"
#include "reset_cause.h"
#include "settings.h"

/*
 * Long-term logging & postmort
 */


typedef enum
{
    kInvalid = 0x00,
    kReboot,
    kEventWithoutContext,
    kMqttPublish,
    kMqttReceive,
    kMqttState,
    kControlStrategy,
    kSlowLoop,
    kSettingsParseError,
    // no power board message - they are repeated too quickly
} log_record_type_t;


// X(const_name, message)
#define LOG_EVENTS(X) \
    X(kEthernetBegin, "Eth begin") \
    /* this may happen later than begin due to DHCP */ \
    X(kEthernetIP, "Eth got IP") \
    X(kEthernetStuck, "Eth stuck, rst") \
    X(kMqttConnected, "MQTT connected") \
    X(kMqttReinitTime, "MQTT reinit (time)") \
    X(kControlNotShorted, "control_shorted, but mode != shorted") \
    X(kControlMqttTimeout, "MQTT control timeout") \
    X(kControlLispError, "error in control_lisp") \
    X(kControlShortEstop, "SHORT e-stop") \
    X(kSettingsReset, "Settings reset") \
    X(kSettingsWrite, "Settings write") \
    X(kSettingsErase, "Settings erase") \
    X(kPowerBoardStatusTimeout, "power_board_status timeout") \
    X(kPowerBoardEmergency, "power_board emergency") \
    X(kOtaEnabled, "OTA enabled") \
    X(kLogReinit, "log reinit") \
    X(kPumpOn, "pump on") \
    X(kPumpOff, "pump off") \
    X(kDS18B20Error, "DS18B20 error") \
    X(kSoftwareReset, "software reset") \
    X(kMillisRollover, "millis rollover")


/**
 * Events without context
 */
typedef enum
{
#define X_ENUM(const_name, message) const_name,
    LOG_EVENTS(X_ENUM)
#undef X_ENUM
} log_event_t;

#define LOG_RECORD_COUNT 1024

void log_init(bool first_reset);
void log_add_record_reboot(reset_cause_t reset_cause);
void log_add_record_control_strategy(control_strategy_t old_strategy, control_strategy_t new_strategy);
void log_add_record_mqtt_publish(uint32_t skipped, uint32_t succeeded);
void log_add_record_mqtt_receive(uint32_t length);
void log_add_record_mqtt_state(int state);
void log_add_record_slow_loop(uint32_t duration, uint32_t mid_duration);
void log_add_record_settings_parse_error(settings_parse_error_t err);
void log_add_event(log_event_t event);
void log_add_event_and_println(log_event_t event, Print * response);
void log_update_lastms();

void log_print_all(Print * response);
bool log_print_new(Print * response, size_t max_count=-1);
