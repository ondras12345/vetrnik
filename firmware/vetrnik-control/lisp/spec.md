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

## `lim`
### Description
Limit a number to a range.

Equivalent lisp implementation:
```lisp
(= lim (fn (x l h)
  (if
    (< x l) l
    (< h x) h
    x
  )
))
```

### Signature
```lisp
(lim
    x
    low
    high
)
```


### Arguments
- `x` \[number\]: value to be processed
- `low` \[number\]: lower bound for the value's range
- `high` \[number\]: upper bound for the value's range

### Return value
\[number\]
## `pow`
### Description
compute x raised to the power of y

### Signature
```lisp
(pow
    x
    y
)
```


### Arguments
- `x` \[number\]
- `y` \[number\]

### Return value
\[number\]
## `pwrg`
### Description
Get a value from the power board state object. Numerical values (in SI units without metric prefix) are returned.

### Signature
```lisp
(pwrg
    name
    [relay_nr]
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
- `relay_nr` \[number\]: relay number (starting from 1); only used if name == "REL"

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
    - `"vwind"`

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
\[number\]: temperature in degrees C. nil is returned if sensor is broken.
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

### Example
```lisp
;;; A simple "blink" control function:
(= dl 50) ; off state duty
(= dh 200) ; on state duty
(= p 5) ; period in seconds
(= pt 0) ; prev time
(= ctrl (fn ()
  ; t is used for "true", do not overwrite - named ti instead
  (let ti (pwrg "time"))
  ; max time is 65535 (see power report()), so no problem with rem
  (if (and (is (rem ti p) 0) (not (is ti pt)))
    (do
      (= pt ti)
      (pwrs "duty" (if (<= (pwrg "duty") dl) dh dl))
    )
  )
))

;;; Simple RPM threshold-based control:
(= ctrl (fn ()
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
