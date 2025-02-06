# Language
See [fe language overview](https://github.com/rxi/fe/blob/master/doc/lang.md)
for more information about the language and built-in functions.

# Types
- number: a floating-point number
- string: a sequence of characters
- nil: always `nil`
- bool: either `nil` (false), or any other value (true)
- any: placeholder used in this document to indicate values of
  more than one type are acceptable.

# Categories
- LCD: functions that control the display
- callbacks: functions to be implemented by the user to respond to various events

# Custom functions
## `rem`
### Description
Remainder function.

Performs the operation truncate on number and divisor and returns the
remainder of the truncate operation.

https://stackoverflow.com/questions/5706398/how-to-get-the-modulus-in-lisp

### Signature
```lisp
(rem
    x
    y
)
```


### Arguments
- `x` \[number\]
- `y` \[number\]

### Return value
\[number\]: `(int)x % (int)y`

### Example
```lisp
(rem 3 2) ; 1
(rem -3 2) ; -1
```

## `round`
### Description
Return the closest integer to x, rounding to even when x is halfway between two integers.

### Signature
```lisp
(round
    x
)
```


### Arguments
- `x` \[number\]: number to round

### Return value
\[number\]

### Example
```lisp
(round 0.4999) ; 0
(round 0.5) ; 0
(round 0.5001) ; 1

(round 1.4999) ; 1
(round 1.5) ; 2
(round 1.5001) ; 2
```

## `map`
### Description
[Arduino-like
map](https://docs.arduino.cc/language-reference/en/functions/math/map/)
function, but isn't as broken (uses floating point
arithmetic).

This could be easily implemented in Lisp, but a cfunc takes up less memory.
Equivalent lisp implementation:
```lisp
(= map (fn (v fl fh tl th)
  (+ tl
    (/ (* (- v fl) (- th tl)) (- fh fl))
  )
))
```

### Signature
```lisp
(map
    value
    from_low
    from_high
    to_low
    to_high
)
```


### Arguments
- `value` \[number\]: the number to map
- `from_low` \[number\]: lower bound of `value`'s current range
- `from_high` \[number\]: upper bound of `value`'s current range
- `to_low` \[number\]: lower bound of `value`'s target range
- `to_high` \[number\]: upper bound of `value`'s target range

### Return value
\[number\]: `value` remapped to the target range

### Example
```lisp
(map 10 0 100 0 1) ; 0.1
(map 10 0 100 -1 0) ; -0.9
```

## `pwrg`
### Description
Get a value from the power board state object. Numerical values (in SI units without metric prefix) are returned.

### Signature
```lisp
(pwrg
    name
)
```


### Arguments
- `name` \[string\]
    - `"valid"`
    - `"time"`
    - `"mode"`
    - `"duty"`
    - `"OCP_max_duty"`
    - `"RPM"`
    - `"voltage"`
    - `"current"`
    - `"hw_enable"`
    - `"sw_enable"`
    - `"enabled"`
    - `"emergency"`

### Return value
\[any\]
## `pwrs`
### Description
Set parameters on the power board. Variables are added for the constants from `power_board_mode_t` enum. They are prefixed with `pwr_`, e.g. `pwr_start`.

### Signature
```lisp
(pwrs
    name
    value
    ...
)
```


### Arguments
- `name` \[string\]
    - `"duty"`
    - `"mode"`
    - `"REL"`
    - `"sw_enable"`
- `value` \[any\]: Desired value (expected type depends on 'name')

### Return value
\[nil\]
## `lcdc`
Category: LCD

### Description
Set LCD cursor position to specified column.

### Signature
```lisp
(lcdc
    column
)
```


### Arguments
- `column` \[number\]

### Return value
\[nil\]
## `lcdw`
Category: LCD

### Description
Write LCD buffer to specified line (row).

### Signature
```lisp
(lcdw
    row
)
```


### Arguments
- `row` \[number\]

### Return value
\[nil\]
## `lcds`
Category: LCD

### Description
Print a string to the LCD buffer.

### Signature
```lisp
(lcds
    str
)
```


### Arguments
- `str` \[string\]

### Return value
\[nil\]
## `lcdn`
Category: LCD

### Description
Print a number to the LCD buffer.

### Signature
```lisp
(lcdn
    number
    align
    precision
)
```


### Arguments
- `number` \[number\]: number to print
- `align` \[number\]: (integer) TODO
- `precision` \[any\]: (integer) number of decimal places (TODO verify)

### Return value
\[nil\]
## `lcdb`
Category: LCD

### Description
Get or set LCD backlight status.

### Signature
```lisp
(lcdb
    [state]
)
```


### Arguments
- `state` \[bool\]: Desired backlight status

### Return value
\[bool\]: LCD backlight status
## `stats`
### Description
Get a value from the stats struct.

### Signature
```lisp
(stats
    name
)
```


### Arguments
- `name` \[string\]
    - `"energy"`

### Return value
\[number\]: value
## `ctrlg`
### Description
Get a value from the control subsystem.

### Signature
```lisp
(ctrlg
    name
)
```


### Arguments
- `name` \[string\]
    - `"strategy"`
    - `"contactor"`

### Return value
\[any\]
## `ctrls`
### Description
Set a value in the control subsystem.

### Signature
```lisp
(ctrls
    name
    value
)
```


### Arguments
- `name` \[string\]
    - `"strategy"`
    - `"contactor"`
- `value` \[any\]: Desired value (expected type depends on 'name')

### Return value
\[nil\]
## `out`
### Description
Get or set state of digital outputs (LEDs and relays) on the control board.

### Signature
```lisp
(out
    out_number
    [state]
)
```


### Arguments
- `out_number` \[string\]
    - `"LED_BLUE"`
    - `"LED_RED"`
    - `"PUMP"`
    - `"REL2"`
- `state` \[bool\]: desired state

### Return value
\[bool\]: state of the digital output
## `ds18`
### Description
Read temperature from DS18B20 sensors.

### Signature
```lisp
(ds18
    sensor_number
)
```


### Arguments
- `sensor_number` \[number\]

### Return value
\[number\]: temperature in degrees C
## `ctrl`
Category: callbacks

### Description
If set up for `LISP` control strategy, the device will call this function
each time a new state object is received.

The `ctrl` function is empty by default, the user is expected to
overwrite this function with their own implementation.

### Signature
```lisp
(ctrl
)
```


### Arguments

### Return value
\[nil\]
## `disp`
Category: callbacks

### Description
The `disp` function is executed every 0.5 s. It is meant to be used to
control the display.

The `disp` function is empty by default, the user is expected to
overwrite this function with their own implementation.

### Signature
```lisp
(disp
)
```


### Arguments

### Return value
\[nil\]
## `btn1_short`
Category: callbacks

### Description
The functions `btn1_short`, `btn1_long`, `btn2_short`, and `btn2_long` are called when the respective button is short / long pressed. (To be more precise, they are only called once the button is released.)

### Signature
```lisp
(btn1_short
)
```


### Arguments

### Return value
\[nil\]
## `btn1_long`
Category: callbacks

### Description
The functions `btn1_short`, `btn1_long`, `btn2_short`, and `btn2_long` are called when the respective button is short / long pressed. (To be more precise, they are only called once the button is released.)

### Signature
```lisp
(btn1_long
)
```


### Arguments

### Return value
\[nil\]
## `btn2_short`
Category: callbacks

### Description
The functions `btn1_short`, `btn1_long`, `btn2_short`, and `btn2_long` are called when the respective button is short / long pressed. (To be more precise, they are only called once the button is released.)

### Signature
```lisp
(btn2_short
)
```


### Arguments

### Return value
\[nil\]
## `btn2_long`
Category: callbacks

### Description
The functions `btn1_short`, `btn1_long`, `btn2_short`, and `btn2_long` are called when the respective button is short / long pressed. (To be more precise, they are only called once the button is released.)

### Signature
```lisp
(btn2_long
)
```


### Arguments

### Return value
\[nil\]
