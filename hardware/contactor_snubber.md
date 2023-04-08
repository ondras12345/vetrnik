# Contactor RC snubber
Ethernet module would reset when contactor was operating (both on and off).
With a x100 oscilloscope probe, I was able to detect 1.5kV peak from one relay
contact to PE when the contactor was switching off. With the RC snubber in
parallel with relay contacts, the contactor would never switch off after being
operated once due to current flowing through the capacitor. RC snubber in
parallel with contactor's coil would reduce the peak voltage to <700V.
However, it wouldn't prevent the Ethernet module from resetting. I had to add
a 10nF capacitor to the module's RSTn pin for that.

There is some info about RC snubber desing in `contactor_snubber.m`.
However, I decided to use the generic laskakit.cz snubber (100 nF, 220 ohm and
a MOV). I'll likely need to put a fuse in series to protect from MOV failure.
