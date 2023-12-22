#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <Print.h>
#include "control.h"
#include "reset_cause.h"

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
    // no power board message - they are repeated too quickly
} log_record_type_t;


/**
 * Events without context
 */
typedef enum
{
    kEthernetBegin = 0,
    kEthernetIP,  // this may happen later than begin due to DHCP
    kEthernetStuck,
    kMqttConnected,
    kControlNotShorted,
    kControlMqttTimeout,
    kControlLispError,
    kControlShortEstop,
    kSettingsReset,
    kSettingsWrite,
    kPowerBoardStatusTimeout,
    kPowerBoardEmergency,
    kOtaEnabled,
    kLogReinit,
    kPumpOn,
    kPumpOff,
    kDS18B20Error,
    kSoftwareReset,
} log_event_t;

#define LOG_RECORD_COUNT 1024

void log_init(bool first_reset);
void log_add_record_reboot(reset_cause_t reset_cause);
void log_add_record_control_strategy(control_strategy_t old_strategy, control_strategy_t new_strategy);
void log_add_record_mqtt_publish(uint32_t skipped, uint32_t succeeded);
void log_add_record_mqtt_receive(uint32_t length);
void log_add_record_mqtt_state(int state);
void log_add_record_slow_loop(uint32_t duration, uint32_t mid_duration);
void log_add_event(log_event_t event);
void log_add_event_and_println(log_event_t event, Print * response);

void log_print_all(Print * response);
bool log_print_new(Print * response, size_t max_count=-1);
