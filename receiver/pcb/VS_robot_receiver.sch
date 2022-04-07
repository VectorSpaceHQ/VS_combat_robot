EESchema Schematic File Version 4
LIBS:VS_robot_receiver-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "RC Receiver"
Date "2021-02-21"
Rev ""
Comp "Vector Space "
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L arduino:arduino-pro-mini U1
U 1 1 5DCB3EBA
P 5200 3450
F 0 "U1" H 5200 4437 55  0000 C CNN
F 1 "arduino-pro-mini" H 5200 4338 55  0000 C CNN
F 2 "arduino-footprint:pro_mini" H 5100 4300 55  0001 C CNN
F 3 "" H 5100 4300 55  0001 C CNN
	1    5200 3450
	1    0    0    -1  
$EndComp
$Comp
L VS_robot_receiver-rescue:NRF24L01_Breakout-RF U2
U 1 1 5DCB4C96
P 7050 3800
F 0 "U2" H 7528 3779 50  0000 L CNN
F 1 "NRF24L01_Breakout" H 7528 3688 50  0000 L CNN
F 2 "RF_Module:nRF24L01_Breakout" H 7200 4400 50  0001 L CIN
F 3 "http://www.nordicsemi.com/eng/content/download/2730/34105/file/nRF24L01_Product_Specification_v2_0.pdf" H 7050 3700 50  0001 C CNN
	1    7050 3800
	1    0    0    -1  
$EndComp
$Comp
L VS_robot_receiver-rescue:Conn_02x04_Counter_Clockwise-Connector_Generic J2
U 1 1 5DCB6746
P 6100 5150
F 0 "J2" H 6150 5467 50  0000 C CNN
F 1 "outputs" H 6150 5376 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x04_P2.54mm_Vertical" H 6100 5150 50  0001 C CNN
F 3 "~" H 6100 5150 50  0001 C CNN
	1    6100 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 2300 7050 3200
$Comp
L VS_robot_receiver-rescue:+3.3V-power #PWR0102
U 1 1 5DCBB551
P 7050 2300
F 0 "#PWR0102" H 7050 2150 50  0001 C CNN
F 1 "+3.3V" H 7065 2473 50  0000 C CNN
F 2 "" H 7050 2300 50  0001 C CNN
F 3 "" H 7050 2300 50  0001 C CNN
	1    7050 2300
	1    0    0    -1  
$EndComp
Connection ~ 7050 2300
Wire Wire Line
	5550 3850 6550 3850
Wire Wire Line
	6550 3850 6550 4000
Wire Wire Line
	4700 2300 4700 2800
Wire Wire Line
	4700 2800 4850 2800
Wire Wire Line
	4700 2300 7050 2300
Wire Wire Line
	6550 3800 5650 3800
Wire Wire Line
	5650 3800 5650 3950
Wire Wire Line
	5650 3950 5550 3950
Wire Wire Line
	5550 4050 6100 4050
Wire Wire Line
	6100 4050 6100 3500
Wire Wire Line
	6100 3500 6550 3500
Wire Wire Line
	6550 3600 6150 3600
Wire Wire Line
	6150 3600 6150 4150
Wire Wire Line
	6150 4150 5550 4150
Wire Wire Line
	5550 4250 6250 4250
Wire Wire Line
	6250 4250 6250 3700
Wire Wire Line
	6250 3700 6550 3700
Wire Wire Line
	7050 4400 6800 4400
$Comp
L VS_robot_receiver-rescue:GND-power #PWR0103
U 1 1 5DCC0FB3
P 7050 4400
F 0 "#PWR0103" H 7050 4150 50  0001 C CNN
F 1 "GND" H 7055 4227 50  0000 C CNN
F 2 "" H 7050 4400 50  0001 C CNN
F 3 "" H 7050 4400 50  0001 C CNN
	1    7050 4400
	1    0    0    -1  
$EndComp
Connection ~ 7050 4400
Wire Wire Line
	5900 5050 4600 5050
Wire Wire Line
	6400 5050 6400 4400
Connection ~ 6400 4400
Wire Wire Line
	6400 4400 6150 4400
Wire Wire Line
	5900 5150 5850 5150
Wire Wire Line
	5850 5150 5850 4650
Wire Wire Line
	5850 3150 5550 3150
Wire Wire Line
	6450 3350 5550 3350
Wire Wire Line
	5550 3450 6500 3450
Wire Wire Line
	6500 5250 6400 5250
Wire Wire Line
	5550 3550 5700 3550
Connection ~ 6600 4400
Wire Wire Line
	6600 4400 6400 4400
Wire Wire Line
	5550 3650 5950 3650
Wire Wire Line
	5950 3650 5950 2800
$Comp
L VS_robot_receiver-rescue:LED-Device D1
U 1 1 5DCCD414
P 6400 2800
F 0 "D1" H 6393 2545 50  0000 C CNN
F 1 "comms" H 6393 2636 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 6400 2800 50  0001 C CNN
F 3 "~" H 6400 2800 50  0001 C CNN
	1    6400 2800
	-1   0    0    1   
$EndComp
Wire Wire Line
	6550 2800 6550 3200
Wire Wire Line
	6550 3200 6400 3200
