(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: zvire-vetrnik-current-sensor-opamp.drl_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Sobota, 04 února 2023 at 20:23)

(This preprocessor is the default preprocessor used by FlatCAM.)
(It is made to work with MACH3 compatible motion controllers.)


(TOOLS DIAMETER: )
(Tool: 1 -> Dia: 0.8)
(Tool: 2 -> Dia: 1.0)
(Tool: 3 -> Dia: 1.19)
(Tool: 4 -> Dia: 2.54)
(Tool: 5 -> Dia: 9.65)
(Tool: 6 -> Dia: 3.2)

(FEEDRATE Z: )
(Tool: 1 -> Feedrate: 120.0)
(Tool: 2 -> Feedrate: 120.0)
(Tool: 3 -> Feedrate: 120.0)
(Tool: 4 -> Feedrate: 120.0)
(Tool: 5 -> Feedrate: 120.0)
(Tool: 6 -> Feedrate: 120.0)

(FEEDRATE RAPIDS: )
(Tool: 1 -> Feedrate Rapids: 1500)
(Tool: 2 -> Feedrate Rapids: 1500)
(Tool: 3 -> Feedrate Rapids: 1500)
(Tool: 4 -> Feedrate Rapids: 1500)
(Tool: 5 -> Feedrate Rapids: 1500)
(Tool: 6 -> Feedrate Rapids: 1500)

(Z_CUT: )
(Tool: 1 -> Z_Cut: -1.65)
(Tool: 2 -> Z_Cut: -1.65)
(Tool: 3 -> Z_Cut: -1.65)
(Tool: 4 -> Z_Cut: -1.65)
(Tool: 5 -> Z_Cut: -1.65)
(Tool: 6 -> Z_Cut: -1.65)

(Tools Offset: )
(Tool: 1 -> Offset Z: 0.0)

(Z_MOVE: )
(Tool: 1 -> Z_Move: 2)
(Tool: 2 -> Z_Move: 2)
(Tool: 3 -> Z_Move: 2)
(Tool: 4 -> Z_Move: 2)
(Tool: 5 -> Z_Move: 2)
(Tool: 6 -> Z_Move: 2)

(Z Toolchange: 5.0 mm)
(X,Y Toolchange: 0.0000, 0.0000 mm)
(Z Start: None mm)
(Z End: 5.0 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Preprocessor Excellon: default)

(X range:    2.1950 ...   53.0850  mm)
(Y range:    2.1400 ...   21.9900  mm)

(Spindle Speed: 10000 RPM)
G21
G90
G94

G01 F120.00

M5
G00 Z5.0000
T1
G00 X0.0000 Y0.0000                
M6
(MSG, Change to Tool Dia = 0.8000 ||| Total drills for tool T1 = 17)
M0
G00 Z5.0000

G01 F120.00
M03 S10000
G4 P1.0
G00 X2.5950 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X15.2400 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X17.7800 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X20.3200 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X22.8600 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X25.4000 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X27.9400 Y2.5400
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X27.9400 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X27.9400 Y16.5100
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X25.4000 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X22.8600 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X20.3200 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X17.7800 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X15.2400 Y10.1600
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X11.4300 Y16.5100
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X11.4300 Y19.0500
G01 Z-1.6500
G01 Z0
G00 Z2.0000
G00 X11.4300 Y21.5900
G01 Z-1.6500
G01 Z0
G00 Z2.0000
M05
G00 Z5.00


