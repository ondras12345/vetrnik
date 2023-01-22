# zvire-vetrnik-control
Firmware pro STM32F4. Ovladani pres MQTT apod.

## Architecture
```text
TODO finish this + add abstraction (vetrnik)

                  +---------------------------------------------+
                  v                                             |
+-----------------+---+         +---------------------+         |
|       CLI           |         |        MQTT         |         |
|                     |         |                     |         |
++--+----------+----+-+         +----+-+--+---+-------+         |
 |  |          ^    ^                ^ ^  ^   |                 |
 |  |          |    |  RX_datapoints | |  |   | TX_data-        |
 |  | cmnd_tx  |    -------------------+  |   | points_         |
 |  +----------------------------------|--|---+ set()           |
 |             |                     | |  |   |                 |
 |             |    +----------------+ |  |   |                 |
 |             |    |                  |  |   |                 |
 |             v    v                  |  |   |                 |
 |  +----------+----+--+  +----------+ |  |   |                 |
 |  |   power_board    |  | control  | |  |   |                 |
 |  |                  <--> TODO     | |  |   |                 |
 |  +-------------+--+-+  +----------+ |  |   |                 |
 |                |  ^                 |  |   |                 |
 |                +--|-----------------|--|---+                 |
 |                   +-----------------+  |   |                 |
 |                                     |  |   |                 |
 |                                     |  |   |                 |
 |            +------------------------|--+   v                 |
 |  +---------+----------+      +------+------+----+            |
 |  | power_text_message |      | power_datapoints |            |
 |  +---------+----------+      +------+------+----+            |
 |            ^                        ^      |                 |
 |            |        RX_datapoints   |      |                 |
 |            |     +------------------+      |                 |
 |            |     |                         |                 v
 |      +-----+-----+-+    TX_datapoints      |           +-----+-----+
 |      | uart_power  +<----------------------+           | USB CDC   |
 |      |             |                                   | UART      |
 |      |             |                                   |           |
 |      +--------+----+                                   +-----------+
 |               ^
 | cmnd_tx_raw   | UART
 +-------------->+
-----------------|-------------------------------------------------------------
                 V
    +------------+--------+
    | zvire-vetrnik-power |
    +---------------------+
```


## Lisp-like repl
A Lisp-like interpreted language called [fe](https://github.com/rxi/fe) is
included. It can be used by calling the `lisp` CLI command or via MQTT.

The intention is that the user can write a custom control algorithm and have
it run directly on the device without updating its firmware.

### Custom functions
The `pwrg` function is used to get values from the power board state object.
Numerical values (in SI units without metric prefix) are returned.

This expressions should return the power consumed by the load in W:
```
(* (pwrg "voltage") (pwrg "current"))
```

The `pwrs` function is used to set parameters on the power board.

```
(pwrs "duty" 10)
```

Variables are added for modes from the `power_board_mode_t` enum.
They are prefixed with `pwr_`.
```
(pwrs "mode" pwr_start)
```

### `control` function
The `control` function is empty by default. If set up for `LISP` control
strategy, the device will call this function each time a new state object is
received.
