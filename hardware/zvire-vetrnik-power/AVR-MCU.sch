EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 6500 2300 2    50   Output ~ 0
PWM1
Text HLabel 6500 2400 2    50   Output ~ 0
PWM2
Text HLabel 6500 4300 2    50   Output ~ 0
~SD
Text HLabel 6500 3700 2    50   Output ~ 0
TX
Text HLabel 6500 3600 2    50   Input ~ 0
RX
$Comp
L MCU_Microchip_ATmega:ATmega8-16PU U12
U 1 1 631AE863
P 5500 3300
F 0 "U12" H 6100 4650 50  0000 C CNN
F 1 "ATmega8-16PU" H 6350 4550 50  0000 C CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket" H 5500 3300 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2486-8-bit-avr-microcontroller-atmega8_l_datasheet.pdf" H 5500 3300 50  0001 C CNN
	1    5500 3300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR092
U 1 1 631AF8FE
P 5500 4900
F 0 "#PWR092" H 5500 4700 50  0001 C CNN
F 1 "GNDPWR" H 5504 4746 50  0000 C CNN
F 2 "" H 5500 4850 50  0001 C CNN
F 3 "" H 5500 4850 50  0001 C CNN
	1    5500 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 4900 5500 4800
Wire Wire Line
	5600 4800 5500 4800
Wire Wire Line
	5600 4700 5600 4800
Connection ~ 5500 4800
Wire Wire Line
	5500 4800 5500 4700
$Comp
L power:+5P #PWR091
U 1 1 631B1183
P 5500 1750
F 0 "#PWR091" H 5500 1600 50  0001 C CNN
F 1 "+5P" H 5515 1923 50  0000 C CNN
F 2 "" H 5500 1750 50  0001 C CNN
F 3 "" H 5500 1750 50  0001 C CNN
	1    5500 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 1900 5500 1800
Wire Wire Line
	5600 1900 5600 1800
Wire Wire Line
	5600 1800 5500 1800
Connection ~ 5500 1800
Wire Wire Line
	5500 1800 5500 1750
$Comp
L Device:C C24
U 1 1 631B25F0
P 4000 3750
F 0 "C24" H 4115 3796 50  0000 L CNN
F 1 "1u / 25V" H 4115 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 4038 3600 50  0001 C CNN
F 3 "~" H 4000 3750 50  0001 C CNN
	1    4000 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:C C21
U 1 1 631B2843
P 3500 3750
F 0 "C21" H 3615 3796 50  0000 L CNN
F 1 "100n" H 3615 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3538 3600 50  0001 C CNN
F 3 "~" H 3500 3750 50  0001 C CNN
	1    3500 3750
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR083
U 1 1 631B369C
P 3750 4150
F 0 "#PWR083" H 3750 3950 50  0001 C CNN
F 1 "GNDPWR" H 3754 3996 50  0000 C CNN
F 2 "" H 3750 4100 50  0001 C CNN
F 3 "" H 3750 4100 50  0001 C CNN
	1    3750 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 3900 4000 4050
Wire Wire Line
	4000 4050 3750 4050
Wire Wire Line
	3500 4050 3500 3900
Wire Wire Line
	3750 4150 3750 4050
Connection ~ 3750 4050
Wire Wire Line
	3750 4050 3500 4050
$Comp
L power:+5P #PWR082
U 1 1 631B4E0C
P 3750 3400
F 0 "#PWR082" H 3750 3250 50  0001 C CNN
F 1 "+5P" H 3765 3573 50  0000 C CNN
F 2 "" H 3750 3400 50  0001 C CNN
F 3 "" H 3750 3400 50  0001 C CNN
	1    3750 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 3600 3500 3500
Wire Wire Line
	3500 3500 3750 3500
Wire Wire Line
	3750 3500 3750 3400
Wire Wire Line
	3750 3500 4000 3500
Wire Wire Line
	4000 3500 4000 3600
Connection ~ 3750 3500
$Comp
L Device:Crystal Y1
U 1 1 631BBA39
P 4000 2500
F 0 "Y1" V 3909 2631 50  0000 L CNN
F 1 "16MHz" V 4000 2631 50  0000 L CNN
F 2 "Crystal:Crystal_HC49-U_Vertical" H 4000 2500 50  0001 C CNN
F 3 "https://www.tme.eu/Document/b7481bb67ab73fd5481ce42749f8e0fb/HC-49S.pdf" H 4000 2500 50  0001 C CNN
F 4 "https://www.tme.eu/cz/details/16m-49s-sr/krystalove-rezonatory-tht/sr-passives/" V 4000 2500 50  0001 C CNN "TME"
F 5 "16M-49S-SR" V 4091 2631 50  0000 L CNN "TME Symbol"
	1    4000 2500
	0    1    1    0   
