# zvire-vetrnik-control
Firmware for STM32F4. Control via MQTT, etc.

## Architecture
```text
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
```lisp
(* (pwrg "voltage") (pwrg "current"))
```

The `pwrs` function is used to set parameters on the power board.

```lisp
(pwrs "duty" 10)
```

Variables are added for modes from the `power_board_mode_t` enum.
They are prefixed with `pwr_`.
```lisp
(pwrs "mode" pwr_start)
```

### `control` function
The `control` function is empty by default. If set up for `LISP` control
strategy, the device will call this function each time a new state object is
received.

```lisp
; "blink" control function
(= dl 50) ; duty for low
(= dh 200) ; duty for high
(= p 5) ; period in seconds
(= pt 0) ; prev time
(= control (fn ()
  ; t is used for "true", do not overwrite - named ti instead
  (let ti (pwrg "time"))
  ; max time is 65535 (see power report()), so no problem with rem
  (if (and (is (rem ti p) 0) (not (is ti pt)))
    (do
      (= pt ti)
      (if (<= (pwrg "duty") dl)
        (pwrs "duty" dh)
        (pwrs "duty" dl)
      )
    )
  )
))
```

```lisp
; simple RPM threshold control function
(= control (fn ()
  (let r (pwrg "RPM")) ; current RPM
  (pwrs "duty"
    (if
      ; pretty much random values
      (< r 10) 0
      (< r 15) 10
      (< r 20) 20
      (< r 30) 40
      (< r 60) 100
      255
    )
  )
))
```


### MISC Lisp code
Test OOM:
```lisp
(= a 0)
(while 1
  (= l (cons a l))
  (= a (+ a 1))
)
```
On PC: a = 1852, does not get completely stuck (can still create new number
variables)
On STM (8*1024 bytes): a = 329
