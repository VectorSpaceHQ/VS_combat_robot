(Exported by FreeCAD)
(Post Processor: linuxcnc_post)
(Output Time:2024-10-28 11:08:53.538205)
(begin preamble)
G17 G54 G40 G49 G80 G90
G20
(begin operation: Fixture)
(machine units: in/min)
G54 
(finish operation: Fixture)
(begin operation: 30_Drill004)
(machine units: in/min)
(30_Drill004) 
M5
M6 T32 
G43 H32 
M3 S8000 
(finish operation: 30_Drill004)
(begin operation: Drilling006)
(machine units: in/min)
(Drilling006) 
(Begin Drilling) 
G0 Z0.1969 
G90 
G98 
G0 X1.1250 Y-0.1875 
G83 X1.1250 Y-0.1875 Z-0.3600 F10.0000 Q0.0967 R0.1181 
G0 X4.1250 Y-0.1875 
G83 X4.1250 Y-0.1875 Z-0.3600 F10.0000 Q0.0967 R0.1181 
G80 
G0 Z0.1181 
G0 Z0.1969 
(finish operation: Drilling006)
(begin postamble)
M05
G17 G54 G90 G80 G40
M2