$EndComp
$Comp
L Device:C C22
U 1 1 631BC7D6
P 3750 2250
F 0 "C22" V 3498 2250 50  0000 C CNN
F 1 "22p" V 3589 2250 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3788 2100 50  0001 C CNN
F 3 "https://www.tme.eu/Document/6a5ad7ac81542ad65ce1ca82bcd21ffa/vjw1bcbascomseries.pdf" H 3750 2250 50  0001 C CNN
F 4 "VJ1206A220JXACW1BC" V 3750 2250 50  0001 C CNN "TME Symbol"
F 5 "C0G" V 3750 2250 50  0001 C CNN "Material"
F 6 "https://www.tme.eu/cz/details/vj1206a220jxacw1bc/kondenzatory-mlcc-smd-1206/vishay/" V 3750 2250 50  0001 C CNN "TME"
	1    3750 2250
	0    1    1    0   
$EndComp
$Comp
L Device:C C23
U 1 1 631BE573
P 3750 2750
F 0 "C23" V 3498 2750 50  0000 C CNN
F 1 "22p" V 3589 2750 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3788 2600 50  0001 C CNN
F 3 "https://www.tme.eu/Document/6a5ad7ac81542ad65ce1ca82bcd21ffa/vjw1bcbascomseries.pdf" H 3750 2750 50  0001 C CNN
F 4 "VJ1206A220JXACW1BC" V 3750 2750 50  0001 C CNN "TME Symbol"
F 5 "C0G" V 3750 2750 50  0001 C CNN "Material"
F 6 "https://www.tme.eu/cz/details/vj1206a220jxacw1bc/kondenzatory-mlcc-smd-1206/vishay/" V 3750 2750 50  0001 C CNN "TME"
	1    3750 2750
	0    1    1    0   
$EndComp
$Comp
L power:GNDPWR #PWR081
U 1 1 631C0B0F
P 3500 2850
F 0 "#PWR081" H 3500 2650 50  0001 C CNN
F 1 "GNDPWR" H 3504 2696 50  0000 C CNN
F 2 "" H 3500 2800 50  0001 C CNN
F 3 "" H 3500 2800 50  0001 C CNN
	1    3500 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2850 3500 2750
Wire Wire Line
	3500 2750 3600 2750
Wire Wire Line
	3500 2750 3500 2250
Wire Wire Line
	3500 2250 3600 2250
Connection ~ 3500 2750
Wire Wire Line
	3900 2250 4000 2250
Wire Wire Line
	4000 2250 4000 2350
Wire Wire Line
	3900 2750 4000 2750
Wire Wire Line
	4000 2750 4000 2650
Wire Wire Line
	4900 2600 4800 2600
Wire Wire Line
	4800 2600 4800 2750
Wire Wire Line
	4800 2750 4000 2750
Connection ~ 4000 2750
Wire Wire Line
	4900 2400 4800 2400
Wire Wire Line
	4800 2400 4800 2250
Wire Wire Line
	4800 2250 4000 2250
Connection ~ 4000 2250
$Comp
L Connector:AVR-ISP-6 J15
U 1 1 631CE60E
P 1600 1550
F 0 "J15" H 1271 1646 50  0000 R CNN
F 1 "AVR-ISP-6" H 1271 1555 50  0000 R CNN
F 2 "Connector_IDC:IDC-Header_2x03_P2.54mm_Vertical" V 1350 1600 50  0001 C CNN
F 3 " ~" H 325 1000 50  0001 C CNN
	1    1600 1550
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR075
U 1 1 631D2089
P 1500 2050
F 0 "#PWR075" H 1500 1850 50  0001 C CNN
F 1 "GNDPWR" H 1504 1896 50  0000 C CNN
F 2 "" H 1500 2000 50  0001 C CNN
F 3 "" H 1500 2000 50  0001 C CNN
	1    1500 2050
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR074
U 1 1 631D3906
P 1500 950
F 0 "#PWR074" H 1500 800 50  0001 C CNN
F 1 "+5P" H 1515 1123 50  0000 C CNN
F 2 "" H 1500 950 50  0001 C CNN
F 3 "" H 1500 950 50  0001 C CNN
	1    1500 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 950  1500 1050
Wire Wire Line
	1500 2050 1500 1950
Text Label 2500 1350 2    50   ~ 0
MISO
Text Label 2500 1450 2    50   ~ 0
MOSI
Text Label 2500 1550 2    50   ~ 0
SCK
Text Label 2500 1650 2    50   ~ 0
~RST
Wire Wire Line
	2500 1650 2000 1650
Wire Wire Line
	2000 1550 2500 1550
Wire Wire Line
	2500 1450 2000 1450
Wire Wire Line
	2000 1350 2500 1350
Text Label 6500 2500 2    50   ~ 0
MOSI
Text Label 6500 2600 2    50   ~ 0
MISO
Text Label 6500 2700 2    50   ~ 0
SCK
Text Label 4500 2200 0    50   ~ 0
~RST
Wire Wire Line
	4500 2200 4900 2200
Wire Wire Line
	6500 2500 6100 2500
Wire Wire Line
	6100 2600 6500 2600
Wire Wire Line
	6500 2700 6100 2700
$Comp
L Device:C C19
U 1 1 631E3879
P 2000 3750
F 0 "C19" H 2115 3796 50  0000 L CNN
F 1 "100n" H 2115 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 2038 3600 50  0001 C CNN
F 3 "~" H 2000 3750 50  0001 C CNN
	1    2000 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R41
