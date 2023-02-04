(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: new_geo_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Neděle, 05 února 2023 at 19:26)

(This preprocessor is the default preprocessor used by FlatCAM.)
(It is made to work with MACH3 compatible motion controllers.)

(TOOL DIAMETER: 1.0 mm)
(Feedrate_XY: 80.0 mm/min)
(Feedrate_Z: 25.0 mm/min)
(Feedrate rapids 1500.0 mm/min)

(Z_Cut: -1.7 mm)
(Z_Move: 2.0 mm)
(Z Start: None mm)
(Z End: 5.0 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Preprocessor Geometry: default)

(X range:   14.5000 ...   24.0000  mm)
(Y range:   13.2500 ...   60.5000  mm)

(Spindle Speed: 10000.0 RPM)
G21
G90
G94

G01 F80.00

M5
G00 Z15.0000
G00 X0.0000 Y0.0000
T1
M6    
(MSG, Change to Tool Dia = 1.0000)
M0
G00 Z15.0000

M03 S10000.0
G4 P1.0
G01 F80.00
G00 X24.0000 Y13.2500
G01 F25.00
G01 Z-1.7000
G01 F80.00
G01 X16.5000 Y13.2500
G01 X14.5000 Y14.5000
G01 X14.5000 Y18.2500
G01 X16.5000 Y19.7500
G01 X24.0000 Y19.7500
G00 Z2.0000
G00 X24.0000 Y33.5000
G01 F25.00
G01 Z-1.7000
G01 F80.00
G01 X16.5000 Y33.5000
G01 X14.5000 Y34.7500
G01 X14.5000 Y38.5000
G01 X16.5000 Y40.0000
G01 X24.0000 Y40.0000
G00 Z2.0000
G00 X24.0000 Y54.0000
G01 F25.00
G01 Z-1.7000
G01 F80.00
G01 X16.5000 Y54.0000
G01 X14.5000 Y55.2500
G01 X14.5000 Y59.0000
G01 X16.5000 Y60.5000
G01 X24.0000 Y60.5000
G00 Z2.0000
M05
G00 Z2.0000
G00 Z5.00