Wire Wire Line
	6400 3200 6400 4400
NoConn ~ 5550 3750
NoConn ~ 5550 3000
NoConn ~ 5550 2900
NoConn ~ 5550 2800
NoConn ~ 5550 2700
NoConn ~ 4850 3000
NoConn ~ 4850 3100
NoConn ~ 4850 3300
NoConn ~ 4850 3400
NoConn ~ 6550 4100
Wire Wire Line
	4850 2700 4600 2700
Wire Wire Line
	4600 2700 4600 5050
$Comp
L VS_robot_receiver-rescue:+5V-power #PWR0101
U 1 1 5DCE7062
P 4600 5050
F 0 "#PWR0101" H 4600 4900 50  0001 C CNN
F 1 "+5V" H 4615 5223 50  0000 C CNN
F 2 "" H 4600 5050 50  0001 C CNN
F 3 "" H 4600 5050 50  0001 C CNN
	1    4600 5050
	-1   0    0    1   
$EndComp
Connection ~ 4600 5050
Wire Wire Line
	4850 4150 4850 4400
Wire Wire Line
	6600 5150 6600 4400
Wire Wire Line
	6400 5150 6600 5150
Wire Wire Line
	6450 5350 6400 5350
$Comp
L VS_robot_receiver-rescue:R_US-Device R1
U 1 1 5DCCCAFF
P 6100 2800
F 0 "R1" V 5895 2800 50  0000 C CNN
F 1 "600" V 5986 2800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 6140 2790 50  0001 C CNN
F 3 "~" H 6100 2800 50  0001 C CNN
	1    6100 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 3350 6450 5350
Wire Wire Line
	6500 3450 6500 5250
Wire Wire Line
	5750 3250 5550 3250
Wire Wire Line
	5750 5250 5750 3250
Wire Wire Line
	5700 3550 5700 5350
Wire Wire Line
	5900 5250 5750 5250
Wire Wire Line
	5700 5350 5900 5350
$Comp
L VS_robot_receiver-rescue:R_US-Device R5
U 1 1 5F0F2FCD
P 6600 5350
F 0 "R5" V 6805 5350 50  0000 C CNN
F 1 "10k" V 6714 5350 50  0000 C CNN
F 2 "" V 6640 5340 50  0001 C CNN
F 3 "~" H 6600 5350 50  0001 C CNN
	1    6600 5350
	0    -1   -1   0   
$EndComp
Connection ~ 6450 5350
$Comp
L VS_robot_receiver-rescue:R_US-Device R6
U 1 1 5F0F49AF
P 6650 5250
F 0 "R6" V 6855 5250 50  0000 C CNN
F 1 "10k" V 6764 5250 50  0000 C CNN
F 2 "" V 6690 5240 50  0001 C CNN
F 3 "~" H 6650 5250 50  0001 C CNN
	1    6650 5250
	0    -1   -1   0   
$EndComp
Connection ~ 6500 5250
$Comp
L VS_robot_receiver-rescue:R_US-Device R2
U 1 1 5F0F4C6D
P 5550 5350
F 0 "R2" V 5755 5350 50  0000 C CNN
F 1 "10k" V 5664 5350 50  0000 C CNN
F 2 "" V 5590 5340 50  0001 C CNN
F 3 "~" H 5550 5350 50  0001 C CNN
	1    5550 5350
	0    -1   -1   0   
$EndComp
Connection ~ 5700 5350
$Comp
L VS_robot_receiver-rescue:R_US-Device R3
U 1 1 5F0F5250
P 5600 5250
F 0 "R3" V 5805 5250 50  0000 C CNN
F 1 "10k" V 5714 5250 50  0000 C CNN
F 2 "" V 5640 5240 50  0001 C CNN
F 3 "~" H 5600 5250 50  0001 C CNN
	1    5600 5250
	0    -1   -1   0   
$EndComp
Connection ~ 5750 5250
Wire Wire Line
	6800 4400 6800 5250
Connection ~ 6800 4400
Wire Wire Line
	6800 4400 6600 4400
Wire Wire Line
	6800 5250 6800 5350
Wire Wire Line
	6800 5350 6750 5350
Connection ~ 6800 5250
Wire Wire Line
	5400 4400 5400 5250
Wire Wire Line
	5400 5250 5450 5250
Connection ~ 5400 4400
Wire Wire Line
	5400 4400 4850 4400
Wire Wire Line
	5400 5250 5400 5350
Connection ~ 5400 5250
$Comp
L VS_robot_receiver-rescue:R_US-Device R4
U 1 1 5F0CC2FB
P 6000 4650
F 0 "R4" V 6205 4650 50  0000 C CNN
F 1 "10k" V 6114 4650 50  0000 C CNN
F 2 "" V 6040 4640 50  0001 C CNN
F 3 "~" H 6000 4650 50  0001 C CNN
	1    6000 4650
	0    -1   -1   0   
$EndComp
Connection ~ 5850 4650
Wire Wire Line
	5850 4650 5850 3150
Wire Wire Line
	6150 4650 6150 4400
Connection ~ 6150 4400
Wire Wire Line
	6150 4400 5400 4400
$EndSCHEMATC
