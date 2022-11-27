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

Shellminator shell(&Serial);
Commander commander;
static char watch_command[SHELLMINATOR_BUFF_LEN - sizeof("watch ") + 1 + 1] = "";

void cmnd_ifconfig(char *args, Stream *response)
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

void cmnd_mqtt_status(char *args, Stream *response)
{
    response->print("state: ");
    response->println(MQTTClient.state());
    response->print("base topic: ");
    response->println(MQTTtopic_prefix);
}


void cmnd_conf(char *args, Stream *response)
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


void cmnd_tx_raw(char *args, Stream *response)
{
    PSerial.println(args);
}


void cmnd_tx(char *args, Stream *response)
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


void cmnd_rx_raw(char *args, Stream *response)  // cppcheck-suppress constParameter
{
    print_RX = (args[0] == '1') ? response : nullptr;  // TODO is response guaranteed to live after this function finishes ?
    response->print("print_RX: ");
    response->println(print_RX != nullptr);
}


void cmnd_dp_rx(char *args, Stream *response)
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


void cmnd_reset(char *args, Stream *response)
{
    response->println("Resetting");
    delay(100);
    NVIC_SystemReset();
}





//#include "stm32f4xx_hal.h"
//
//#define SYSMEM_RESET_VECTOR            0x1fffC804
//#define RESET_TO_BOOTLOADER_MAGIC_CODE 0xDEADBEEF
//#define BOOTLOADER_STACK_POINTER       0x20002250  // TODO probably wrong
//
//uint32_t dfu_reset_to_bootloader_magic;
//
//void __initialize_hardware_early(void)
//{
//    if (dfu_reset_to_bootloader_magic == RESET_TO_BOOTLOADER_MAGIC_CODE) {
//        void (*bootloader)(void) = (void (*)(void)) (*((uint32_t *) SYSMEM_RESET_VECTOR));
//        dfu_reset_to_bootloader_magic = 0;
//        __set_MSP(BOOTLOADER_STACK_POINTER);
//        bootloader();
//        while (42);
//    } else {
//        SystemInit();
//    }
//}
//
//void dfu_run_bootloader()
//{
//    dfu_reset_to_bootloader_magic = RESET_TO_BOOTLOADER_MAGIC_CODE;
//    NVIC_SystemReset();
//}

void cmnd_dfu(char *args, Stream *response)
{
    // https://community.arm.com/support-forums/f/keil-forum/36389/stm32f411vetx-enabling-dfu-mode-from-application-code
    //*((unsigned long *)0x2001FFF0) = 0xDEADBEEF; // Write a scratch location at end of RAM (or wherever)
    //NVIC_SystemReset();
    // TODO https://stackoverflow.com/questions/28288453/how-do-you-jump-to-the-bootloader-dfu-mode-in-software-on-the-stm32-f072
    response->println("Unimplemented");
    //dfu_run_bootloader();
}

void cmnd_ver(char *args, Stream *response)
{
    response->println(__DATE__ " " __TIME__);
}


void cmnd_watch(char *args, Stream *response)
{
    if (strlen(args) < sizeof(watch_command))
    {
        strcpy(watch_command, args);
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
    apiElement("conf",          "Get or set configuration options.",        cmnd_conf),
    apiElement("tx_raw",        "Transmit message to power board.",         cmnd_tx_raw),
    apiElement("tx",            "Set value of TX datapoint.",               cmnd_tx),
    apiElement("rx_raw",        "Toggle printing of messages from power board.", cmnd_rx_raw),
    apiElement("dp_rx",         "Print out RX_datapoints.",                 cmnd_dp_rx),
    apiElement("dfu",           "Switch to DFU firmware download mode.",    cmnd_dfu),
    apiElement("reset",         "Reset the MCU.",                           cmnd_reset),
    apiElement("ver",           "Print out version info.",                  cmnd_ver),
    apiElement("watch",         "Run command every second.",                cmnd_watch),
    // help -d to print out descriptions
};


void CLI_init()
{
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
}


void CLI_loop()
{
    shell.update();
    unsigned long now = millis();
    static unsigned long watch_prev_millis = 0;
    if (now - watch_prev_millis >= 1000UL && watch_command[0] != '\0')
    {
        watch_prev_millis = now;
        commander.execute(watch_command, &Serial);
    }
}
