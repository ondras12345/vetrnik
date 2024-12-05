# Wind speed sensor modbus
- RS-FSJT-N01

## Initial testing
- 12 V power brown +, black -
- yellow (green) RS-485 A, blue: RS-485 B
- PL2303-485 USB converter (PL-2303HX + MAX485ESA)

```console
$ sudo apt install mbpoll

$ mbpoll -m rtu -a 1 -t 3 -r 1 -c 1 -l 500 -b 4800 /dev/ttyUSB0
# does not work

# debug:
$ mbpoll -v -m rtu -a 1 -t 3 -r 1 -c 1 -l 500 -b 4800 /dev/ttyUSB0
debug enabled
Set mode=rtu
iGetIntList(1)
Integer found: 1
iCount=1
Set function=3
Set start reference=1
Set number of values=1
Set poll rate=500
Set rtu baudrate=4800
Set device=/dev/ttyUSB0
mbpoll 1.0-0 - FieldTalk(tm) Modbus(R) Master Simulator
Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; type 'mbpoll -w' for details.

Opening /dev/ttyUSB0 at 4800 bauds (E, 8, 1)
Set response timeout to 1 sec, 0 us
Protocol configuration: Modbus RTU
Slave configuration...: address = [1]
                        start reference = 1, count = 1
Communication.........: /dev/ttyUSB0,       4800-8E1 
                        t/o 1.00 s, poll rate 500 ms
Data type.............: 16-bit register, input register table

-- Polling slave 1... Ctrl-C to stop)
[01][04][00][00][00][01][31][CA]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read input register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][04][00][00][00][01][31][CA]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read input register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][04][00][00][00][01][31][CA]
Waiting for a confirmation...
<01><00><00><00><FE>
ERROR CRC received 0xFE != CRC calculated 0x2000
Read input register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][04][00][00][00][01][31][CA]
Waiting for a confirmation...
<01><00><00><00><FE>
ERROR CRC received 0xFE != CRC calculated 0x2000
Read input register failed: Invalid CRC
```

According to the manual, only modbus function code 0x03 (read register data)
is supported.

```console
$ mbpoll -v -m rtu -a 1 -t 4 -r 1 -c 1 -l 500 -b 4800 /dev/ttyUSB0
debug enabled
Set mode=rtu
iGetIntList(1)
Integer found: 1
iCount=1
Set function=4
Set start reference=1
Set number of values=1
Set poll rate=500
Set rtu baudrate=4800
Set device=/dev/ttyUSB0
mbpoll 1.0-0 - FieldTalk(tm) Modbus(R) Master Simulator
Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; type 'mbpoll -w' for details.

Opening /dev/ttyUSB0 at 4800 bauds (E, 8, 1)
Set response timeout to 1 sec, 0 us
Protocol configuration: Modbus RTU
Slave configuration...: address = [1]
                        start reference = 1, count = 1
Communication.........: /dev/ttyUSB0,       4800-8E1 
                        t/o 1.00 s, poll rate 500 ms
Data type.............: 16-bit register, output (holding) register table

-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><B8><00>
ERROR CRC received 0xB800 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
^C--- /dev/ttyUSB0 poll statistics ---
3 frames transmitted, 0 received, 3 errors, 100.0% frame loss

everything was closed.
Have a nice day !
```

Now the query matches the manual, but there still is no response.

Connected grounds together.

I can see responses on the oscilloscope, but mbpoll still complains:
```console
$ mbpoll -v -m rtu -a 1 -t 4 -r 1 -c 1 -l 500 -b 4800 /dev/ttyUSB0
debug enabled
Set mode=rtu
iGetIntList(1)
Integer found: 1
iCount=1
Set function=4
Set start reference=1
Set number of values=1
Set poll rate=500
Set rtu baudrate=4800
Set device=/dev/ttyUSB0
mbpoll 1.0-0 - FieldTalk(tm) Modbus(R) Master Simulator
Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; type 'mbpoll -w' for details.

Opening /dev/ttyUSB0 at 4800 bauds (E, 8, 1)
Set response timeout to 1 sec, 0 us
Protocol configuration: Modbus RTU
Slave configuration...: address = [1]
                        start reference = 1, count = 1
Communication.........: /dev/ttyUSB0,       4800-8E1 
                        t/o 1.00 s, poll rate 500 ms
Data type.............: 16-bit register, output (holding) register table

-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
-- Polling slave 1... Ctrl-C to stop)
[01][03][00][00][00][01][84][0A]
Waiting for a confirmation...
<01><00><00><00><00>
ERROR CRC received 0x0 != CRC calculated 0x2000
Read output (holding) register failed: Invalid CRC
^C--- /dev/ttyUSB0 poll statistics ---
3 frames transmitted, 0 received, 3 errors, 100.0% frame loss

everything was closed.
Have a nice day !
```

Parity was not set properly.
```console
$ mbpoll -m rtu -a 1 -t 4 -r 1 -c 1 -l 500 -b 4800 -P none /dev/ttyUSB0
mbpoll 1.0-0 - FieldTalk(tm) Modbus(R) Master Simulator
Copyright © 2015-2019 Pascal JEAN, https://github.com/epsilonrt/mbpoll
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; type 'mbpoll -w' for details.

Protocol configuration: Modbus RTU
Slave configuration...: address = [1]
                        start reference = 1, count = 1
Communication.........: /dev/ttyUSB0,       4800-8N1 
                        t/o 1.00 s, poll rate 500 ms
Data type.............: 16-bit register, output (holding) register table

-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	11
-- Polling slave 1... Ctrl-C to stop)
[1]: 	14
-- Polling slave 1... Ctrl-C to stop)
[1]: 	14
-- Polling slave 1... Ctrl-C to stop)
[1]: 	11
-- Polling slave 1... Ctrl-C to stop)
[1]: 	11
-- Polling slave 1... Ctrl-C to stop)
[1]: 	8
-- Polling slave 1... Ctrl-C to stop)
[1]: 	8
-- Polling slave 1... Ctrl-C to stop)
[1]: 	6
-- Polling slave 1... Ctrl-C to stop)
[1]: 	6
-- Polling slave 1... Ctrl-C to stop)
[1]: 	4
-- Polling slave 1... Ctrl-C to stop)
[1]: 	4
-- Polling slave 1... Ctrl-C to stop)
[1]: 	2
-- Polling slave 1... Ctrl-C to stop)
[1]: 	2
-- Polling slave 1... Ctrl-C to stop)
[1]: 	2
-- Polling slave 1... Ctrl-C to stop)
[1]: 	1
-- Polling slave 1... Ctrl-C to stop)
[1]: 	1
-- Polling slave 1... Ctrl-C to stop)
[1]: 	1
-- Polling slave 1... Ctrl-C to stop)
[1]: 	1
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
-- Polling slave 1... Ctrl-C to stop)
[1]: 	0
^C--- /dev/ttyUSB0 poll statistics ---
29 frames transmitted, 29 received, 0 errors, 0.0% frame loss

everything was closed.
Have a nice day !
```
