#include "cli.h"
#include <Arduino.h>
#include <Shellminator.hpp>
#include <Shellminator-IO.hpp>
#include <Commander-API.hpp>
#include <Commander-API-Commands.hpp>
#include <Commander-IO.hpp>
#include <Ethernet.h>
#include "settings.h"
#include "mqtt.h"
#include "Print_utils.h"
#include "uart_power.h"
#include "power_datapoints.h"
#include "power_board.h"
#include "lisp.h"
#include "control.h"
#include "stats.h"
#include "display.h"
#include "debug.h"
#include <CLIeditor.h>
#include <SerialFlash.h>
#include <malloc.h>

#ifdef SHELL_TELNET
#include <TelnetStream.h>
static Shellminator shell_telnet(&TelnetStream);
#endif


static Shellminator shell(&Serial);
static Commander commander;
static char watch_command[COMMANDER_MAX_COMMAND_SIZE - sizeof("watch ") + 1 + 1] = "";
static Stream * watch_response = nullptr;


static bool ed_mode = false;
static bool ed_first = false;
static SerialFlashFile * ed_file = nullptr;
static Stream * ed_response = nullptr;
static CLIeditor ed;

static void ed_write_file(void * buf, size_t len)
{
    if (ed_file == nullptr) return;
    DEBUG_GENERAL->print("ed writing ");
    DEBUG_GENERAL->print(len);
    DEBUG_GENERAL->print(" at ");
    DEBUG_GENERAL->println(ed_file->position());
    if (len == 0) return;  // this doesn't seem to be handled in SerialFlash
    ed_file->write(buf, len);
}

static void ed_write_response(char c)
{
    if (ed_response == nullptr) return;
    ed_response->write(c);
}


static void cmnd_ifconfig(char *args, Stream *response)
{
    if (strcmp(args, "-r") == 0)
    {
        response->print("Resetting ETH module: ");
        response->println(MQTT_reinit());
        response->println();
    }

    response->print("HW: ");
    response->println((uint8_t)Ethernet.hardwareStatus());
    response->print("status: ");
    response->println((uint8_t)Ethernet.linkStatus());
    response->print("DHCP_mode: ");
    response->println(DHCP_mode);
    response->print("eth_skip: ");
    response->println(eth_skip);
    response->print("MQTT_skip: ");
    response->println(MQTT_skip);
    response->print("IP: ");
    response->println(Ethernet.localIP());
    response->print("mask: ");
    response->println(Ethernet.subnetMask());
    response->print("MAC: ");
    stream_print_MAC(response, settings.ETH_MAC);
    response->println();
    response->println("`ifconfig -r` to reset ETH module");
}


static void cmnd_mqtt(char *args, Stream *response)
{
    response->print("state: ");
    response->println(MQTTClient.state());
    response->print("base topic: ");
    response->println(MQTTtopic_prefix);
}


static void cmnd_conf(char *args, Stream *response)
{
    static settings_t s = settings;

    response->println("Usage: conf [name value]");
    response->println("conf -s  -- save to EEPROM");
    response->println("Configuration changes are only applied after a reset.");
    response->println();

    char * setting_name = strsep(&args, " ");
    char * setting_value = args;
    if (setting_name == nullptr)
    {
        // this is ok, just print out config
    }
    else if (strcmp(setting_name, "-s") == 0)
    {
        settings_write(s);
    }
    else if (setting_value == nullptr)
    {
        response->println("Missing value");
    }
    else if (strcmp(setting_name, "ETH_MAC") == 0)
    {
        unsigned int MAC[6];
        // newlib-nano does not support %hhx
        if (sscanf(setting_value, "%x:%x:%x:%x:%x:%x",
                   &MAC[0], &MAC[1], &MAC[2], &MAC[3], &MAC[4], &MAC[5])
            != sizeof MAC / sizeof MAC[0])
        {
            response->println("Invalid format, expected xx:xx:xx:xx:xx:xx");
        }
        else
        {
            for (uint8_t i = 0; i < sizeof MAC / sizeof MAC[0]; i++)
                s.ETH_MAC[i] = (uint8_t)MAC[i];
        }
    }
#define IPAddress_conf(name) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        IPAddress addr; \
        if (addr.fromString(setting_value)) \
        { \
            for (uint8_t i = 0; i < sizeof s.name; i++) \
                s.name[i] = addr[i]; \
        } \
        else \
        { \
            response->println("Invalid format, expected x.x.x.x"); \
        } \
    }
    IPAddress_conf(ETH_IP)
    IPAddress_conf(MQTTserver)