U 1 1 631E5643
P 2000 3250
F 0 "R41" H 2070 3296 50  0000 L CNN
F 1 "10k" H 2070 3205 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 1930 3250 50  0001 C CNN
F 3 "~" H 2000 3250 50  0001 C CNN
	1    2000 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR077
U 1 1 631E5A9E
P 2000 4000
F 0 "#PWR077" H 2000 3800 50  0001 C CNN
F 1 "GNDPWR" H 2004 3846 50  0000 C CNN
F 2 "" H 2000 3950 50  0001 C CNN
F 3 "" H 2000 3950 50  0001 C CNN
	1    2000 4000
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR076
U 1 1 631E74E0
P 2000 3000
F 0 "#PWR076" H 2000 2850 50  0001 C CNN
F 1 "+5P" H 2015 3173 50  0000 C CNN
F 2 "" H 2000 3000 50  0001 C CNN
F 3 "" H 2000 3000 50  0001 C CNN
	1    2000 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 3000 2000 3100
Wire Wire Line
	2000 4000 2000 3900
Wire Wire Line
	2000 3600 2000 3500
Text Label 2500 3500 2    50   ~ 0
~RST
Wire Wire Line
	2500 3500 2000 3500
Connection ~ 2000 3500
Wire Wire Line
	2000 3500 2000 3400
$Comp
L Device:R R39
U 1 1 631EBA35
P 1650 3500
F 0 "R39" V 1443 3500 50  0000 C CNN
F 1 "330" V 1534 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 1580 3500 50  0001 C CNN
F 3 "~" H 1650 3500 50  0001 C CNN
	1    1650 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	1800 3500 2000 3500
$Comp
L Switch:SW_Push SW1
U 1 1 631ED49B
P 1350 3750
F 0 "SW1" V 1396 3702 50  0000 R CNN
F 1 "AVR RESET" V 1305 3702 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H4.3mm" H 1350 3950 50  0001 C CNN
F 3 "~" H 1350 3950 50  0001 C CNN
	1    1350 3750
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDPWR #PWR073
U 1 1 631EDD9D
P 1350 4000
F 0 "#PWR073" H 1350 3800 50  0001 C CNN
F 1 "GNDPWR" H 1354 3846 50  0000 C CNN
F 2 "" H 1350 3950 50  0001 C CNN
F 3 "" H 1350 3950 50  0001 C CNN
	1    1350 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 4000 1350 3950
Wire Wire Line
	1350 3550 1350 3500
Wire Wire Line
	1350 3500 1500 3500
Wire Wire Line
	6500 3700 6100 3700
Wire Wire Line
	6100 3600 6500 3600
Wire Wire Line
	6100 2300 6500 2300
Wire Wire Line
	6500 2400 6100 2400
Text Notes 7250 3500 0    50   ~ 0
TODO which timer/counter to measure freq / RPM?
$Comp
L power:GNDPWR #PWR084
U 1 1 6320F781
P 4000 1400
F 0 "#PWR084" H 4000 1200 50  0001 C CNN
F 1 "GNDPWR" H 4004 1246 50  0000 C CNN
F 2 "" H 4000 1350 50  0001 C CNN
F 3 "" H 4000 1350 50  0001 C CNN
	1    4000 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR090
U 1 1 63210706
P 5150 950
F 0 "#PWR090" H 5150 800 50  0001 C CNN
F 1 "+5P" H 5165 1123 50  0000 C CNN
F 2 "" H 5150 950 50  0001 C CNN
F 3 "" H 5150 950 50  0001 C CNN
	1    5150 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 1000 4400 1000
Wire Wire Line
	4000 1400 4000 1300
$Comp
L power:+12P #PWR079
U 1 1 63213193
P 3500 950
F 0 "#PWR079" H 3500 800 50  0001 C CNN
F 1 "+12P" H 3515 1123 50  0000 C CNN
F 2 "" H 3500 950 50  0001 C CNN
F 3 "" H 3500 950 50  0001 C CNN
	1    3500 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 950  3500 1000
Wire Wire Line
	3500 1000 3700 1000
$Comp
L Regulator_Linear:L78L05_TO92 U11
U 1 1 6321630F
P 4000 1000
F 0 "U11" H 4000 1242 50  0000 C CNN
F 1 "L78L05_TO92" H 4000 1151 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92L_HandSolder" H 4000 1225 50  0001 C CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/15/55/e5/aa/23/5b/43/fd/CD00000446.pdf/files/CD00000446.pdf/jcr:content/translations/en.CD00000446.pdf" H 4000 950 50  0001 C CNN
	1    4000 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C25
U 1 1 63216DE2
P 4400 1200
F 0 "C25" H 4515 1246 50  0000 L CNN
F 1 "100n" H 4515 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 4438 1050 50  0001 C CNN
F 3 "~" H 4400 1200 50  0001 C CNN
	1    4400 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C20
U 1 1 632171AF
P 3500 1200
F 0 "C20" H 3615 1246 50  0000 L CNN
F 1 "330n" H 3615 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3538 1050 50  0001 C CNN
F 3 "~" H 3500 1200 50  0001 C CNN
	1    3500 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 1000 3500 1050
