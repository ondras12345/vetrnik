#include "log.h"
#include <Arduino.h>
#include <inttypes.h>
#include "control.h"
#include "mqtt.h"

typedef struct
{
    uint32_t time;
    log_record_type_t type;
    union
    {
        uint64_t magic;

        reset_cause_t reboot_reset_cause;

        log_event_t event;

        struct
        {
            uint32_t skipped;
            uint32_t succeeded;
        } MQTT_publish_info;

        struct
        {
            uint32_t length;
        } MQTT_receive_info;

        int MQTT_state;

        struct
        {
            control_strategy_t old_strategy;
            control_strategy_t new_strategy;
        } control_strategy_info;
    };
} log_record_t;


// https://github.com/stm32duino/Arduino_Core_STM32/wiki/API#Remembering-variables-across-resets
static log_record_t log_records[LOG_RECORD_COUNT] __attribute__((__section__(".noinit")));
/// Points to the cell that will should written next
static size_t write_index __attribute__((__section__(".noinit")));
/// Points to the cell that will should read next
static size_t read_index __attribute__((__section__(".noinit")));

static const uint32_t log_magic_1 = 0xA5A5A5A5;
static const uint64_t log_magic_2 = 0xDEADBEEF5A5A5A5A;

static char print_buffer[80 + sizeof("\r\n")];


const char * get_event_string(log_event_t event)
{
    switch (event)
    {
        case kEthernetBegin:
            return "Eth begin";

        case kEthernetIP:
            return "Eth got IP";

        case kEthernetStuck:
            return "Eth stuck, rst";

        case kMqttConnected:
            return "MQTT connected";

        case kControlNotShorted:
            return "control_shorted, but mode != shorted";

        case kControlMqttTimeout:
            return "MQTT control timeout";

        case kControlLispError:
            return "error in control_lisp";

        case kControlShortEstop:
            return "SHORT e-stop";

        case kSettingsReset:
            return "Settings reset";

        case kSettingsWrite:
            return "Settings write";

        case kPowerBoardStatusTimeout:
            return "power_board_status timeout";

        case kPowerBoardEmergency:
            return "power_board emergency";

        case kOtaEnabled:
            return "OTA enabled";

        case kLogReinit:
            return "log reinit";
    }

    return "?";
}


/**
 * Initialize logger.
 * @param first_reset set to true if __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)
 */
void log_init(bool first_reset)
{
    // first record is magic
    bool magic_valid = (
        log_records[0].type == kLogMagic &&
        log_records[0].time == log_magic_1 &&
        log_records[0].magic == log_magic_2
    );

    if (magic_valid && !first_reset)
    {
        // there is postmort in memory
        // let's leave read / write index uninitialized
        if (write_index < LOG_RECORD_COUNT && read_index < LOG_RECORD_COUNT)
        {
            return;
        }
    }

    // reinitialize
    write_index = 1;
    read_index = 1;
    log_records[0].type = kLogMagic;
    log_records[0].time = log_magic_1;
    log_records[0].magic = log_magic_2;
    for (size_t i = 1; i < LOG_RECORD_COUNT; i++)
    {
        log_records[i].type = kInvalid;
    }

    log_add_event(kLogReinit);
}


static size_t advance_index(size_t index)
{
    index++;
    // index 0 is magic, skip that
    if (index >= LOG_RECORD_COUNT) index = 1;
    return index;
}


// cppcheck-suppress passedByValue
static void log_add_record(log_record_t record)
{
    log_records[write_index] = record;
    write_index = advance_index(write_index);
    if (write_index == read_index)
        read_index = advance_index(read_index);
}


void log_add_event(log_event_t event)
{
    log_record_t record;
    record.time = millis();
    record.type = kEventWithoutContext;
    record.event = event;
    log_add_record(record);
}


void log_add_event_and_println(log_event_t event, Print * response)
{
    log_add_event(event);
    if (response != nullptr) response->println(get_event_string(event));
}


