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


### Built-in functions
See https://github.com/rxi/fe/blob/ed4cda96bd582cbb08520964ba627efb40f3dd91/doc/lang.md


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


The `rem` function performs the operation truncate on number and divisor and
returns the remainder of the truncate operation.
```lisp
(rem 3 2) ; 1
(rem -3 2) ; -1
```

The `round` function returns the closest integer to x, rounding to even when x
is halfway between two integers.
```lisp
(round 0.4999) ; 0
(round 0.5) ; 0
(round 0.5001) ; 1

(round 1.4999) ; 1
(round 1.5) ; 2
(round 1.5001) ; 2
```

The `map` function works similar to Arduino's `map`, but it does the
computation in `float`.
```lisp
; (map value from_min from_max to_min to_max)
(map 10 0 100 -1 0) ; -0.9
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

```lisp
; linear mapping from RPM to duty

; included as a cfunc in newer firmware
;(= map (fn (v fl fh tl th)
;  (+ tl
;    (/ (* (- v fl) (- th tl)) (- fh fl))
;  )
;))

(= control (fn ()
  (let r (pwrg "RPM")) ; current RPM
  (pwrs "duty"
    (if
      (< r 10) 0
      (< r 60) (map r 10 60 0 255)
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
