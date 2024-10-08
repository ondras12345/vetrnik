(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: zvire-vetrnik-power.drl_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Čtvrtek, 01 září 2022 at 17:32)

(This preprocessor is used with a motion controller loaded with GRBL firmware.)
(It is configured to be compatible with almost any version of GRBL firmware.)


(TOOLS DIAMETER: )
(Tool: 1 -> Dia: 0.75)
(Tool: 2 -> Dia: 0.8)
(Tool: 3 -> Dia: 1.0)
(Tool: 4 -> Dia: 1.075)
(Tool: 5 -> Dia: 1.1)
(Tool: 6 -> Dia: 1.19)
(Tool: 7 -> Dia: 1.2)
(Tool: 8 -> Dia: 1.3)
(Tool: 9 -> Dia: 1.9)
(Tool: 10 -> Dia: 2.0)
(Tool: 11 -> Dia: 3.2)

(FEEDRATE Z: )
(Tool: 1 -> Feedrate: 100.0)
(Tool: 2 -> Feedrate: 100.0)
(Tool: 3 -> Feedrate: 100.0)
(Tool: 4 -> Feedrate: 100.0)
(Tool: 5 -> Feedrate: 100.0)
(Tool: 6 -> Feedrate: 100.0)
(Tool: 7 -> Feedrate: 100.0)
(Tool: 8 -> Feedrate: 100.0)
(Tool: 9 -> Feedrate: 100.0)
(Tool: 10 -> Feedrate: 100.0)
(Tool: 11 -> Feedrate: 100.0)

(FEEDRATE RAPIDS: )
(Tool: 1 -> Feedrate Rapids: 1500)
(Tool: 2 -> Feedrate Rapids: 1500)
(Tool: 3 -> Feedrate Rapids: 1500)
(Tool: 4 -> Feedrate Rapids: 1500)
(Tool: 5 -> Feedrate Rapids: 1500)
(Tool: 6 -> Feedrate Rapids: 1500)
(Tool: 7 -> Feedrate Rapids: 1500)
(Tool: 8 -> Feedrate Rapids: 1500)
(Tool: 9 -> Feedrate Rapids: 1500)
(Tool: 10 -> Feedrate Rapids: 1500)
(Tool: 11 -> Feedrate Rapids: 1500)

(Z_CUT: )
(Tool: 1 -> Z_Cut: -1.7)
(Tool: 2 -> Z_Cut: -1.7)
(Tool: 3 -> Z_Cut: -1.7)
(Tool: 4 -> Z_Cut: -1.7)
(Tool: 5 -> Z_Cut: -1.7)
(Tool: 6 -> Z_Cut: -1.7)
(Tool: 7 -> Z_Cut: -1.7)
(Tool: 8 -> Z_Cut: -1.7)
(Tool: 9 -> Z_Cut: -1.7)
(Tool: 10 -> Z_Cut: -1.7)
(Tool: 11 -> Z_Cut: -1.7)

(Tools Offset: )
(Tool: 3 -> Offset Z: 0.0)
(Tool: 4 -> Offset Z: 0.0)
(Tool: 5 -> Offset Z: 0.0)
(Tool: 6 -> Offset Z: 0.0)
(Tool: 7 -> Offset Z: 0.0)

(Z_MOVE: )
(Tool: 1 -> Z_Move: 4.0)
(Tool: 2 -> Z_Move: 4.0)
(Tool: 3 -> Z_Move: 4.0)
(Tool: 4 -> Z_Move: 4.0)
(Tool: 5 -> Z_Move: 4.0)
(Tool: 6 -> Z_Move: 4.0)
(Tool: 7 -> Z_Move: 4.0)
(Tool: 8 -> Z_Move: 4.0)
(Tool: 9 -> Z_Move: 4.0)
(Tool: 10 -> Z_Move: 4.0)
(Tool: 11 -> Z_Move: 4.0)

(Z Toolchange: 5.0 mm)
(X,Y Toolchange: 0.0000, 0.0000 mm)
(Z Start: None mm)
(Z End: 5.0 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Steps per circle: 64)
(Preprocessor Excellon: GRBL_11_no_M6)

(X range:    2.2100 ...  122.2500  mm)
(Y range:  -96.8500 ...   -3.4100  mm)

(Spindle Speed: 10000 RPM)
G21
G90
G17
G94


G01 F100.00

M5             
G00 Z5.0000
G00 X0.0000 Y0.0000                
T3
(MSG, Change to Tool Dia = 1.0000 ||| Total drills for tool T3 = 69)
M0
G00 Z5.0000
        
G01 F100.00
M03 S10000
G4 P1.0
G00 X34.3700 Y-16.5100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X35.5600 Y-7.6200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X38.1000 Y-7.6200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X48.2600 Y-7.6200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X48.2600 Y-10.1600
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X48.2600 Y-12.7000
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X48.2600 Y-15.2400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X45.7200 Y-20.3200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X48.2600 Y-20.3200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-49.5300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-46.9900
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-41.9100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-39.3700
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-36.8300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-33.0200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-30.4800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X74.9300 Y-27.9400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X77.4700 Y-20.3200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X77.4700 Y-17.7800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X77.4700 Y-15.2400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X80.0100 Y-15.2400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X80.0100 Y-17.7800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X80.0100 Y-20.3200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-12.7000
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-15.2400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-22.8600
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-25.4000
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-27.9400
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-30.4800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-33.0200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-35.5600
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-41.9100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-44.4500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-48.2600
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-50.8000
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X113.0300 Y-50.8000
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-54.6100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-57.1500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-59.6900
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-67.3100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-69.8500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-72.3900
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X120.6500 Y-74.9300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X113.0300 Y-81.2800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X95.2500 Y-77.4700
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X95.2500 Y-87.6300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X95.2500 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X92.7100 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X88.9000 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X86.3600 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X76.2000 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X73.6600 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X69.8500 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X67.3100 Y-95.2500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X76.2000 Y-87.6300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X76.2000 Y-77.4700
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X57.1500 Y-58.4200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X57.1500 Y-69.8500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X51.8300 Y-81.2800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X36.8300 Y-81.2800
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X34.6500 Y-69.8500
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X34.6500 Y-58.4200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X43.1800 Y-49.5300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X43.1800 Y-45.0300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X49.6800 Y-49.5300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X49.6800 Y-45.0300
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X45.7200 Y-33.0200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X43.1800 Y-33.0200
G01 Z-1.7000
G01 Z0
G00 Z4.0000
G00 X39.3700 Y-16.5100
G01 Z-1.7000
G01 Z0
G00 Z4.0000
M05
G00 Z5.00