Connection ~ 3500 1000
Wire Wire Line
	4400 1050 4400 1000
$Comp
L power:GNDPWR #PWR080
U 1 1 6321CD6A
P 3500 1400
F 0 "#PWR080" H 3500 1200 50  0001 C CNN
F 1 "GNDPWR" H 3504 1246 50  0000 C CNN
F 2 "" H 3500 1350 50  0001 C CNN
F 3 "" H 3500 1350 50  0001 C CNN
	1    3500 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR086
U 1 1 6321D179
P 4400 1400
F 0 "#PWR086" H 4400 1200 50  0001 C CNN
F 1 "GNDPWR" H 4404 1246 50  0000 C CNN
F 2 "" H 4400 1350 50  0001 C CNN
F 3 "" H 4400 1350 50  0001 C CNN
	1    4400 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 1400 3500 1350
Wire Wire Line
	4400 1350 4400 1400
$Comp
L Device:C C26
U 1 1 6323779A
P 4500 3250
F 0 "C26" H 4615 3296 50  0000 L CNN
F 1 "100n" H 4615 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 4538 3100 50  0001 C CNN
F 3 "~" H 4500 3250 50  0001 C CNN
	1    4500 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3100 4500 2800
Wire Wire Line
	4500 2800 4900 2800
$Comp
L power:GNDPWR #PWR087
U 1 1 63239444
P 4500 3500
F 0 "#PWR087" H 4500 3300 50  0001 C CNN
F 1 "GNDPWR" H 4504 3346 50  0000 C CNN
F 2 "" H 4500 3450 50  0001 C CNN
F 3 "" H 4500 3450 50  0001 C CNN
	1    4500 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3500 4500 3400
Text Notes 6900 3850 0    50   ~ 0
PD2, PD3 interrupts
Text Notes 6300 4050 0    50   ~ 0
PD4, PD5 timer inputs
Wire Wire Line
	6500 4300 6100 4300
$Comp
L power:GNDPWR #PWR089
U 1 1 6325E09D
P 4500 6000
F 0 "#PWR089" H 4500 5800 50  0001 C CNN
F 1 "GNDPWR" H 4504 5846 50  0000 C CNN
F 2 "" H 4500 5950 50  0001 C CNN
F 3 "" H 4500 5950 50  0001 C CNN
	1    4500 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5800 4500 6000
Text Label 4850 5300 2    50   ~ 0
RPM
Text Label 6750 3800 2    50   ~ 0
RPM
$Comp
L Diode:1N4148W D5
U 1 1 632715A2
P 2200 5500
F 0 "D5" V 2154 5580 50  0000 L CNN
F 1 "1N4148W" V 2245 5580 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-123" H 2200 5325 50  0001 C CNN
F 3 "https://www.vishay.com/docs/85748/1n4148w.pdf" H 2200 5500 50  0001 C CNN
	1    2200 5500
	0    1    1    0   
$EndComp
Wire Wire Line
	2200 5350 2200 5250
Wire Wire Line
	2200 5250 2600 5250
Wire Wire Line
	2600 5250 2600 5400
Wire Wire Line
	2600 5400 2700 5400
Wire Wire Line
	2200 5650 2200 5750
Wire Wire Line
	2200 5750 2600 5750
Wire Wire Line
	2600 5750 2600 5600
Wire Wire Line
	2600 5600 2700 5600
Wire Wire Line
	1900 5250 2200 5250
Connection ~ 2200 5250
Connection ~ 2200 5750
Text HLabel 8150 3950 2    50   Output ~ 0
SHORT
Wire Wire Line
	6750 3800 6200 3800
Wire Wire Line
	6100 4000 6200 4000
Wire Wire Line
	6200 4000 6200 3800
Connection ~ 6200 3800
Wire Wire Line
	6200 3800 6100 3800
$Comp
L Device:R R47
U 1 1 632A3E40
P 10000 1500
F 0 "R47" V 9793 1500 50  0000 C CNN
F 1 "1k" V 9884 1500 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 9930 1500 50  0001 C CNN
F 3 "~" H 10000 1500 50  0001 C CNN
	1    10000 1500
	0    1    1    0   
$EndComp
Text Label 6750 2900 2    50   ~ 0
ADC0_VSENSE
Wire Wire Line
	6750 2900 6100 2900
$Comp
L Diode:BAV99 D6
U 1 1 632A8DCB
P 9500 1500
F 0 "D6" V 9700 1600 50  0000 L CNN
F 1 "BAV99" V 9550 1600 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9500 1000 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAV99_SER.pdf" H 9500 1500 50  0001 C CNN
	1    9500 1500
	0    -1   -1   0   
$EndComp
Text Label 10800 1500 2    50   ~ 0
ADC0_VSENSE
Wire Wire Line
	10800 1500 10200 1500
Wire Wire Line
	9850 1500 9700 1500
