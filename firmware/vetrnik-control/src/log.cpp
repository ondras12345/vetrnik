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
        // Gcc C++ allows type punning through unions
        // https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#Type%2Dpunning
        uint8_t dump[8];

        struct
        {
            uint32_t lastms;
            reset_cause_t reset_cause;
        } reboot_info;

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

        struct
        {
            uint32_t duration;
            uint32_t mid_duration;
        } slow_loop_info;

        settings_parse_error_t settings_parse_error;
    };
} log_record_t;


// https://github.com/stm32duino/Arduino_Core_STM32/wiki/API#Remembering-variables-across-resets
static log_record_t log_records[LOG_RECORD_COUNT] __attribute__((__section__(".noinit")));
/// Points to the cell that will should written next
static size_t write_index __attribute__((__section__(".noinit")));
/// Points to the cell that will should read next
static size_t read_index __attribute__((__section__(".noinit")));

static uint64_t log_magic __attribute__((__section__(".noinit")));
static const uint64_t log_magic_correct = 0x5A5ADEADBEEF5A5A;

static unsigned long log_lastms __attribute__((__section__(".noinit")));

static char print_buffer[80 + sizeof("\r\n")];


const char * get_event_string(log_event_t event)
{
#define X_CASE(const_name, message) case const_name: return message;
    switch (event)
    {
        LOG_EVENTS(X_CASE)
    }
#undef X_CASE

    return "?";
}


/**
 * Initialize logger.
 * @param first_reset set to true if __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)
 */
void log_init(bool first_reset)
{
    if (log_magic == log_magic_correct && !first_reset)
    {
        // there is postmort in memory
        // let's leave read / write index uninitialized
        if (write_index < LOG_RECORD_COUNT && read_index < LOG_RECORD_COUNT)
        {
            return;
        }
    }

    // reinitialize
    write_index = 0;
    read_index = 0;
    log_magic = log_magic_correct;
    for (size_t i = 0; i < LOG_RECORD_COUNT; i++)
    {
        log_records[i].type = kInvalid;
    }

    log_add_event(kLogReinit);
}


static size_t advance_index(size_t index)
{
    index++;
    index %= LOG_RECORD_COUNT;
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
    record.reboot_info.reset_cause = reset_cause;
    record.reboot_info.lastms = log_lastms;
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


void log_add_record_slow_loop(uint32_t duration, uint32_t mid_duration)
{
    log_record_t record;
    record.time = millis();
    record.type = kSlowLoop;
    record.slow_loop_info.duration = duration;
    record.slow_loop_info.mid_duration = mid_duration;
    log_add_record(record);
}


void log_add_record_settings_parse_error(settings_parse_error_t err)
{
    log_record_t record;
    record.time = millis();
    record.type = kSettingsParseError;
    record.settings_parse_error = err;
    log_add_record(record);
}


static void print_record(log_record_t record, Print * response)
{
    char timestamp[7 + 1 + sizeof "[.]"];
    snprintf(
        timestamp, sizeof timestamp,
        "[%7lu.%u]",
        record.time / 1000UL,
        (unsigned int)((record.time / 100UL) % 10UL)
    );

    switch (record.type)
    {
        // we already handled kInvalid

        case kReboot:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s boot: 0x%02X %s lastms=%" PRIu32 "\r\n",
                timestamp,
                record.reboot_info.reset_cause,
                reset_cause_get_name(record.reboot_info.reset_cause),
                record.reboot_info.lastms
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
                "%s MQTT publish: skipped=0x%08" PRIX32 " succeeded=0x%08" PRIX32 "\r\n",
                timestamp,
                record.MQTT_publish_info.skipped,
                record.MQTT_publish_info.succeeded
            );
            break;

        case kMqttReceive:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s MQTT receive: length=%" PRIu32 "\r\n",
                timestamp,
                record.MQTT_receive_info.length
            );
            break;

        case kMqttState:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s MQTT state: %d %s\r\n",
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

        case kSlowLoop:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s slow loop: duration=%" PRIu32 " mid_duration=%" PRIu32 " (ms)\r\n",
                timestamp,
                record.slow_loop_info.duration,
                record.slow_loop_info.mid_duration
            );
            break;

        case kSettingsParseError:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s settings parse error: %s\r\n",
                timestamp,
                settings_parse_error_message(record.settings_parse_error)
            );
            break;

        default:
            snprintf(
                print_buffer, sizeof print_buffer,
                "%s unknown: time=0x%08" PRIX32 " type=0x%02X dump=%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                timestamp,  // might not mean anything...
                record.time,
                record.type,
                record.dump[0], record.dump[1], record.dump[2], record.dump[3],
                record.dump[4], record.dump[5], record.dump[6], record.dump[7]
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


/**
 * Print new log messages.
 * @param max_count Stop after printing this many records. Set to -1 (default)
 *                  to print everything.
 * @return true if everything was printed, false if max_count was reached
 *         before everything was printed.
 */
bool log_print_new(Print * response, size_t max_count)
{
    while (read_index != write_index && max_count > 0)
    {
        print_record(log_records[read_index], response);
        read_index = advance_index(read_index);
        max_count--;
    }

    return read_index == write_index;
}


/**
 * Update lastms value.
 * This function should be called after a full loop,
 * but not before log_add_record_reboot() is called in setup.
 * Lastms (millis) value is logged at startup to aid with debugging.
 * It should help correlate the time of reset with events from previous boot.
 */
void log_update_lastms()
{
    log_lastms = millis();
}