void log_add_record_reboot(reset_cause_t reset_cause)
{
    log_record_t record;
    record.time = 0;
    record.type = kReboot;
    record.reboot_reset_cause = reset_cause;
    log_add_record(record);
}


void log_add_record_control_strategy(control_strategy_t old_strategy,
                                     control_strategy_t new_strategy
                                     )
{
    log_record_t record;
    record.time = millis();
    record.type = kControlStrategy;
    record.control_strategy_info.new_strategy = new_strategy;
    record.control_strategy_info.old_strategy = old_strategy;
    log_add_record(record);
}


void log_add_record_mqtt_publish(uint32_t skipped, uint32_t succeeded)
{
    log_record_t record;
    record.time = millis();
    record.type = kMqttPublish;
    record.MQTT_publish_info.skipped = skipped;
    record.MQTT_publish_info.succeeded = succeeded;
    log_add_record(record);
}


void log_add_record_mqtt_receive(uint32_t length)
{
    log_record_t record;
    record.time = millis();
    record.type = kMqttReceive;
    record.MQTT_receive_info.length = length;
    log_add_record(record);
}


void log_add_record_mqtt_state(int state)
{
    log_record_t record;
    record.time = millis();
    record.type = kMqttState;
    record.MQTT_state = state;
    log_add_record(record);
}


static void print_record(log_record_t record, Print * response)
{
    char timestamp[7+2+1];
    snprintf(
        timestamp, sizeof timestamp,
        "[%7lu]",
        record.time / 1000UL
    );

    switch (record.type)
    {
        // we already handled kInvalid

        // we should never see kLogMagic

        case kReboot:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s boot: 0x%02X %s\r\n",
                timestamp,
                record.reboot_reset_cause,
                reset_cause_get_name(record.reboot_reset_cause)
            );
            break;

        case kEventWithoutContext:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s event: 0x%02X %s\r\n",
                timestamp,
                record.event, get_event_string(record.event)
            );
            break;

        case kMqttPublish:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s MQTT publish: skipped=0x%02X new=%02X\r\n",
                timestamp,
                record.MQTT_publish_info.skipped,
                record.MQTT_publish_info.succeeded
            );
            break;

        case kMqttReceive:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s MQTT receive: length=%u\r\n",
                timestamp,
                record.MQTT_receive_info.length
            );
            break;

        case kMqttState:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s MQTT state: %d %s",
                timestamp,
                record.MQTT_state,
                MQTT_state_to_str(record.MQTT_state)
            );
            break;

        case kControlStrategy:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s control strategy: old=%s new=%s\r\n",
                timestamp,
                // This is a bit unsafe - if we are parsing a malformed
                // record, we might read strings from random memory locations.
                // snprintf should protect us.
                control_strategies[record.control_strategy_info.old_strategy],
                control_strategies[record.control_strategy_info.new_strategy]
            );
            break;

        default:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s unknown: time=0x%08" PRIX32 " type=0x%02X magic=0x%016" PRIX64 "\r\n",
                timestamp,  // might not mean anything...
                record.time,
                record.type,
                record.magic
            );
            break;
    }
    response->print(print_buffer);
}


void log_print_all(Print * response)
{
    bool was_invalid = false;
    for (size_t rp = advance_index(write_index); rp != write_index; rp = advance_index(rp))
    {
        // We want to printf to a buffer and then print() that, because
        // otherwise telnetstream would send one packet per character
        // (I think).
        log_record_t record = log_records[rp];

        if (record.type == kInvalid)
        {
            // skip empty records
            was_invalid = true;
            continue;
        }

        if (was_invalid)
        {
            response->print("---\r\n");
            was_invalid = false;
        }

        print_record(record, response);
    }
}


void log_print_new(Print * response)
{
    while (read_index != write_index)
    {
        print_record(log_records[read_index], response);
        read_index = advance_index(read_index);
    }
}