$Comp
L power:+5P #PWR097
U 1 1 632AF55B
P 9500 1150
F 0 "#PWR097" H 9500 1000 50  0001 C CNN
F 1 "+5P" H 9515 1323 50  0000 C CNN
F 2 "" H 9500 1150 50  0001 C CNN
F 3 "" H 9500 1150 50  0001 C CNN
	1    9500 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 1150 9500 1200
Wire Wire Line
	9500 1900 9500 1800
Wire Wire Line
	8750 1500 9700 1500
Connection ~ 9700 1500
$Comp
L Device:C C30
U 1 1 632C11B7
P 10200 1700
F 0 "C30" H 10315 1746 50  0000 L CNN
F 1 "10n" H 10315 1655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 10238 1550 50  0001 C CNN
F 3 "~" H 10200 1700 50  0001 C CNN
	1    10200 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0105
U 1 1 632C1A6A
P 10200 1900
F 0 "#PWR0105" H 10200 1700 50  0001 C CNN
F 1 "GNDPWR" H 10204 1746 50  0000 C CNN
F 2 "" H 10200 1850 50  0001 C CNN
F 3 "" H 10200 1850 50  0001 C CNN
	1    10200 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 1900 10200 1850
Wire Wire Line
	10200 1550 10200 1500
Connection ~ 10200 1500
Wire Wire Line
	10200 1500 10150 1500
$Comp
L Connector_Generic:Conn_01x02 J14
U 1 1 6337899D
P 800 5450
F 0 "J14" H 800 5700 50  0000 C CNN
F 1 "AC frequency sense" H 700 5600 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 800 5450 50  0001 C CNN
F 3 "~" H 800 5450 50  0001 C CNN
	1    800  5450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1000 5550 1350 5550
Wire Wire Line
	1350 5550 1350 5750
Wire Wire Line
	1000 5450 1350 5450
Wire Wire Line
	1350 5450 1350 5250
Wire Wire Line
	1350 5250 1600 5250
$Comp
L Device:R R40
U 1 1 632762DE
P 1750 5250
F 0 "R40" V 1543 5250 50  0000 C CNN
F 1 "22k 5W" V 1634 5250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0922_L20.0mm_D9.0mm_P30.48mm_Horizontal" V 1680 5250 50  0001 C CNN
F 3 "~" H 1750 5250 50  0001 C CNN
	1    1750 5250
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:BC847 Q9
U 1 1 6338C31C
P 4400 5600
F 0 "Q9" H 4591 5646 50  0000 L CNN
F 1 "BC847" H 4591 5555 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4600 5525 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/Infineon-BC847SERIES_BC848SERIES_BC849SERIES_BC850SERIES-DS-v01_01-en.pdf?fileId=db3a304314dca389011541d4630a1657" H 4400 5600 50  0001 L CNN
	1    4400 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5300 4500 5400
Wire Wire Line
	4850 5300 4500 5300
Connection ~ 4500 5300
$Comp
L Device:R R44
U 1 1 633A08DA
P 4500 5050
F 0 "R44" H 4570 5096 50  0000 L CNN
F 1 "10k" H 4570 5005 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 4430 5050 50  0001 C CNN
F 3 "~" H 4500 5050 50  0001 C CNN
	1    4500 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5200 4500 5300
$Comp
L power:+5P #PWR088
U 1 1 633A5378
P 4500 4850
F 0 "#PWR088" H 4500 4700 50  0001 C CNN
F 1 "+5P" H 4515 5023 50  0000 C CNN
F 2 "" H 4500 4850 50  0001 C CNN
F 3 "" H 4500 4850 50  0001 C CNN
	1    4500 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4850 4500 4900
Text Notes 9850 6100 0    50   ~ 0
ACS712 takes 13 mA\nfrom isolated supply!
$Comp
L Device:R R42
U 1 1 6309454A
P 3500 5600
F 0 "R42" V 3293 5600 50  0000 C CNN
F 1 "10k" V 3384 5600 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3430 5600 50  0001 C CNN
F 3 "~" H 3500 5600 50  0001 C CNN
	1    3500 5600
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 5600 3300 5600
$Comp
L power:+5P #PWR078
U 1 1 63098230
P 3350 5350
F 0 "#PWR078" H 3350 5200 50  0001 C CNN
F 1 "+5P" H 3365 5523 50  0000 C CNN
F 2 "" H 3350 5350 50  0001 C CNN
F 3 "" H 3350 5350 50  0001 C CNN
	1    3350 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R43
U 1 1 63099CF9
P 4000 5800
F 0 "R43" H 4070 5846 50  0000 L CNN
F 1 "100k" H 4070 5755 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3930 5800 50  0001 C CNN
F 3 "~" H 4000 5800 50  0001 C CNN
	1    4000 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 5600 4000 5600
Wire Wire Line
	4000 5650 4000 5600
Connection ~ 4000 5600
Wire Wire Line
	4000 5600 4200 5600
$Comp
L power:GNDPWR #PWR085
U 1 1 630AFFFF
P 4000 6000
F 0 "#PWR085" H 4000 5800 50  0001 C CNN
F 1 "GNDPWR" H 4004 5846 50  0000 C CNN
F 2 "" H 4000 5950 50  0001 C CNN
F 3 "" H 4000 5950 50  0001 C CNN
	1    4000 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 6000 4000 5950
