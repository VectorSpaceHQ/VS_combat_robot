(Exported by FreeCAD)
(Post Processor: linuxcnc_post)
(Output Time:2024-10-28 11:44:19.151286)
(begin preamble)
G17 G54 G40 G49 G80 G90
G20
(begin operation: Fixture)
(machine units: in/min)
G54 
(finish operation: Fixture)
(begin operation: TC: 3-16_Endmill)
(machine units: in/min)
(TC: 3-16_Endmill) 
M5
M6 T34 
G43 H34 
M3 S10000 
(finish operation: TC: 3-16_Endmill)
(begin operation: Pocket_Shape002)
(machine units: in/min)
(Pocket_Shape002) 
G0 Z0.1969 
G0 X1.8164 Y-0.4261 
G0 Z0.1181 
G1 X1.8164 Y-0.4261 Z-0.1200 F10.0000 
G2 X1.8178 Y-0.4474 Z-0.1200 I-0.0114 J-0.0114 F20.0000 
G2 X1.7951 Y-0.4503 Z-0.1200 I-0.0128 J0.0099 F20.0000 
G2 X1.7922 Y-0.4276 Z-0.1200 I0.0099 J0.0128 F20.0000 
G2 X1.8164 Y-0.4261 Z-0.1200 I0.0128 J-0.0099 F20.0000 
G0 Z0.1969 
G0 Z0.1969 
G0 X1.8164 Y-0.4261 
G0 X1.8053 Y-1.0463 
G0 X1.8053 Y-1.0463 Z0.1181 
G1 X1.8053 Y-1.0463 Z-0.1200 F10.0000 
G2 X1.8178 Y-1.0724 Z-0.1200 I-0.0003 J-0.0162 F20.0000 
G2 X1.7951 Y-1.0753 Z-0.1200 I-0.0128 J0.0099 F20.0000 
G2 X1.7922 Y-1.0526 Z-0.1200 I0.0099 J0.0128 F20.0000 
G2 X1.8053 Y-1.0463 Z-0.1200 I0.0128 J-0.0099 F20.0000 
G0 Z0.1969 
G0 Z0.1969 
(finish operation: Pocket_Shape002)
(begin operation: TC: 30_Drill005)
(machine units: in/min)
(TC: 30_Drill005) 
M5
M6 T32 
G43 H32 
M3 S8000 
(finish operation: TC: 30_Drill005)
(begin operation: Drilling008)
(machine units: in/min)
(Drilling008) 
(Begin Drilling) 
G0 Z0.1969 
G90 
G98 
G0 X1.8050 Y-0.4375 
G83 X1.8050 Y-0.4375 Z-0.4200 F10.0000 Q0.0967 R0.1181 
G0 X1.8050 Y-1.0625 
G83 X1.8050 Y-1.0625 Z-0.4200 F10.0000 Q0.0967 R0.1181 
G80 
G0 Z0.1181 
G0 Z0.1969 
(finish operation: Drilling008)
(begin postamble)
M05
G17 G54 G90 G80 G40
M2
