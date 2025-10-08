(Exported by FreeCAD)
(Post Processor: linuxcnc_post)
(Output Time:2024-10-27 10:41:43.726165)
(begin preamble)
G17 G54 G40 G49 G80 G90
G20
(begin operation: Fixture)
(machine units: in/min)
G54 
(finish operation: Fixture)
(begin operation: TC: Default Tool)
(machine units: in/min)
(TC: Default Tool) 
M5
M6 T1 
G43 H1 
M3 S10000 
(finish operation: TC: Default Tool)
(begin operation: Drilling)
(machine units: in/min)
(Drilling) 
(Begin Drilling) 
G0 Z0.1969 
G90 
G98 
G0 X3.1250 Y0.1875 
G83 X3.1250 Y0.1875 Z-0.3500 F10.0000 Q0.1476 R0.1181 
G0 X6.1250 Y0.1875 
G83 X6.1250 Y0.1875 Z-0.3500 F10.0000 Q0.1476 R0.1181 
G80 
G0 Z0.1181 
G0 Z0.1969 
(finish operation: Drilling)
(begin postamble)
M05
G17 G54 G90 G80 G40
M2