Wire Wire Line
	1350 5750 2200 5750
Text HLabel 8750 1500 0    50   Input ~ 0
VSENSE
$Comp
L Device:R R45
U 1 1 635255BD
P 7500 1750
F 0 "R45" H 7570 1796 50  0000 L CNN
F 1 "4k7" H 7570 1705 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 7430 1750 50  0001 C CNN
F 3 "~" H 7500 1750 50  0001 C CNN
	1    7500 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:Thermistor_NTC TH1
U 1 1 635264D1
P 7500 1250
F 0 "TH1" H 7597 1296 50  0000 L CNN
F 1 "TT7-10KC3-11" H 7597 1205 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 7500 1300 50  0001 C CNN
F 3 "https://www.tme.eu/Document/32a31570f1c819f9b3730213e5eca259/TT7-10KC3-11.pdf" H 7500 1300 50  0001 C CNN
F 4 "https://www.tme.eu/cz/details/tt7-10kc3-11/merici-termistory-ntc-tht/tewa-temperature-sensors/" H 7500 1250 50  0001 C CNN "TME"
	1    7500 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR098
U 1 1 632B2DB4
P 9500 1900
F 0 "#PWR098" H 9500 1700 50  0001 C CNN
F 1 "GNDPWR" H 9504 1746 50  0000 C CNN
F 2 "" H 9500 1850 50  0001 C CNN
F 3 "" H 9500 1850 50  0001 C CNN
	1    9500 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR095
U 1 1 6352B762
P 7500 2000
F 0 "#PWR095" H 7500 1800 50  0001 C CNN
F 1 "GNDPWR" H 7504 1846 50  0000 C CNN
F 2 "" H 7500 1950 50  0001 C CNN
F 3 "" H 7500 1950 50  0001 C CNN
	1    7500 2000
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR094
U 1 1 6352BA2E
P 7500 1000
F 0 "#PWR094" H 7500 850 50  0001 C CNN
F 1 "+5P" H 7515 1173 50  0000 C CNN
F 2 "" H 7500 1000 50  0001 C CNN
F 3 "" H 7500 1000 50  0001 C CNN
	1    7500 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 1000 7500 1100
Wire Wire Line
	7500 1400 7500 1500
Text Label 6750 3200 2    50   ~ 0
ADC3_NTC1
Wire Wire Line
	6750 3200 6100 3200
Text Label 8000 1500 2    50   ~ 0
ADC3_NTC1
Wire Wire Line
	8000 1500 7500 1500
Connection ~ 7500 1500
Wire Wire Line
	7500 1500 7500 1600
Wire Wire Line
	7500 1900 7500 2000
Text Label 6750 3000 2    50   ~ 0
ADC1_ACS712
Wire Wire Line
	6750 3000 6100 3000
$Comp
L Connector_Generic:Conn_01x03 J17
U 1 1 63559FEE
P 10250 5500
F 0 "J17" H 10330 5542 50  0000 L CNN
F 1 "ACS712" H 10330 5451 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-03A_1x03_P2.54mm_Vertical" H 10250 5500 50  0001 C CNN
F 3 "~" H 10250 5500 50  0001 C CNN
	1    10250 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0104
U 1 1 6355C3D7
P 9950 5700
F 0 "#PWR0104" H 9950 5500 50  0001 C CNN
F 1 "GNDPWR" H 9954 5546 50  0000 C CNN
F 2 "" H 9950 5650 50  0001 C CNN
F 3 "" H 9950 5650 50  0001 C CNN
	1    9950 5700
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR0103
U 1 1 6355E3B7
P 9950 5300
F 0 "#PWR0103" H 9950 5150 50  0001 C CNN
F 1 "+5P" H 9965 5473 50  0000 C CNN
F 2 "" H 9950 5300 50  0001 C CNN
F 3 "" H 9950 5300 50  0001 C CNN
	1    9950 5300
	1    0    0    -1  
$EndComp
Text Label 8850 5500 0    50   ~ 0
ADC1_ACS712
Wire Wire Line
	8850 5500 9500 5500
Wire Wire Line
	9950 5700 9950 5600
Wire Wire Line
	9950 5600 10050 5600
Wire Wire Line
	9950 5300 9950 5400
Wire Wire Line
	9950 5400 10050 5400
Text HLabel 6500 4100 2    50   Output ~ 0
FAN
Wire Wire Line
	6100 4100 6500 4100
$Comp
L Device:C C32
U 1 1 636403F1
P 10800 5500
F 0 "C32" H 10915 5546 50  0000 L CNN
F 1 "100n" H 10915 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 10838 5350 50  0001 C CNN
F 3 "~" H 10800 5500 50  0001 C CNN
	1    10800 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0109
