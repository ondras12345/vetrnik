# Vetrnik hardware
Block diagram:
```
            3 phase
+---------+ contactor +-----------+ DC +--------+ +-----------+ 1-phase +---------+
| Turbine +-+__/ __---+ rectifier +-+--+ ACS770 +-+ H-bridge  +---------+ heater  +
+---------+ |  ^      +-----------+ |  +----+---+ +-----------+ AC      +---------+
            |  |                    |       |          ^
            |  |             voltage|       |          |
            |  |                    |       | current  | duty
            |  |                    v       |          |
  frequency |  |      +-------------+-+     |          |
  (RPM)     |  +------+ vetrnik-power +<----+          |
            +-------->+               +----------------+
                      +-------+-------+
                              ^
                              | UART
                              v
                      +-------+-----------+
                      |  vetrnik-control  |
                      |                   +<----------> Ethernet
                      +-------------------+
```


The hardware consists of several PCBs in a custom-made enclosure. Most PCBs
are single-sided and designed to be milled out on a CNC.
- vetrnik-power
  - Controlled by an ATmega8 MCU.
  - Controls a FET H-bridge mounted on a heatsink.
  - Has isolated power supply, its GNDPWR is at wind turbine output potential.
  - Receives commands from vetrnik-control via optically isolated UART.
  - No remote firmware updates, no bootloader.
  - This PCB also contains analog comparator-based emergency stop circuitry
    that handles over-voltage condition even if the MCU is not working.
- vetrnik-control
  - Controlled by STM32F401.
  - Handles smart features (Ethernet, telnet, MQTT).
  - Implements LISP-like interpreted language that can be used to send
    commands to vetrnik-power. MPPT controller is implemented on this device
    in LISP.
  - Supports OTA firmware updates.
- vetrnik-current-sensor
  - A double-sided PCB with heavy copper. It passes all the current from the
    bridge rectifier through an ACS770-050U hall-effect current sensor.
- vetrnik-current-sensor-opamp
  - This PCB contains some additional signal-conditioning circuitry to improve
    current sensing precision and filter out noise.
- vetrnik-power-supply
  - A power supply board that converts 12 V from MeanWell power supply to
    various voltage levels needed by the device.
    The 3.3V voltage regulator is switch mode, 5V regulator is linear.
- vetrnik-relays
  - This is just a simple board with 3 relays that can be used to switch 230V
    devices like the coil of the contactor used to disconnect the wind turbine
    output, and a circulator pump.


## Wind turbine
- iSta breeze i-2000 Windsafe
- three phase
- 2 kW nominal power, 2.2 kW max
- 6 pole pairs --> 10 RPM / Hz


## Load
- 4x 600 W resistive heating element
- https://www.growzone.cz/48V-1200W-topna-patrona-G-6-4-palce-38x-197mm-d8275.htm
