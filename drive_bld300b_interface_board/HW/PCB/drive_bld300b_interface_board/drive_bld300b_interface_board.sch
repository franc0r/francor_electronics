EESchema Schematic File Version 4
LIBS:drive_bld300b_interface_board-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "BLDC BLD-300B Interface Board"
Date "2019-04-07"
Rev ""
Comp "Francor e.V."
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x19_Odd_Even J?
U 1 1 5CAA175A
P 2100 6150
F 0 "J?" H 2150 7150 50  0000 C CNN
F 1 "N_CN7" H 2150 5150 50  0000 C CNN
F 2 "" H 2100 6150 50  0001 C CNN
F 3 "~" H 2100 6150 50  0001 C CNN
	1    2100 6150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x19_Odd_Even J?
U 1 1 5CAA2D45
P 4000 6150
F 0 "J?" H 4050 7150 50  0000 C CNN
F 1 "N_CN10" H 4050 5150 50  0000 C CNN
F 2 "" H 4000 6150 50  0001 C CNN
F 3 "~" H 4000 6150 50  0001 C CNN
	1    4000 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 6150 1450 6150
Wire Wire Line
	2400 6150 2750 6150
Wire Wire Line
	2750 6150 2750 6200
Wire Wire Line
	2750 6250 2400 6250
Wire Wire Line
	2400 5550 2750 5550
$Comp
L power:GND #PWR?
U 1 1 5CAA9119
P 2750 5550
F 0 "#PWR?" H 2750 5300 50  0001 C CNN
F 1 "GND" V 2755 5422 50  0000 R CNN
F 2 "" H 2750 5550 50  0001 C CNN
F 3 "" H 2750 5550 50  0001 C CNN
	1    2750 5550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5CAA9679
P 2750 6200
F 0 "#PWR?" H 2750 5950 50  0001 C CNN
F 1 "GND" V 2755 6072 50  0000 R CNN
F 2 "" H 2750 6200 50  0001 C CNN
F 3 "" H 2750 6200 50  0001 C CNN
	1    2750 6200
	0    -1   -1   0   
$EndComp
Connection ~ 2750 6200
Wire Wire Line
	2750 6200 2750 6250
$Comp
L power:GND #PWR?
U 1 1 5CAA9A15
P 1450 6150
F 0 "#PWR?" H 1450 5900 50  0001 C CNN
F 1 "GND" V 1455 6022 50  0000 R CNN
F 2 "" H 1450 6150 50  0001 C CNN
F 3 "" H 1450 6150 50  0001 C CNN
	1    1450 6150
	0    1    1    0   
$EndComp
NoConn ~ 1900 5450
NoConn ~ 1900 6850
NoConn ~ 2400 6450
NoConn ~ 2400 6350
NoConn ~ 2400 6050
NoConn ~ 2400 5950
NoConn ~ 2400 5850
NoConn ~ 2400 5750
NoConn ~ 2400 5650
NoConn ~ 2400 5450
NoConn ~ 3800 5550
$Comp
L power:GND #PWR?
U 1 1 5CAA3886
P 3600 5650
F 0 "#PWR?" H 3600 5400 50  0001 C CNN
F 1 "GND" V 3605 5522 50  0000 R CNN
F 2 "" H 3600 5650 50  0001 C CNN
F 3 "" H 3600 5650 50  0001 C CNN
	1    3600 5650
	0    1    1    0   
$EndComp
Wire Wire Line
	3800 5650 3600 5650
Wire Wire Line
	4300 6750 4600 6750
$Comp
L power:GND #PWR?
U 1 1 5CAA4724
P 4600 6750
F 0 "#PWR?" H 4600 6500 50  0001 C CNN
F 1 "GND" V 4605 6622 50  0000 R CNN
F 2 "" H 4600 6750 50  0001 C CNN
F 3 "" H 4600 6750 50  0001 C CNN
	1    4600 6750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4300 6150 4600 6150
$Comp
L power:GND #PWR?
U 1 1 5CAA516B
P 4600 6150
F 0 "#PWR?" H 4600 5900 50  0001 C CNN
F 1 "GND" V 4605 6022 50  0000 R CNN
F 2 "" H 4600 6150 50  0001 C CNN
F 3 "" H 4600 6150 50  0001 C CNN
	1    4600 6150
	0    -1   -1   0   
$EndComp
NoConn ~ 4300 6050
NoConn ~ 4300 5550
Text Label 4650 6650 2    50   ~ 0
M1_EN
Wire Wire Line
	4300 6650 4650 6650
Wire Wire Line
	4300 6550 4650 6550
Text Label 4650 6550 2    50   ~ 0
M1_BRK
Wire Wire Line
	4300 5950 4650 5950
Text Label 4650 5950 2    50   ~ 0
M1_DIR
Wire Wire Line
	3800 6350 3350 6350