U 1 1 6364156A
P 10800 5700
F 0 "#PWR0109" H 10800 5500 50  0001 C CNN
F 1 "GNDPWR" H 10804 5546 50  0000 C CNN
F 2 "" H 10800 5650 50  0001 C CNN
F 3 "" H 10800 5650 50  0001 C CNN
	1    10800 5700
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR0108
U 1 1 636418A2
P 10800 5300
F 0 "#PWR0108" H 10800 5150 50  0001 C CNN
F 1 "+5P" H 10815 5473 50  0000 C CNN
F 2 "" H 10800 5300 50  0001 C CNN
F 3 "" H 10800 5300 50  0001 C CNN
	1    10800 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10800 5300 10800 5350
Wire Wire Line
	10800 5700 10800 5650
Text Label 6400 4200 2    50   ~ 0
~SHORT
$Comp
L Transistor_BJT:BC846 Q10
U 1 1 6320C454
P 7900 4200
F 0 "Q10" H 8091 4246 50  0000 L CNN
F 1 "BC846" H 8091 4155 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8100 4125 50  0001 L CIN
F 3 "https://assets.nexperia.com/documents/data-sheet/BC846_SER.pdf" H 7900 4200 50  0001 L CNN
	1    7900 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R46
U 1 1 6320CD54
P 7500 4200
F 0 "R46" V 7293 4200 50  0000 C CNN
F 1 "47k" V 7384 4200 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 7430 4200 50  0001 C CNN
F 3 "~" H 7500 4200 50  0001 C CNN
	1    7500 4200
	0    1    1    0   
$EndComp
Wire Wire Line
	6100 4200 7350 4200
Wire Wire Line
	7700 4200 7650 4200
Wire Wire Line
	8000 4000 8000 3950
Wire Wire Line
	8000 3950 8150 3950
$Comp
L power:GNDPWR #PWR096
U 1 1 6321F25D
P 8000 4500
F 0 "#PWR096" H 8000 4300 50  0001 C CNN
F 1 "GNDPWR" H 8004 4346 50  0000 C CNN
F 2 "" H 8000 4450 50  0001 C CNN
F 3 "" H 8000 4450 50  0001 C CNN
	1    8000 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4500 8000 4400
Text HLabel 6500 3900 2    50   Input ~ 0
~ENABLE
$Comp
L Device:C C27
U 1 1 632B7FFC
P 7100 1750
F 0 "C27" H 7215 1796 50  0000 L CNN
F 1 "100n" H 7215 1705 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 7138 1600 50  0001 C CNN
F 3 "~" H 7100 1750 50  0001 C CNN
	1    7100 1750
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR093
U 1 1 632B8D9B
P 7100 2000
F 0 "#PWR093" H 7100 1800 50  0001 C CNN
F 1 "GNDPWR" H 7104 1846 50  0000 C CNN
F 2 "" H 7100 1950 50  0001 C CNN
F 3 "" H 7100 1950 50  0001 C CNN
	1    7100 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 2000 7100 1900
Wire Wire Line
	7100 1600 7100 1500
Wire Wire Line
	7100 1500 7500 1500
$Comp
L Device:C C29
U 1 1 632D2CDC
P 9500 5700
F 0 "C29" H 9615 5746 50  0000 L CNN
F 1 "10n" H 9615 5655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 9538 5550 50  0001 C CNN
F 3 "~" H 9500 5700 50  0001 C CNN
	1    9500 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0100
U 1 1 632D2CE2
P 9500 5900
F 0 "#PWR0100" H 9500 5700 50  0001 C CNN
F 1 "GNDPWR" H 9504 5746 50  0000 C CNN
F 2 "" H 9500 5850 50  0001 C CNN
F 3 "" H 9500 5850 50  0001 C CNN
	1    9500 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 5900 9500 5850
Wire Wire Line
	9500 5550 9500 5500
Connection ~ 9500 5500
Wire Wire Line
	9500 5500 10050 5500
Wire Wire Line
	6500 3900 6100 3900
Text HLabel 6500 2200 2    50   Output ~ 0
REL1
Text HLabel 6500 3400 2    50   Output ~ 0
REL2
Text HLabel 6500 3300 2    50   Output ~ 0
REL3
Wire Wire Line
	6500 3400 6100 3400
Wire Wire Line
	6100 3300 6500 3300
Wire Wire Line
	6500 2200 6100 2200
Wire Wire Line
	6750 3100 6100 3100
$Comp
L Connector_Generic:Conn_01x03 J16
U 1 1 636C37A8
P 10250 4500
F 0 "J16" H 10330 4542 50  0000 L CNN
F 1 "ACS712_2" H 10330 4451 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-03A_1x03_P2.54mm_Vertical" H 10250 4500 50  0001 C CNN
F 3 "~" H 10250 4500 50  0001 C CNN
	1    10250 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0102
U 1 1 636C37AE
P 9950 4700
F 0 "#PWR0102" H 9950 4500 50  0001 C CNN
F 1 "GNDPWR" H 9954 4546 50  0000 C CNN
F 2 "" H 9950 4650 50  0001 C CNN
F 3 "" H 9950 4650 50  0001 C CNN
	1    9950 4700
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR0101
U 1 1 636C37B4
P 9950 4300
F 0 "#PWR0101" H 9950 4150 50  0001 C CNN
F 1 "+5P" H 9965 4473 50  0000 C CNN
F 2 "" H 9950 4300 50  0001 C CNN
F 3 "" H 9950 4300 50  0001 C CNN
	1    9950 4300
	1    0    0    -1  
