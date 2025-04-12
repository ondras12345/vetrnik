# vetrnik-control
Firmware for STM32F4. Handles telemetry and remote control via MQTT,
MPPT controller, etc.

## Architecture
```text
        +-------+   +-------+     +----------+   cmnd_tx_raw
        | LISP  |   | MQTT  |     | CLI      +-----------------+
        +---+---+   +---+-+-+     +-----+--+-+                 |
            |           | |             |  | cmnd_tx           |
      +-----|-----------|-+-------------|--+                   |
RX_   |     |           |               |                      |
data  | +---+-----------+---------------+---------------+      |
points| |               wt_hal / hal.c                  |      |
_get()| +--------+----------------------+---------+-----+      |
  /   |          |                      |         |            |
TX_   | +--------+----------+     +-----+-----+   |            |
data  | | power_board       |     | control   |   |            |
points| +--------+----------+     +-----------+   |            |
_set()|==========|================================|============|===============
    +-+----------+--------+                       |            |
    | power_datapoints    |                       |            |
    +------------+--------+                       |            |
                 |                                |            |
        +--------+----+                   +-------+-----+      |     HARDWARE
        | uart_power  |                   | sensor_wind |      |      DRIVERS
        +--------+----+                   +-------+-----+      |
                 |                                |            |
                 +<-------------------------------|------------+
                 | UART                           |
=================|================================|============================
                 |                                |                 HARDWARE
    +------------+--------+       +---------------+-----+
    | vetrnik-power PCB   |       | wind sensor (RS485) |
    +---------------------+       +---------------------+
```


## Lisp-like REPL
A Lisp-like interpreted language called [fe](https://github.com/rxi/fe) is
included. It can be used by calling the `lisp` CLI command or via MQTT.

The intention is that the user can write a custom control algorithm and have
it run directly on the device without updating its firmware.

See [`lisp/spec.md`](lisp/spec.md).


### MISC Lisp code
Test OOM:
```lisp
(= a 0)
(while t
  (= l (cons a l))
  (= a (+ a 1))
)
```
On PC: a = 1852, does not get completely stuck (can still create new numeric
variables)
On STM (8*1024 bytes): a = 329


Patch for testing in lab without DS18B20 sensors:
```lisp
(= ds18 (fn () 23.4))
```


## Telnet
To connect with telnet on GNU/Linux, use this command:
```sh
# netcat - better
stty -icanon -echo && nc -NC -O 1 192.168.1.188 23

# telnet - sends garbage at startup
telnet 192.168.1.188 23  # specifying the port should disable negotiation
```

It is recommended to put the following configuration in your `~/.telnetrc` if
you want to use the `telnet` command:
```
# vetrnik-control
192.168.1.188
    mode character
    # TODO improve
```


## Build
Just run `make` in this directory. GNU/Linux OS with PlatformIO installed is
required.
Type `make help` for more info.
