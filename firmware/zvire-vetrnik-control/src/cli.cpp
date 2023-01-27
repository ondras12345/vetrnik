#include "cli.h"
#include <Arduino.h>
#include <Shellminator.hpp>
#include <Shellminator-IO.hpp>
#include <Commander-API.hpp>
#include <Commander-IO.hpp>
#include <Ethernet.h>
#include "settings.h"
#include "mqtt.h"
#include "Stream_utils.h"
#include "uart_power.h"
#include "power_datapoints.h"
#include "power_board.h"
#include "control.h"
#include <SerialFlash.h>

#ifdef SHELL_TELNET
#include <TelnetStream.h>
static Shellminator shell_telnet(&TelnetStream);
#endif

#ifdef LISP_REPL
#include "lisp.h"
#endif

static Shellminator shell(&Serial);
static Commander commander;
static char watch_command[COMMANDER_MAX_COMMAND_SIZE - sizeof("watch ") + 1 + 1] = "";
static Stream * watch_response = nullptr;


static void cmnd_ifconfig(char *args, Stream *response)
{
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
}


static void cmnd_mqtt_status(char *args, Stream *response)
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


static void cmnd_rx_raw(char *args, Stream *response)  // cppcheck-suppress constParameter
{
    print_RX = (args[0] == '1') ? response : nullptr;  // TODO is response guaranteed to live after this function finishes ?
    response->print("print_RX: ");
    response->println(print_RX != nullptr);
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
    printStatU(RPM, "RPM");
    printStatC(voltage, 0.1, 1, "V");
    printStatC(current, 0.001, 3, "A");
    printStat(enabled);
    printStatC(temperature_heatsink, 0.1, 1, "'C");
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

    else if (setting_value == nullptr)
    {
        response->println("Missing value");
        response->println("Usage: power [[duty|mode] value | clear_errors]");
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


#ifdef SHELL_TELNET
static void cmnd_telnet_quit(char *args, Stream *response)
{
    if (!settings.shell_telnet)
    {
        response->println("shell_telnet is disabled in conf");
        return;
    }
    TelnetStream.flush();
    TelnetStream.stop();
}
#endif


#ifdef LISP_REPL
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
}
#endif


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
        uint8_t id[5];
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
    else if (strcmp(subcommand_name, "erase") == 0)
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

    return;
bad:
    response->println(
"Usage: SPIflash [erase|ls | create name length erasable | rm name\r\n"
"   | dump name start | dumpchip start]"
    );
}


static void cmnd_dfu(char *args, Stream *response)
{
    response->println("Jumping to DFU bootloader");
    response->flush();
    jump_to_bootloader();
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
    apiElement("mqtt_status",   "Print out MQTT status.",                   cmnd_mqtt_status),
    // ping would be nice, but seems hard to implement
    apiElement("conf",          "Get or set configuration options.",        cmnd_conf),
    apiElement("tx_raw",        "Transmit message to power board.",         cmnd_tx_raw),
    apiElement("tx",            "Set value of TX datapoint.",               cmnd_tx),
    apiElement("rx_raw",        "Toggle printing of messages from power board.", cmnd_rx_raw),
    apiElement("rx",            "Print out RX_datapoints.",                 cmnd_rx),
    apiElement("power",         "Print status of power PCB or set params.", cmnd_power),
#ifdef SHELL_TELNET
    apiElement("telnet_quit",   "Stop the telnet session.",                 cmnd_telnet_quit),
#endif
#ifdef LISP_REPL
    apiElement("lisp",          "Process a line of Lisp",                   cmnd_lisp),
    apiElement("lisp_reset",    "Reinit Lisp interpreter",                  cmnd_lisp_reset),
#endif
    apiElement("SPIflash",      "Issue commands to SPI flash",              cmnd_SPIflash),
    apiElement("dfu",           "Switch to DFU firmware download mode.",    cmnd_dfu),
    apiElement("reset",         "Reset the MCU.",                           cmnd_reset),
    apiElement("ver",           "Print out version info.",                  cmnd_ver),
    apiElement("watch",         "Run command every second.",                cmnd_watch),
    // help -d to print out descriptions
};


void CLI_init()
{
#ifdef SHELL_TELNET
    if (settings.shell_telnet)
    {
        TelnetStream.begin();
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
    }
#endif
}


void CLI_loop()
{
    shell.update();

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

        shell_telnet.update();
    }
#endif

    static unsigned long watch_prev_millis = 0;
    if (now - watch_prev_millis >= 1000UL && watch_command[0] != '\0')
    {
        watch_prev_millis = now;
        if (watch_response) commander.execute(watch_command, watch_response);
    }
}