$EndComp
Text Label 8850 4500 0    50   ~ 0
ADC2_ACS712_2
Wire Wire Line
	8850 4500 9500 4500
Wire Wire Line
	9950 4700 9950 4600
Wire Wire Line
	9950 4600 10050 4600
Wire Wire Line
	9950 4300 9950 4400
Wire Wire Line
	9950 4400 10050 4400
$Comp
L Device:C C28
U 1 1 636C37C0
P 9500 4700
F 0 "C28" H 9615 4746 50  0000 L CNN
F 1 "10n" H 9615 4655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 9538 4550 50  0001 C CNN
F 3 "~" H 9500 4700 50  0001 C CNN
	1    9500 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR099
U 1 1 636C37C6
P 9500 4900
F 0 "#PWR099" H 9500 4700 50  0001 C CNN
F 1 "GNDPWR" H 9504 4746 50  0000 C CNN
F 2 "" H 9500 4850 50  0001 C CNN
F 3 "" H 9500 4850 50  0001 C CNN
	1    9500 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 4900 9500 4850
Wire Wire Line
	9500 4550 9500 4500
Connection ~ 9500 4500
Wire Wire Line
	9500 4500 10050 4500
$Comp
L Device:C C31
U 1 1 636DA208
P 10800 4500
F 0 "C31" H 10915 4546 50  0000 L CNN
F 1 "100n" H 10915 4455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 10838 4350 50  0001 C CNN
F 3 "~" H 10800 4500 50  0001 C CNN
	1    10800 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0107
U 1 1 636DA20E
P 10800 4700
F 0 "#PWR0107" H 10800 4500 50  0001 C CNN
F 1 "GNDPWR" H 10804 4546 50  0000 C CNN
F 2 "" H 10800 4650 50  0001 C CNN
F 3 "" H 10800 4650 50  0001 C CNN
	1    10800 4700
	1    0    0    -1  
$EndComp
$Comp
L power:+5P #PWR0106
U 1 1 636DA214
P 10800 4300
F 0 "#PWR0106" H 10800 4150 50  0001 C CNN
F 1 "+5P" H 10815 4473 50  0000 C CNN
F 2 "" H 10800 4300 50  0001 C CNN
F 3 "" H 10800 4300 50  0001 C CNN
	1    10800 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10800 4300 10800 4350
Wire Wire Line
	10800 4700 10800 4650
Text Notes 8100 6450 0    50   ~ 0
It's probably not a good idea to use both ACS712 at the same time.\nCheck current consumption from isolated supply before doing that.
$Comp
L Connector:TestPoint_2Pole TP2
U 1 1 636F4F9E
P 4750 1000
F 0 "TP2" H 4750 1195 50  0000 C CNN
F 1 "TestPoint_2Pole" H 4750 1104 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4750 1000 50  0001 C CNN
F 3 "~" H 4750 1000 50  0001 C CNN
	1    4750 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 1000 4400 1000
Connection ~ 4400 1000
Wire Wire Line
	4950 1000 5150 1000
Wire Wire Line
	5150 1000 5150 950 
Wire Wire Line
	3350 5350 3350 5400
Wire Wire Line
	3350 5400 3300 5400
Text HLabel 6500 2700 2    50   Output ~ 0
REL4
$Comp
L power:PWR_FLAG #FLG06
U 1 1 63A41F41
P 5200 1000
F 0 "#FLG06" H 5200 1075 50  0001 C CNN
F 1 "PWR_FLAG" V 5200 1128 50  0000 L CNN
F 2 "" H 5200 1000 50  0001 C CNN
F 3 "~" H 5200 1000 50  0001 C CNN
	1    5200 1000
	0    1    1    0   
$EndComp
Wire Wire Line
	5200 1000 5150 1000
Connection ~ 5150 1000
$Comp
L Device:Jumper JP1
U 1 1 63960712
P 5500 1500
F 0 "JP1" H 5500 1764 50  0000 C CNN
F 1 "Jumper" H 5500 1673 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" H 5500 1500 50  0001 C CNN
F 3 "~" H 5500 1500 50  0001 C CNN
	1    5500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 1500 5150 1500
Wire Wire Line
	5150 1500 5150 1800
Wire Wire Line
	5150 1800 5500 1800
Wire Wire Line
	5800 1500 5850 1500
Wire Wire Line
	5850 1500 5850 1800
Wire Wire Line
	5850 1800 5600 1800
Connection ~ 5600 1800
$Comp
L Isolator:LTV-817 U10
U 1 1 630E6CD7
P 3000 5500
F 0 "U10" H 3000 5825 50  0000 C CNN
F 1 "LTV-817" H 3000 5734 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 2800 5300 50  0001 L CIN
F 3 "http://www.us.liteon.com/downloads/LTV-817-827-847.PDF" H 3000 5400 50  0001 L CNN
	1    3000 5500
	1    0    0    -1  
$EndComp
Text Label 6750 3100 2    50   ~ 0
ADC2_ACS712_2
$EndSCHEMATC