Text Label 3350 6350 0    50   ~ 0
M1_PWM
Wire Wire Line
	4300 6250 4650 6250
Text Label 4650 6250 2    50   ~ 0
M1_ALM
Wire Wire Line
	2400 7050 2750 7050
Text Label 2750 7050 2    50   ~ 0
M1_SPD
Wire Wire Line
	2400 6550 2750 6550
Wire Wire Line
	2400 6650 2750 6650
Wire Wire Line
	3800 6450 3350 6450
Text Label 2750 6550 2    50   ~ 0
M1_HA
Text Label 2750 6650 2    50   ~ 0
M1_HB
Text Label 3350 6450 0    50   ~ 0
M1_HC
Wire Wire Line
	4300 6450 4650 6450
Wire Wire Line
	3350 5250 3800 5250
Wire Wire Line
	4300 5750 4650 5750
Text Label 4650 6450 2    50   ~ 0
M2_EN
Text Label 4650 5750 2    50   ~ 0
M2_DIR
Text Label 3350 5250 0    50   ~ 0
M2_BRK
Wire Wire Line
	3800 6250 3350 6250
Wire Wire Line
	4300 6350 4650 6350
Text Label 4650 6350 2    50   ~ 0
M2_ALM
Text Label 3350 6250 0    50   ~ 0
M2_PWM
Wire Wire Line
	2400 6950 2750 6950
Text Label 2750 6950 2    50   ~ 0
M2_SPD
Wire Wire Line
	3800 5850 3350 5850
Wire Wire Line
	3800 5950 3350 5950
Wire Wire Line
	2400 6850 2750 6850
Text Label 2750 6850 2    50   ~ 0
M2_HC
Text Label 3350 5850 0    50   ~ 0
M2_HA
Text Label 3350 5950 0    50   ~ 0
M2_HB
Wire Wire Line
	2400 6750 2750 6750
Text Label 2750 6750 2    50   ~ 0
M3_EN
Wire Wire Line
	4300 6850 4650 6850
NoConn ~ 4300 6950
NoConn ~ 4300 7050
Text Label 4650 6850 2    50   ~ 0
M3_BRK
Wire Wire Line
	4300 5450 4650 5450
Text Label 4650 5450 2    50   ~ 0
M3_DIR
Wire Wire Line
	3800 6850 3350 6850
Text Label 3350 6850 0    50   ~ 0
M3_PWM
Wire Wire Line
	3800 6650 3350 6650
Text Label 3350 6650 0    50   ~ 0
M3_ALM
Text Label 1450 6950 0    50   ~ 0
M3_SPEED
Wire Wire Line
	1450 6950 1900 6950
Wire Wire Line
	3800 6050 3350 6050
Text Label 3350 6050 0    50   ~ 0
M3_HA
Wire Wire Line
	3800 5350 3350 5350
Text Label 3350 5350 0    50   ~ 0
M3_HC
Wire Wire Line
	1900 6250 1450 6250
Text Label 1450 6250 0    50   ~ 0
M3_HB
Wire Wire Line
	1900 5250 1450 5250
Text Label 1450 5250 0    50   ~ 0
M4_EN
Text Label 2800 5250 2    50   ~ 0
M4_BRK
Wire Wire Line
	2800 5250 2400 5250
Text Label 1450 5350 0    50   ~ 0
M4_DIR
Wire Wire Line
	1450 5350 1900 5350
Wire Wire Line
	4650 5850 4300 5850
Text Label 4650 5850 2    50   ~ 0
M4_PWM
Wire Wire Line
	1900 6050 1450 6050
Text Label 1450 6050 0    50   ~ 0
M4_ALM
Wire Wire Line
	1900 7050 1450 7050
Text Label 1450 7050 0    50   ~ 0
M4_SPEED
Wire Wire Line
	4300 5350 4650 5350
Wire Wire Line
	4300 5250 4650 5250
Text Label 4650 5250 2    50   ~ 0
M4_HC
Text Label 4650 5350 2    50   ~ 0
M4_HA
Wire Wire Line
	3800 6150 3350 6150
Text Label 3350 6150 0    50   ~ 0
M4_HB
NoConn ~ 1900 5550
NoConn ~ 1900 5650
NoConn ~ 1900 5750
NoConn ~ 1900 5850
NoConn ~ 1900 5950
NoConn ~ 1900 6350
NoConn ~ 1900 6450
NoConn ~ 1900 6550
NoConn ~ 1900 6650
NoConn ~ 1900 6750
NoConn ~ 2400 5350
NoConn ~ 3800 5450
NoConn ~ 3800 5750
NoConn ~ 3800 6950
NoConn ~ 3800 7050
NoConn ~ 3800 6550
NoConn ~ 3800 6750
$EndSCHEMATC