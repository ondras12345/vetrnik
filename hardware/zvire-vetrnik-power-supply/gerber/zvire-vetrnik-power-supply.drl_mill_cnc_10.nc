(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: zvire-vetrnik-power-supply.drl_mill_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Sobota, 04 února 2023 at 20:16)

(This preprocessor is the default preprocessor used by FlatCAM.)
(It is made to work with MACH3 compatible motion controllers.)

(TOOL DIAMETER: 1.0 mm)
(Feedrate_XY: 60.0 mm/min)
(Feedrate_Z: 25.0 mm/min)
(Feedrate rapids 1500.0 mm/min)

(Z_Cut: -1.6 mm)
(Z_Move: 2.0 mm)
(Z Start: None mm)
(Z End: 5.0 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Preprocessor Geometry: default)

(X range:    2.7100 ...   77.3000  mm)
(Y range:    2.7100 ...   18.8800  mm)

(Spindle Speed: 10000.0 RPM)
G21
G90
G94

G01 F60.00

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
G01 F60.00
G00 X4.9100 Y3.8100
G01 F25.00
G01 Z-1.6000
G01 F60.00
G01 X4.9047 Y3.7022
G01 X4.8889 Y3.5954
G01 X4.8626 Y3.4907
G01 X4.8263 Y3.3890
G01 X4.7801 Y3.2915
G01 X4.7246 Y3.1989
G01 X4.6603 Y3.1122
G01 X4.5878 Y3.0322
G01 X4.5078 Y2.9597
G01 X4.4211 Y2.8954
G01 X4.3285 Y2.8399
G01 X4.2310 Y2.7937
G01 X4.1293 Y2.7574
G01 X4.0246 Y2.7311
G01 X3.9178 Y2.7153
G01 X3.8100 Y2.7100
G01 X3.7022 Y2.7153
G01 X3.5954 Y2.7311
G01 X3.4907 Y2.7574
G01 X3.3890 Y2.7937
G01 X3.2915 Y2.8399
G01 X3.1989 Y2.8954
G01 X3.1122 Y2.9597
G01 X3.0322 Y3.0322
G01 X2.9597 Y3.1122
G01 X2.8954 Y3.1989
G01 X2.8399 Y3.2915
G01 X2.7937 Y3.3890
G01 X2.7574 Y3.4907
G01 X2.7311 Y3.5954
G01 X2.7153 Y3.7022
G01 X2.7100 Y3.8100
G01 X2.7153 Y3.9178
G01 X2.7311 Y4.0246
G01 X2.7574 Y4.1293
G01 X2.7937 Y4.2310
G01 X2.8399 Y4.3285
G01 X2.8954 Y4.4211
G01 X2.9597 Y4.5078
G01 X3.0322 Y4.5878
G01 X3.1122 Y4.6603
G01 X3.1989 Y4.7246
G01 X3.2915 Y4.7801
G01 X3.3890 Y4.8263
G01 X3.4907 Y4.8626
G01 X3.5954 Y4.8889
G01 X3.7022 Y4.9047
G01 X3.8100 Y4.9100
G01 X3.9178 Y4.9047
G01 X4.0246 Y4.8889
G01 X4.1293 Y4.8626
G01 X4.2310 Y4.8263
G01 X4.3285 Y4.7801
G01 X4.4211 Y4.7246
G01 X4.5078 Y4.6603
G01 X4.5878 Y4.5878
G01 X4.6603 Y4.5078
G01 X4.7246 Y4.4211
G01 X4.7801 Y4.3285
G01 X4.8263 Y4.2310
G01 X4.8626 Y4.1293
G01 X4.8889 Y4.0246
G01 X4.9047 Y3.9178
G01 X4.9100 Y3.8100
G00 Z2.0000
G00 X4.9100 Y17.7800
G01 F25.00
G01 Z-1.6000
G01 F60.00
G01 X4.9047 Y17.6722
G01 X4.8889 Y17.5654
G01 X4.8626 Y17.4607
G01 X4.8263 Y17.3590
G01 X4.7801 Y17.2615
G01 X4.7246 Y17.1689
G01 X4.6603 Y17.0822
G01 X4.5878 Y17.0022
G01 X4.5078 Y16.9297
G01 X4.4211 Y16.8654
G01 X4.3285 Y16.8099
G01 X4.2310 Y16.7637
G01 X4.1293 Y16.7274
G01 X4.0246 Y16.7011
G01 X3.9178 Y16.6853
G01 X3.8100 Y16.6800
G01 X3.7022 Y16.6853
G01 X3.5954 Y16.7011
G01 X3.4907 Y16.7274
G01 X3.3890 Y16.7637
G01 X3.2915 Y16.8099
G01 X3.1989 Y16.8654
G01 X3.1122 Y16.9297
G01 X3.0322 Y17.0022
G01 X2.9597 Y17.0822
G01 X2.8954 Y17.1689
G01 X2.8399 Y17.2615
G01 X2.7937 Y17.3590
G01 X2.7574 Y17.4607
G01 X2.7311 Y17.5654
G01 X2.7153 Y17.6722
G01 X2.7100 Y17.7800
G01 X2.7153 Y17.8878
G01 X2.7311 Y17.9946
G01 X2.7574 Y18.0993
G01 X2.7937 Y18.2010
G01 X2.8399 Y18.2985
G01 X2.8954 Y18.3911
G01 X2.9597 Y18.4778
G01 X3.0322 Y18.5578
G01 X3.1122 Y18.6303
G01 X3.1989 Y18.6946
G01 X3.2915 Y18.7501
G01 X3.3890 Y18.7963
G01 X3.4907 Y18.8326
G01 X3.5954 Y18.8589
G01 X3.7022 Y18.8747
G01 X3.8100 Y18.8800
G01 X3.9178 Y18.8747
G01 X4.0246 Y18.8589
G01 X4.1293 Y18.8326
G01 X4.2310 Y18.7963
G01 X4.3285 Y18.7501
G01 X4.4211 Y18.6946
G01 X4.5078 Y18.6303
G01 X4.5878 Y18.5578
G01 X4.6603 Y18.4778
G01 X4.7246 Y18.3911
G01 X4.7801 Y18.2985
G01 X4.8263 Y18.2010
G01 X4.8626 Y18.0993
G01 X4.8889 Y17.9946
G01 X4.9047 Y17.8878
G01 X4.9100 Y17.7800
G00 Z2.0000
G00 X77.3000 Y3.8100
G01 F25.00
G01 Z-1.6000
G01 F60.00
G01 X77.2947 Y3.7022
G01 X77.2789 Y3.5954
G01 X77.2526 Y3.4907
G01 X77.2163 Y3.3890
G01 X77.1701 Y3.2915
G01 X77.1146 Y3.1989
G01 X77.0503 Y3.1122
G01 X76.9778 Y3.0322
G01 X76.8978 Y2.9597
G01 X76.8111 Y2.8954
G01 X76.7185 Y2.8399
G01 X76.6210 Y2.7937
G01 X76.5193 Y2.7574
G01 X76.4146 Y2.7311
G01 X76.3078 Y2.7153
G01 X76.2000 Y2.7100
G01 X76.0922 Y2.7153
G01 X75.9854 Y2.7311
G01 X75.8807 Y2.7574
G01 X75.7790 Y2.7937
G01 X75.6815 Y2.8399
G01 X75.5889 Y2.8954
G01 X75.5022 Y2.9597
G01 X75.4222 Y3.0322
G01 X75.3497 Y3.1122
G01 X75.2854 Y3.1989
G01 X75.2299 Y3.2915
G01 X75.1837 Y3.3890
G01 X75.1474 Y3.4907
G01 X75.1211 Y3.5954
G01 X75.1053 Y3.7022
G01 X75.1000 Y3.8100
G01 X75.1053 Y3.9178
G01 X75.1211 Y4.0246
G01 X75.1474 Y4.1293
G01 X75.1837 Y4.2310
G01 X75.2299 Y4.3285
G01 X75.2854 Y4.4211
G01 X75.3497 Y4.5078
G01 X75.4222 Y4.5878
G01 X75.5022 Y4.6603
G01 X75.5889 Y4.7246
G01 X75.6815 Y4.7801
G01 X75.7790 Y4.8263
G01 X75.8807 Y4.8626
G01 X75.9854 Y4.8889
G01 X76.0922 Y4.9047
G01 X76.2000 Y4.9100
G01 X76.3078 Y4.9047
G01 X76.4146 Y4.8889
G01 X76.5193 Y4.8626
G01 X76.6210 Y4.8263
G01 X76.7185 Y4.7801
G01 X76.8111 Y4.7246
G01 X76.8978 Y4.6603
G01 X76.9778 Y4.5878
G01 X77.0503 Y4.5078
G01 X77.1146 Y4.4211
G01 X77.1701 Y4.3285
G01 X77.2163 Y4.2310
G01 X77.2526 Y4.1293
G01 X77.2789 Y4.0246
G01 X77.2947 Y3.9178
G01 X77.3000 Y3.8100
G00 Z2.0000
M05
G00 Z2.0000
G00 Z5.00