#define String_conf(name) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        if (strlen(setting_value) < sizeof s.name) \
        { \
            /* fill the rest of the memory with zeros */ \
            strncpy(s.name, setting_value, sizeof s.name); \
        } \
        else \
            response->println("String too long"); \
    }
    String_conf(MQTTuser)
    String_conf(MQTTpassword)
#define Bool_conf(name) \
    else if (strcmp(setting_name, #name) == 0) \
    { \
        s.name = (setting_value[0] == '1'); \
    }
    Bool_conf(shell_telnet)
    Bool_conf(report_raw)
    else
    {
        response->print("Invalid config option: ");
        response->println(setting_name);
    }

    response->println();
    response->println("Current configuration:");
    stream_print_settings(response, settings);

    response->println();
    response->println("Next boot configuration:");
    stream_print_settings(response, s);
}


static void cmnd_tx_raw(char *args, Stream *response)
{
    PSerial.println(args);
}


static void cmnd_tx(char *args, Stream *response)
{
    char dp = args[0];
    int value = 0;
    if (dp == '\0')
    {
        goto usage;
    }
    if (args[1] != '=')
    {
        goto usage;
    }

    if (sscanf(args+2, "%i", &value) < 1)
    {
        goto usage;
    }

    if (value < 0 || value > 255)
    {
        response->println(F("value out of range"));
        return;
    }

    TX_datapoints_set(dp, value);
    return;

usage:
    response->println(F("Usage: tx DP=value"));
    return;
}


static Stream * print_RX_response = nullptr;
static void print_RX(char c)
{
    if (print_RX_response) print_RX_response->print(c);
}


static void cmnd_rx_raw(char *args, Stream *response)  // cppcheck-suppress constParameter
{
    print_RX_response = (args[0] == '1') ? response : nullptr;
    response->print("print raw RX: ");
    response->println(print_RX_response != nullptr);
}


static void cmnd_rx(char *args, Stream *response)
{
    response->println("Non-zero RX datapoints:");
    for (char c = '!'; c <= '~'; c++)
    {
        RX_datapoint_t dp = RX_datapoints_get(c);
        if (dp.value != 0)
        {
            response->print(c);
            response->print(" = ");
            response->println(dp.value);
        }
    }
}


static void print_power_board_status(Stream *response)
{
    response->println("power board status:");
#define printStat(name) \
    response->print("" #name ": "); \
    response->println(power_board_status.name);
#define printStatU(name, unit) \
    response->print("" #name ": "); \
    response->print(power_board_status.name); \
    response->println(" " unit);
#define printStatC(name, conversion, dp, unit) \
    response->print("" #name ": "); \
    response->print(power_board_status.name * conversion, dp); \
    response->println(" " unit); \

    printStat(retrieved_millis);
    printStat(valid);
    printStatU(time, "s");
    printStat(mode);
    printStatU(duty, "(0-255)");
    printStatU(OCP_max_duty, "(0-255)");
    printStatU(RPM, "RPM");
    printStatC(voltage, 0.1, 1, "V");
    printStatC(current, 0.001, 3, "A");
    printStat(enabled);
    printStat(emergency);
    printStatC(temperature_heatsink, 0.1, 1, "'C");
    printStatC(temperature_rectifier, 0.1, 1, "'C");
    printStat(fan);
    printStat(error_count);
#undef printStat
#undef printStatU
#undef printStatC
}


static void cmnd_power(char *args, Stream *response)
{
    char * setting_name = strsep(&args, " ");
    char * setting_value = args;
    if (setting_name == nullptr)
    {
        // do nothing, just print out status
        print_power_board_status(response);
    }

    // subcommands that need no value
    else if (strcmp(setting_name, "clear_errors") == 0)
    {
        power_board_clear_errors();
        response->println("Power board errors cleared.");
    }

    else if (strcmp(setting_name, "reset") == 0)
    {
        power_board_command(PCOMMAND_RESET);
    }

    else if (strcmp(setting_name, "WDT_test") == 0)
    {
        power_board_command(PCOMMAND_WDT_TEST);
    }

    else if (setting_value == nullptr)
    {
        response->println("Missing value");
        response->println("Usage: power [[duty|mode] value | clear_errors | reset | WDT_test]");
        goto bad;
    }

    // subcommands that need setting_value
    else if (strcmp(setting_name, "duty") == 0)
    {
        unsigned int duty;
        sscanf(setting_value, "%u", &duty);
        if (duty > 255) {
            response->println("duty must be 0-255");
            goto bad;
        }
        power_board_set_duty(duty);
        response->print("Setting duty to ");
        response->println(duty);
    }
    else if (strcmp(setting_name, "mode") == 0)
    {
        bool found = false;
        for (size_t i = 0; power_board_modes[i] != nullptr; i++)
        {
            if (strcmp(setting_value, power_board_modes[i]) == 0)
            {
                found = true;
                power_board_mode_t mode = (power_board_mode_t)i;
                response->print("Setting mode to ");
                response->println(mode);
                power_board_set_mode(mode);
            }
        }
        if (!found)
        {
            response->print("Unknown mode: ");
            response->println(setting_value);
            response->print("modes: ");
            for (size_t i = 0; power_board_modes[i] != nullptr; i++)
            {
                response->print(power_board_modes[i]);
                response->print(' ');
            }
            response->println();
        }

    }

bad:
    return;
}


static void cmnd_control(char *args, Stream *response)
{
    char * setting_name = strsep(&args, " ");
    char * setting_value = args;
    if (setting_name == nullptr)
    {
        // do nothing, just print out status
        response->print("strategy: ");
        response->println(control_strategies[control_get_strategy()]);
    }

    // subcommands that need no value
    // (currently none)

    else if (setting_value == nullptr)
    {
        goto bad;
    }

    // subcommands that need setting_value
    else if (strcmp(setting_name, "strategy") == 0)
    {
        if (!control_set_strategy(setting_value))
        {
            response->print("Unknown strategy: ");
            response->println(setting_value);
            response->print("strategies: ");
            for (size_t i = 0; control_strategies[i] != nullptr; i++)
            {
                response->print(control_strategies[i]);
                response->print(' ');
            }
            response->println();
        }
    }
    else
    {
        response->println("Invalid subcommand");
        goto bad;
    }

    return;
bad:
    response->println("Missing value");
    response->println("Usage: control [strategy s]");
}


static void cmnd_stats(char *args, Stream *response)
{
    response->println("stats:");
    response->printf("energy: %u.%02u kWh\r\n",
                     stats.energy / 100U, stats.energy % 100U);
}


#ifdef SHELL_TELNET

static void telnet_logout()
{
    TelnetStream.flush();
    TelnetStream.stop();
}

#endif


static void cmnd_lisp(char *args, Stream *response)
{
    response->printf("Executing: '%s'\r\n", args);
    bool success = lisp_process(args, response);
    response->println();
    response->print("Success: ");
    response->println(success);
}


static void cmnd_lisp_reset(char *args, Stream *response)
{
    lisp_reinit();
    control_init_lisp();
    display_init_lisp();
    // Loading init.lisp should be done manually using cmnd_lisp_read
    //lisp_run_blind_file(LISP_INIT_FILENAME);
}


static void cmnd_lisp_read(char *args, Stream *response)
{
    char * filename = strsep(&args, " ");
    char * offstr = args;
    if (filename == nullptr || offstr == nullptr)
    {
        response->println("Usage: lisp_read filename offset");
        return;
    }
    unsigned int offset = strtoul(offstr, nullptr, 0);  // should accept hex 0x
    response->print("Executing lisp from '");
    response->print(filename);
    response->println("'");
    response->println(lisp_run_blind_file(filename, offset));
}


static void cmnd_SPIflash(char *args, Stream *response)
{
    char * subcommand_name = strsep(&args, " ");
    char * subcommand_args = args;
    if (subcommand_name == nullptr)
    {
        // do nothing, just print out status
        response->print("ready: ");
        response->println(SerialFlash.ready());
        response->print("ID:");
        uint8_t id[5] = {0};
        SerialFlash.readID(id);
        for (uint8_t i = 0; i < sizeof id; i++)
        {
            response->printf(" %02x", id[i]);
        }
        response->println();
        // Not supported by this chip (reads all FFs):
        //response->print("serial NR:");
        //uint8_t serial[8];
        //SerialFlash.readSerialNumber(serial);
        //for (uint8_t i = 0; i < sizeof serial; i++)
        //{
        //    response->printf(" %02x", serial[i]);
        //}
        //response->println();
        response->print("capacity: ");
        response->println(SerialFlash.capacity(id));
        response->print("block size: ");
        response->println(SerialFlash.blockSize());
    }

    // subcommands that need no args
    else if (strcmp(subcommand_name, "erase_chip") == 0)
    {
        SerialFlash.eraseAll();
        response->println("Erasing");
    }
    else if (strcmp(subcommand_name, "ls") == 0)
    {
        response->println("Files:");
        response->println("address (HEX)\tname\tsize (Bytes)");
        SerialFlash.opendir();
        char filename[32];
        uint32_t filesize;
        uint32_t address = 0;
        while (SerialFlash.readdir(filename, sizeof filename, filesize))
        {
            SerialFlashFile f = SerialFlash.open(filename);
            if (f)
            {
                address = f.getFlashAddress();
                f.close();
            }
            response->printf("%06x\t%s\t%u\r\n", address, filename, filesize);
        }
    }

    else if (subcommand_args == nullptr)
    {
        response->println("Missing subcommand args");
        goto bad;
    }

    // subcommands that need subcommand_args
    else if (strcmp(subcommand_name, "rm") == 0)
    {
        response->println("rm does NOT reclaim space, just filename");
        response->print("success: ");
        response->println(SerialFlash.remove(subcommand_args));
    }
    else if (strcmp(subcommand_name, "create") == 0)
    {
        char * filename = strsep(&subcommand_args, " ");
        char * lengthstr = strsep(&subcommand_args, " ");
        char * erasablestr = subcommand_args;
        if (filename == nullptr || lengthstr == nullptr || erasablestr == nullptr)
        {
            response->println("Invalid args");
            goto bad;
        }
        unsigned int length = 0;
        sscanf(lengthstr, "%u", &length);
        bool erasable = erasablestr[0] == '1';
        response->printf("Creating %sfile with name \"%s\" length %u\r\n",
                         erasable ? "erasable " : "", filename, length);
        response->print("success: ");
        response->println(
            erasable ? SerialFlash.createErasable(filename, length)
            : SerialFlash.create(filename, length)
        );
    }
    else if (strcmp(subcommand_name, "dump") == 0)
    {
        char * filename = strsep(&subcommand_args, " ");
        char * startstr = subcommand_args;
        if (filename == nullptr || startstr == nullptr)
        {
            response->println("Invalid args");
            goto bad;
        }

        unsigned int start = strtoul(startstr, nullptr, 0);  // should accept hex 0x
        SerialFlashFile f = SerialFlash.open(filename);
        if (!f)
        {
            response->println("Cannot open file");
            return;
        }
        uint8_t buf[256];
        f.seek(start);
        stream_hexdump(response, buf, f.read(buf, sizeof buf), start);
        f.close();
    }
    else if (strcmp(subcommand_name, "erase") == 0)
    {
        SerialFlashFile f = SerialFlash.open(subcommand_args);
        if (!f)
        {
            response->println("Cannot open file");
            return;
        }
        response->println("(only files created as erasable can be erased)");
        f.erase();
        f.close();
    }
    else if (strcmp(subcommand_name, "ed") == 0)
    {
        if (ed_mode)
        {
            // could be used by the other CLI
            response->println("Already in ed mode");
            return;
        }
        char * filename = strsep(&subcommand_args, " ");
        char * startstr = subcommand_args;
        if (filename == nullptr || startstr == nullptr)
        {
            response->println("Invalid args");
            goto bad;
        }

        unsigned int start = strtoul(startstr, nullptr, 0);  // should accept hex 0x
        static SerialFlashFile f;
        f = SerialFlash.open(filename);
        if (!f)
        {
            response->println("Cannot open file");
            return;
        }
        f.seek(start);
        ed_mode = true;
        ed_first = true;
        ed_file = &f;
        ed_response = response;
        ed = CLIeditor(ed_write_response, ed_write_file);
        return;
    }
    else if (strcmp(subcommand_name, "dumpchip") == 0)
    {
        unsigned int start = strtoul(subcommand_args, nullptr, 0);  // should accept hex 0x
        uint8_t id[5];
        SerialFlash.readID(id);
        uint32_t capacity = SerialFlash.capacity(id);
        uint8_t buf[256];
        if (start >= capacity)
        {
            response->println("Address out of range");
            return;
        }
        uint32_t len = sizeof buf;
        if (len > capacity - start) len = capacity - start;
        SerialFlash.read(start, buf, len);
        stream_hexdump(response, buf, len, start);
    }
    else
    {
        response->println("Invalid subcommand");
        goto bad;
    }

    return;
bad:
    response->println(
"Usage: SPIflash [erase_chip|ls | create name length erasable | rm name\r\n"
"   | dump name start | dumpchip start | erase filename | ed filename start]"
    );
}


static void cmnd_log(char *args, Stream *response)
{
    char * setting_name = strsep(&args, " ");
    char * setting_value = args;
    if (setting_name == nullptr)
    {
        // print out the circular buffer
        DEBUG_buffer.print_out(response);
    }
    else if (setting_value == nullptr)
    {
        response->println("Missing value");
        goto bad;
    }
    else if (strcmp(setting_name, "power") == 0)
    {
        log_set_DEBUG_PB(setting_value[0] == '1');
    }
    else if (strcmp(setting_name, "MQTT") == 0)
    {
        log_set_DEBUG_MQTT(setting_value[0] == '1');
    }
    else if (strcmp(setting_name, "general") == 0)
    {
        log_set_DEBUG_GENERAL(setting_value[0] == '1');
    }
    else
    {
        goto bad;
    }
    return;
bad:
    response->println("Usage: log [[power|MQTT|general] (1|0)]");
}


extern "C" char *sbrk(int i);
static void cmnd_free(char *args, Stream *response)
{
    // https://github.com/stm32duino/STM32Examples/blob/b875d3a1781ee0d82690ed7a350efdf43a81c42f/examples/Benchmarking/MemoryAllocationStatistics/MemoryAllocationStatistics.ino
    /* Use linker definition */
    extern char _end;
    extern char _sdata;
    extern char _estack;
    extern char _Min_Stack_Size;

    static char *ramstart = &_sdata;
    static char *ramend = &_estack;
    static char *minSP = (char*)(ramend - &_Min_Stack_Size);  // cppcheck-suppress comparePointers

    char *heapend = (char*)sbrk(0);
    char * stack_ptr = (char*)__get_MSP();
    struct mallinfo mi = mallinfo();

    response->print("Total non-mmapped bytes (arena):       ");
    response->println(mi.arena);
    response->print("# of free chunks (ordblks):            ");
    response->println(mi.ordblks);
    response->print("# of free fastbin blocks (smblks):     ");
    response->println(mi.smblks);
    response->print("# of mapped regions (hblks):           ");
    response->println(mi.hblks);
    response->print("Bytes in mapped regions (hblkhd):      ");
    response->println(mi.hblkhd);
    response->print("Max. total allocated space (usmblks):  ");
    response->println(mi.usmblks);
    response->print("Free bytes held in fastbins (fsmblks): ");
    response->println(mi.fsmblks);
    response->print("Total allocated space (uordblks):      ");
    response->println(mi.uordblks);
    response->print("Total free space (fordblks):           ");
    response->println(mi.fordblks);
    response->print("Topmost releasable block (keepcost):   ");
    response->println(mi.keepcost);

    response->print("RAM Start at:       0x");
    response->println((unsigned long)ramstart, HEX);
    response->print("Data/Bss end at:    0x");
    response->println((unsigned long)&_end, HEX);
    response->print("Heap end at:        0x");
    response->println((unsigned long)heapend, HEX);
    response->print("Stack Ptr end at:   0x");
    response->println((unsigned long)stack_ptr, HEX);
    response->print("RAM End at:         0x");
    response->println((unsigned long)ramend, HEX);

    response->print("Heap RAM Used:      ");
    response->println(mi.uordblks);
    response->print("Program RAM Used:   ");
    response->println(&_end - ramstart);
    response->print("Stack RAM Used:     ");
    response->println(ramend - stack_ptr);
    response->print("Estimated Free RAM: ");
    response->println(((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks);
}


static void cmnd_reset(char *args, Stream *response)
{
    response->println("Resetting");
    delay(100);
    NVIC_SystemReset();
}


// https://stm32f4-discovery.net/2017/04/tutorial-jump-system-memory-software-stm32/
// https://github.com/markusgritsch/SilF4ware/blob/12d0371ac2f5fd561d653acd041caff73cf3cff4/SilF4ware/drv_reset.c
#define SYSTEM_MEMORY_START 0x1FFF0000
static void jump_to_bootloader()
{
    __enable_irq();
    HAL_RCC_DeInit();
    HAL_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

    // Set stack pointer
    // This will break local variables in this function
    const uint32_t p = (*((uint32_t *) SYSTEM_MEMORY_START));
    __set_MSP(p);

    void (*SysMemBootJump)(void);
    SysMemBootJump = (void (*)(void)) (*((uint32_t *)(SYSTEM_MEMORY_START+4)));
    SysMemBootJump();

    for (;;);
}
#undef SYSTEM_MEMORY_START


static void cmnd_dfu(char *args, Stream *response)
{
    response->println("Jumping to DFU bootloader");
    response->flush();
    jump_to_bootloader();
    // TODO watchdog ??
}


static void cmnd_ver(char *args, Stream *response)
{
    response->println(__DATE__ " " __TIME__);
}


static void cmnd_watch(char *args, Stream *response)
{
    if (strlen(args) < sizeof(watch_command))
    {
        strcpy(watch_command, args);
        watch_response = response;
    }
    else
    {
        response->println("command too long; should not happen");
    }
    response->print("Every 1s: '");
    response->print(watch_command);
    response->println("'");
}


Commander::API_t API_tree[] = {
    apiElement("ifconfig",      "Print out networking information.",        cmnd_ifconfig),
    apiElement("mqtt",          "Print out MQTT status.",                   cmnd_mqtt),
    // ping would be nice, but seems hard to implement
    apiElement("conf",          "Get or set configuration options.",        cmnd_conf),
    apiElement("tx_raw",        "Transmit message to power board.",         cmnd_tx_raw),
    apiElement("tx",            "Set value of TX datapoint.",               cmnd_tx),
    apiElement("rx_raw",        "Toggle printing of messages from power board.", cmnd_rx_raw),
    apiElement("rx",            "Print out RX_datapoints.",                 cmnd_rx),
    apiElement("power",         "Print status of power PCB or set params.", cmnd_power),
    apiElement("control",       "Get or set params of control algorithm.",  cmnd_control),
    apiElement("stats",         "Get stats (energy, ...)",                  cmnd_stats),
    apiElement("lisp",          "Process a line of Lisp",                   cmnd_lisp),
    apiElement("lisp_reset",    "Reinit Lisp interpreter",                  cmnd_lisp_reset),
    apiElement("lisp_read",     "Execute Lisp from file",                   cmnd_lisp_read),
    apiElement("SPIflash",      "Issue commands to SPI flash",              cmnd_SPIflash),
    apiElement("log",           "Filter debug messages",                    cmnd_log),
    apiElement("free",          "Print out amount of free memory.",         cmnd_free),
    apiElement("dfu",           "Switch to DFU firmware download mode.",    cmnd_dfu),
    apiElement("reset",         "Reset the MCU.",                           cmnd_reset),
    apiElement("ver",           "Print out version info.",                  cmnd_ver),
    apiElement("watch",         "Run command every second.",                cmnd_watch),
    // commander pre-made commands
    API_ELEMENT_MILLIS,
    API_ELEMENT_UPTIME,
    // digitalRead, analogRead look too primitive to be useful. They also don't
    // validate args, that could potentially be dangerous.
};


void CLI_init()
{
    print_RX_callback = print_RX;

#ifdef SHELL_TELNET
    if (settings.shell_telnet)
    {
        TelnetStream.begin();
        log_add_INFO_backend(&TelnetStream);
    }
#endif

    // Clear the terminal
    shell.clear();

    // Attach the logo.
    //shell.attachLogo( logo );

    // There is an option to attach a debug channel to Commander.
    // It can be handy to find any problems during the initialization
    // phase.
    //commander.attachDebugChannel( &Serial );

    commander.attachTree(API_tree);

    commander.init();

    shell.attachCommander(&commander);

    shell.begin("vetrnik-control");

#ifdef SHELL_TELNET
    if (settings.shell_telnet)
    {
        shell_telnet.attachCommander(&commander);
        shell_telnet.begin("vetrnik-control");
        shell_telnet.overrideLogoutKey(telnet_logout);
    }
#endif
}


void CLI_loop()
{
    if (ed_mode)
    {
        if (ed_first)
        {
            // don't leave the cursor at the line with Shellminator's prompt
            ed_response->println("ed");
            ed_response->println("type '\\n.\\n' to exit");
            ed_first = false;
        }
        // Process a maximum of 100 characters at a time to prevent blocking
        // the main loop
        for (uint8_t i = 0; i < 100 && ed_response->available(); i++)
        {
            char c = ed_response->read();
            if (ed.process(c))
            {
                ed_mode = false;
                ed_file->close();
            }
        }
    }

    if (!ed_mode || ed_response != &Serial) shell.update();

    unsigned long now = millis();

#ifdef SHELL_TELNET
    if (settings.shell_telnet)
    {
        static unsigned long telnet_prev_millis = 0;
        if (now - telnet_prev_millis >= 1000UL)
        {
            telnet_prev_millis = now;
            // TODO this is critical, otherwise telnet starts refusing connections
            TelnetStream.write('\a');
        }

        if (!ed_mode || ed_response != &TelnetStream) shell_telnet.update();
    }
#endif

    static unsigned long watch_prev_millis = 0;
    if (now - watch_prev_millis >= 1000UL && watch_command[0] != '\0')
    {
        watch_prev_millis = now;
        if (watch_response) commander.execute(watch_command, watch_response);
    }
}
