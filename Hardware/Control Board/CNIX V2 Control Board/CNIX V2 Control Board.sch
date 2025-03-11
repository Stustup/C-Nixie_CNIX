EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x16 J1
U 1 1 61D9053D
P 900 2550
F 0 "J1" V 1117 2496 50  0000 C CNN
F 1 "Control" V 1026 2496 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x16_P2.54mm_Vertical" H 900 2550 50  0001 C CNN
F 3 "~" H 900 2550 50  0001 C CNN
	1    900  2550
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 61D919D8
P 8800 2450
F 0 "J5" H 8880 2442 50  0000 L CNN
F 1 "LED" H 8880 2351 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 8800 2450 50  0001 C CNN
F 3 "~" H 8800 2450 50  0001 C CNN
	1    8800 2450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 61D91DE4
P 8800 1900
F 0 "J4" H 8880 1892 50  0000 L CNN
F 1 "GND" H 8880 1801 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 8800 1900 50  0001 C CNN
F 3 "~" H 8800 1900 50  0001 C CNN
	1    8800 1900
	1    0    0    -1  
$EndComp
$Comp
L MCU_Module:Arduino_Nano_v2.x A1
U 1 1 61D9265D
P 1900 6300
F 0 "A1" V 1900 6700 50  0000 C CNN
F 1 "Arduino_Nano_v2.x" V 1900 6200 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 1900 6300 50  0001 C CIN
F 3 "https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf" H 1900 6300 50  0001 C CNN
	1    1900 6300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 61D9385D
P 6150 6600
F 0 "SW1" H 6150 6885 50  0000 C CNN
F 1 "SW_Plus" H 6150 6794 50  0000 C CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx31-2LFS" H 6150 6800 50  0001 C CNN
F 3 "~" H 6150 6800 50  0001 C CNN
	1    6150 6600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 61D938C5
P 6150 6950
F 0 "SW2" H 6150 7235 50  0000 C CNN
F 1 "SW_Menu" H 6150 7144 50  0000 C CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx31-2LFS" H 6150 7150 50  0001 C CNN
F 3 "~" H 6150 7150 50  0001 C CNN
	1    6150 6950
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 61D93A0E
P 6150 7300
F 0 "SW3" H 6150 7585 50  0000 C CNN
F 1 "SW_Minus" H 6150 7494 50  0000 C CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx31-2LFS" H 6150 7500 50  0001 C CNN
F 3 "~" H 6150 7500 50  0001 C CNN
	1    6150 7300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 61D92084
P 8800 1300
F 0 "J2" H 8880 1292 50  0000 L CNN
F 1 "5V" H 8880 1201 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 8800 1300 50  0001 C CNN
F 3 "~" H 8800 1300 50  0001 C CNN
	1    8800 1300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 61D91CB1
P 8800 750
F 0 "J3" H 8880 742 50  0000 L CNN
F 1 "HT" H 8880 651 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 8800 750 50  0001 C CNN
F 3 "~" H 8800 750 50  0001 C CNN
	1    8800 750 
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J8
U 1 1 61C8BBEF
P 9900 4650
F 0 "J8" H 9957 4967 50  0000 C CNN
F 1 "9-24V DC" H 9957 4876 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 9950 4610 50  0001 C CNN
F 3 "~" H 9950 4610 50  0001 C CNN
	1    9900 4650
	-1   0    0    1   
$EndComp
$Comp
L Device:CP_Small C6
U 1 1 61C8CB16
P 6100 4700
F 0 "C6" H 6188 4746 50  0000 L CNN
F 1 "22µF 400V" H 6188 4655 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6100 4700 50  0001 C CNN
F 3 "~" H 6100 4700 50  0001 C CNN
	1    6100 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C9
U 1 1 61C8D49F
P 8250 4650
F 0 "C9" H 8342 4696 50  0000 L CNN
F 1 "100nF " H 8342 4605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 8250 4650 50  0001 C CNN
F 3 "~" H 8250 4650 50  0001 C CNN
	1    8250 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C7
U 1 1 61C8E4B2
P 6700 4700
F 0 "C7" H 6788 4746 50  0000 L CNN
F 1 "22µF 400V" H 6788 4655 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6700 4700 50  0001 C CNN
F 3 "~" H 6700 4700 50  0001 C CNN
	1    6700 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C10
U 1 1 61C8EF17
P 8700 4650
F 0 "C10" H 8788 4696 50  0000 L CNN
F 1 "100µF 16V" H 8788 4605 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 8700 4650 50  0001 C CNN
F 3 "~" H 8700 4650 50  0001 C CNN
	1    8700 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:Ferrite_Bead_Small FB1
U 1 1 61C8DD5F
P 9050 4550
F 0 "FB1" V 9300 4450 50  0000 L CNN
F 1 "100R@100MHz" V 9200 4200 50  0000 L CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.05x1.20mm_HandSolder" V 8980 4550 50  0001 C CNN
F 3 "~" H 9050 4550 50  0001 C CNN
	1    9050 4550
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 61C91449
P 9850 3500
F 0 "R2" H 9909 3546 50  0000 L CNN
F 1 "4k7" H 9909 3455 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 9850 3500 50  0001 C CNN
F 3 "~" H 9850 3500 50  0001 C CNN
	1    9850 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R1
U 1 1 61C919C2
P 9600 3500
F 0 "R1" H 9659 3546 50  0000 L CNN
F 1 "4k7" H 9659 3455 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 9600 3500 50  0001 C CNN
F 3 "~" H 9600 3500 50  0001 C CNN
	1    9600 3500
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0101
U 1 1 61C91EA2
P 9850 3350
F 0 "#PWR0101" H 9850 3200 50  0001 C CNN
F 1 "+5V" H 9865 3523 50  0000 C CNN
F 2 "" H 9850 3350 50  0001 C CNN
F 3 "" H 9850 3350 50  0001 C CNN
	1    9850 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 3350 9600 3400
Wire Wire Line
	9850 3350 9850 3400
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 61C93D9D
P 10700 3550
F 0 "J9" H 10618 3125 50  0000 C CNN
F 1 "RTC" H 10618 3216 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x05_P2.54mm_Vertical" H 10700 3550 50  0001 C CNN
F 3 "~" H 10700 3550 50  0001 C CNN
	1    10700 3550
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 61C94E0B
P 10400 3850
F 0 "#PWR0102" H 10400 3600 50  0001 C CNN
F 1 "GND" H 10405 3677 50  0000 C CNN
F 2 "" H 10400 3850 50  0001 C CNN
F 3 "" H 10400 3850 50  0001 C CNN
	1    10400 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10400 3850 10400 3750
Wire Wire Line
	10400 3750 10500 3750
$Comp
L power:+5V #PWR0103
U 1 1 61C95CC7
P 10400 3650
F 0 "#PWR0103" H 10400 3500 50  0001 C CNN
F 1 "+5V" V 10415 3778 50  0000 L CNN
F 2 "" H 10400 3650 50  0001 C CNN
F 3 "" H 10400 3650 50  0001 C CNN
	1    10400 3650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10400 3650 10500 3650
Text GLabel 10450 3550 0    50   Input ~ 0
SDA
Text GLabel 10450 3450 0    50   Input ~ 0
SCL
Text GLabel 10450 3350 0    50   Input ~ 0
DS
Wire Wire Line
	10450 3350 10500 3350
Wire Wire Line
	10450 3450 10500 3450
Wire Wire Line
	10450 3550 10500 3550
Text GLabel 9850 3650 3    50   Input ~ 0
SDA
Text GLabel 9600 3650 3    50   Input ~ 0
SCL
Wire Wire Line
	9600 3650 9600 3600
Wire Wire Line
	9850 3650 9850 3600
$Comp
L 74xx:74HC595 U1
U 1 1 61D8FA23
P 2550 1550
F 0 "U1" H 2500 2200 50  0000 R CNN
F 1 "74HC595" H 2500 2100 50  0000 R CNN
F 2 "Package_SO:SOP-16_3.9x9.9mm_P1.27mm" H 2550 1550 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 2550 1550 50  0001 C CNN
	1    2550 1550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3000 3000 2950 3000
Wire Wire Line
	2100 2450 2100 2050
Wire Wire Line
	2100 2050 2150 2050
Wire Wire Line
	2100 2450 3000 2450
Wire Wire Line
	3000 2450 3000 3000
$Comp
L power:+5V #PWR0104
U 1 1 61CA5A36
P 2550 2750
F 0 "#PWR0104" H 2550 2600 50  0001 C CNN
F 1 "+5V" H 2565 2923 50  0000 C CNN
F 2 "" H 2550 2750 50  0001 C CNN
F 3 "" H 2550 2750 50  0001 C CNN
	1    2550 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2750 2550 2800
$Comp
L Device:C_Small C3
U 1 1 61CA656E
P 2350 2750
F 0 "C3" V 2121 2750 50  0000 C CNN
F 1 "100nF " V 2212 2750 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2350 2750 50  0001 C CNN
F 3 "~" H 2350 2750 50  0001 C CNN
	1    2350 2750
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 61CA763A
P 2550 4150
F 0 "#PWR0105" H 2550 3900 50  0001 C CNN
F 1 "GND" H 2650 4150 50  0000 C CNN
F 2 "" H 2550 4150 50  0001 C CNN
F 3 "" H 2550 4150 50  0001 C CNN
	1    2550 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 61CA7AB9
P 2550 2300
F 0 "#PWR0106" H 2550 2050 50  0001 C CNN
F 1 "GND" H 2650 2300 50  0000 C CNN
F 2 "" H 2550 2300 50  0001 C CNN
F 3 "" H 2550 2300 50  0001 C CNN
	1    2550 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2250 2550 2300
Wire Wire Line
	2550 4100 2550 4150
$Comp
L power:GND #PWR0107
U 1 1 61CA8B0C
P 2200 2800
F 0 "#PWR0107" H 2200 2550 50  0001 C CNN
F 1 "GND" H 2100 2800 50  0000 C CNN
F 2 "" H 2200 2800 50  0001 C CNN
F 3 "" H 2200 2800 50  0001 C CNN
	1    2200 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 2750 2250 2750
Wire Wire Line
	2200 2750 2200 2800
$Comp
L 74xx:74HC595 U2
U 1 1 61D8DEB7
P 2550 3400
F 0 "U2" H 2500 4050 50  0000 R CNN
F 1 "74HC595" H 2500 3950 50  0000 R CNN
F 2 "Package_SO:SOP-16_3.9x9.9mm_P1.27mm" H 2550 3400 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 2550 3400 50  0001 C CNN
	1    2550 3400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2450 2750 2550 2750
Connection ~ 2550 2750
$Comp
L power:+5V #PWR0108
U 1 1 61CABC86
P 2550 900
F 0 "#PWR0108" H 2550 750 50  0001 C CNN
F 1 "+5V" H 2565 1073 50  0000 C CNN
F 2 "" H 2550 900 50  0001 C CNN
F 3 "" H 2550 900 50  0001 C CNN
	1    2550 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 900  2550 950 
$Comp
L Device:C_Small C2
U 1 1 61CABC8D
P 2350 900
F 0 "C2" V 2121 900 50  0000 C CNN
F 1 "100nF " V 2212 900 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2350 900 50  0001 C CNN
F 3 "~" H 2350 900 50  0001 C CNN
	1    2350 900 
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 61CABC93
P 2200 950
F 0 "#PWR0109" H 2200 700 50  0001 C CNN
F 1 "GND" H 2050 900 50  0000 C CNN
F 2 "" H 2200 950 50  0001 C CNN
F 3 "" H 2200 950 50  0001 C CNN
	1    2200 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 900  2250 900 
Wire Wire Line
	2200 900  2200 950 
Wire Wire Line
	2450 900  2550 900 
Connection ~ 2550 900 
Text GLabel 3400 1150 2    50   Input ~ 0
DATA
$Comp
L power:GND #PWR0110
U 1 1 61CB0C4C
P 3000 1800
F 0 "#PWR0110" H 3000 1550 50  0001 C CNN
F 1 "GND" H 3100 1800 50  0000 C CNN
F 2 "" H 3000 1800 50  0001 C CNN
F 3 "" H 3000 1800 50  0001 C CNN
	1    3000 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 1800 3000 1750
Wire Wire Line
	3000 1750 2950 1750
$Comp
L power:GND #PWR0111
U 1 1 61CB1669
P 3000 3650
F 0 "#PWR0111" H 3000 3400 50  0001 C CNN
F 1 "GND" H 3100 3650 50  0000 C CNN
F 2 "" H 3000 3650 50  0001 C CNN
F 3 "" H 3000 3650 50  0001 C CNN
	1    3000 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3650 3000 3600
Wire Wire Line
	3000 3600 2950 3600
$Comp
L power:+5V #PWR0112
U 1 1 61CB22AE
P 3000 3300
F 0 "#PWR0112" H 3000 3150 50  0001 C CNN
F 1 "+5V" V 3100 3300 50  0000 L CNN
F 2 "" H 3000 3300 50  0001 C CNN
F 3 "" H 3000 3300 50  0001 C CNN
	1    3000 3300
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0113
U 1 1 61CB2D32
P 3000 1450
F 0 "#PWR0113" H 3000 1300 50  0001 C CNN
F 1 "+5V" V 3015 1578 50  0000 L CNN
F 2 "" H 3000 1450 50  0001 C CNN
F 3 "" H 3000 1450 50  0001 C CNN
	1    3000 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 1450 2950 1450
Wire Wire Line
	3000 3300 2950 3300
Text GLabel 3400 1350 2    50   Input ~ 0
CLK
Text GLabel 3400 1650 2    50   Input ~ 0
LATCH
Wire Wire Line
	3400 1650 3250 1650
Wire Wire Line
	2950 1350 3350 1350
Wire Wire Line
	3400 1150 2950 1150
Wire Wire Line
	3250 1650 3250 3500
Wire Wire Line
	3250 3500 2950 3500
Connection ~ 3250 1650
Wire Wire Line
	3250 1650 2950 1650
Wire Wire Line
	3350 1350 3350 3200
Wire Wire Line
	3350 3200 2950 3200
Connection ~ 3350 1350
Wire Wire Line
	3350 1350 3400 1350
Wire Wire Line
	2150 1850 2000 1850
Wire Wire Line
	2000 1850 2000 2550
Wire Wire Line
	2000 2550 1100 2550
Wire Wire Line
	2150 1750 1900 1750
Wire Wire Line
	1900 1750 1900 2450
Wire Wire Line
	1900 2450 1100 2450
Wire Wire Line
	2150 1650 1800 1650
Wire Wire Line
	1800 1650 1800 2350
Wire Wire Line
	1800 2350 1100 2350
Wire Wire Line
	2150 1550 1700 1550
Wire Wire Line
	1700 1550 1700 2250
Wire Wire Line
	1700 2250 1100 2250
Wire Wire Line
	2150 1450 1600 1450
Wire Wire Line
	1600 1450 1600 2150
Wire Wire Line
	1600 2150 1100 2150
Wire Wire Line
	2150 1350 1500 1350
Wire Wire Line
	1500 1350 1500 2050
Wire Wire Line
	1500 2050 1100 2050
Wire Wire Line
	2150 1250 1400 1250
Wire Wire Line
	1400 1250 1400 1950
Wire Wire Line
	1400 1950 1100 1950
Wire Wire Line
	1100 1850 1300 1850
Wire Wire Line
	1300 1850 1300 1150
Wire Wire Line
	1300 1150 2150 1150
Wire Wire Line
	2150 3000 2000 3000
Wire Wire Line
	2000 3000 2000 2650
Wire Wire Line
	2000 2650 1100 2650
Wire Wire Line
	2150 3100 1900 3100
Wire Wire Line
	1900 3100 1900 2750
Wire Wire Line
	1900 2750 1100 2750
Wire Wire Line
	2150 3200 1800 3200
Wire Wire Line
	1800 3200 1800 2850
Wire Wire Line
	1800 2850 1100 2850
Wire Wire Line
	2150 3300 1700 3300
Wire Wire Line
	1700 3300 1700 2950
Wire Wire Line
	1700 2950 1100 2950
Wire Wire Line
	2150 3400 1600 3400
Wire Wire Line
	1600 3400 1600 3050
Wire Wire Line
	1600 3050 1100 3050
Wire Wire Line
	2150 3500 1500 3500
Wire Wire Line
	1500 3500 1500 3150
Wire Wire Line
	1500 3150 1100 3150
Wire Wire Line
	2150 3600 1400 3600
Wire Wire Line
	1400 3600 1400 3250
Wire Wire Line
	1400 3250 1100 3250
Wire Wire Line
	1100 3350 1300 3350
Wire Wire Line
	1300 3350 1300 3700
Wire Wire Line
	1300 3700 2150 3700
$Comp
L power:+5V #PWR0114
U 1 1 61C922DD
P 9600 3350
F 0 "#PWR0114" H 9600 3200 50  0001 C CNN
F 1 "+5V" H 9615 3523 50  0000 C CNN
F 2 "" H 9600 3350 50  0001 C CNN
F 3 "" H 9600 3350 50  0001 C CNN
	1    9600 3350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0115
U 1 1 61CFBC36
P 2100 5250
F 0 "#PWR0115" H 2100 5100 50  0001 C CNN
F 1 "+5V" H 2115 5423 50  0000 C CNN
F 2 "" H 2100 5250 50  0001 C CNN
F 3 "" H 2100 5250 50  0001 C CNN
	1    2100 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 5250 2100 5300
$Comp
L power:GND #PWR0116
U 1 1 61CFF483
P 1950 7450
F 0 "#PWR0116" H 1950 7200 50  0001 C CNN
F 1 "GND" H 1955 7277 50  0000 C CNN
F 2 "" H 1950 7450 50  0001 C CNN
F 3 "" H 1950 7450 50  0001 C CNN
	1    1950 7450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 7400 1950 7400
Wire Wire Line
	2000 7400 2000 7300
Wire Wire Line
	1900 7300 1900 7400
Wire Wire Line
	1950 7400 1950 7450
Connection ~ 1950 7400
Wire Wire Line
	1950 7400 2000 7400
$Comp
L power:+12V #PWR0117
U 1 1 61D09489
P 8450 4450
F 0 "#PWR0117" H 8450 4300 50  0001 C CNN
F 1 "+12V" H 8465 4623 50  0000 C CNN
F 2 "" H 8450 4450 50  0001 C CNN
F 3 "" H 8450 4450 50  0001 C CNN
	1    8450 4450
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0118
U 1 1 61D09D1D
P 1800 4900
F 0 "#PWR0118" H 1800 4750 50  0001 C CNN
F 1 "+12V" H 1815 5073 50  0000 C CNN
F 2 "" H 1800 4900 50  0001 C CNN
F 3 "" H 1800 4900 50  0001 C CNN
	1    1800 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 5250 1800 5300
Wire Wire Line
	8450 4450 8450 4550
Wire Wire Line
	8700 4550 8450 4550
Connection ~ 8700 4550
Wire Wire Line
	8250 4750 8450 4750
Wire Wire Line
	8950 4550 8700 4550
Wire Wire Line
	9150 4550 9600 4550
$Comp
L power:GND #PWR0119
U 1 1 61D335A6
P 9500 4900
F 0 "#PWR0119" H 9500 4650 50  0001 C CNN
F 1 "GND" H 9505 4727 50  0000 C CNN
F 2 "" H 9500 4900 50  0001 C CNN
F 3 "" H 9500 4900 50  0001 C CNN
	1    9500 4900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0120
U 1 1 61D338CF
P 8450 4900
F 0 "#PWR0120" H 8450 4650 50  0001 C CNN
F 1 "GND" H 8455 4727 50  0000 C CNN
F 2 "" H 8450 4900 50  0001 C CNN
F 3 "" H 8450 4900 50  0001 C CNN
	1    8450 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 4900 8450 4750
Connection ~ 8450 4750
Wire Wire Line
	8450 4750 8700 4750
Wire Wire Line
	9500 4900 9500 4750
Wire Wire Line
	9500 4750 9600 4750
Connection ~ 8450 4550
Wire Wire Line
	8450 4550 8250 4550
$Comp
L Device:C_Small C8
U 1 1 61D460E6
P 7300 4700
F 0 "C8" H 7392 4746 50  0000 L CNN
F 1 "100nF 400V" H 7392 4655 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm" H 7300 4700 50  0001 C CNN
F 3 "~" H 7300 4700 50  0001 C CNN
	1    7300 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 4600 6700 4600
Connection ~ 6700 4600
Wire Wire Line
	6700 4600 6100 4600
$Comp
L power:GND #PWR0121
U 1 1 61D4D7DB
P 6700 4900
F 0 "#PWR0121" H 6700 4650 50  0001 C CNN
F 1 "GND" H 6705 4727 50  0000 C CNN
F 2 "" H 6700 4900 50  0001 C CNN
F 3 "" H 6700 4900 50  0001 C CNN
	1    6700 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 4800 6700 4800
Connection ~ 6700 4800
Wire Wire Line
	6700 4800 7300 4800
Wire Wire Line
	6700 4800 6700 4900
$Comp
L power:HT #PWR0122
U 1 1 61D55A61
P 6700 4550
F 0 "#PWR0122" H 6700 4670 50  0001 C CNN
F 1 "HT" H 6720 4693 50  0000 C CNN
F 2 "" H 6700 4550 50  0001 C CNN
F 3 "" H 6700 4550 50  0001 C CNN
	1    6700 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 4550 6700 4600
Text GLabel 5900 6600 0    50   Input ~ 0
Plus
Text GLabel 5900 6950 0    50   Input ~ 0
Menu
Text GLabel 5900 7300 0    50   Input ~ 0
Minus
Wire Wire Line
	5900 6600 5950 6600
Wire Wire Line
	5900 6950 5950 6950
Wire Wire Line
	5900 7300 5950 7300
$Comp
L power:GND #PWR0123
U 1 1 61D7ED0E
P 6400 7400
F 0 "#PWR0123" H 6400 7150 50  0001 C CNN
F 1 "GND" H 6405 7227 50  0000 C CNN
F 2 "" H 6400 7400 50  0001 C CNN
F 3 "" H 6400 7400 50  0001 C CNN
	1    6400 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 7400 6400 7300
Wire Wire Line
	6400 6600 6350 6600
Wire Wire Line
	6350 6950 6400 6950
Connection ~ 6400 6950
Wire Wire Line
	6400 6950 6400 6600
Wire Wire Line
	6350 7300 6400 7300
Connection ~ 6400 7300
Wire Wire Line
	6400 7300 6400 6950
Text GLabel 1350 6400 0    50   Input ~ 0
Plus
Text GLabel 1350 6300 0    50   Input ~ 0
Menu
Text GLabel 1350 6200 0    50   Input ~ 0
Minus
Wire Wire Line
	1350 6200 1400 6200
Wire Wire Line
	1350 6300 1400 6300
Wire Wire Line
	1350 6400 1400 6400
Text GLabel 1350 6100 0    50   Input ~ 0
DATA
Text GLabel 1350 6000 0    50   Input ~ 0
CLK
Text GLabel 900  5900 0    50   Input ~ 0
LATCH
Wire Wire Line
	900  5900 1000 5900
Wire Wire Line
	1350 6000 1400 6000
Wire Wire Line
	1350 6100 1400 6100
Text GLabel 2450 6700 2    50   Input ~ 0
SDA
Text GLabel 2450 6800 2    50   Input ~ 0
SCL
Wire Wire Line
	2450 6700 2400 6700
Wire Wire Line
	2400 6800 2450 6800
$Comp
L Connector_Generic:Conn_01x06 J7
U 1 1 61DEE593
P 3200 6700
F 0 "J7" H 3280 6692 50  0000 L CNN
F 1 "Misc A" H 3280 6601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 3200 6700 50  0001 C CNN
F 3 "~" H 3200 6700 50  0001 C CNN
	1    3200 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 6800 2700 6800
Wire Wire Line
	2700 6800 2700 6600
Wire Wire Line
	2700 6600 2400 6600
Wire Wire Line
	2400 6500 2750 6500
Wire Wire Line
	2750 6500 2750 6700
Wire Wire Line
	2750 6700 3000 6700
Wire Wire Line
	3000 6600 2800 6600
Wire Wire Line
	2800 6600 2800 6400
Wire Wire Line
	2800 6400 2400 6400
Wire Wire Line
	2400 6300 2850 6300
Wire Wire Line
	2850 6300 2850 6500
Wire Wire Line
	2850 6500 3000 6500
Wire Wire Line
	3000 6900 2400 6900
Wire Wire Line
	2400 7000 3000 7000
Wire Wire Line
	1050 6700 1400 6700
Wire Wire Line
	1050 6800 1400 6800
Wire Wire Line
	1400 6900 1050 6900
Wire Wire Line
	1050 7000 1400 7000
$Comp
L Device:C_Small C5
U 1 1 61E4E283
P 2700 6100
F 0 "C5" V 2471 6100 50  0000 C CNN
F 1 "100nF " V 2562 6100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2700 6100 50  0001 C CNN
F 3 "~" H 2700 6100 50  0001 C CNN
	1    2700 6100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0124
U 1 1 61E4E72D
P 3000 6200
F 0 "#PWR0124" H 3000 5950 50  0001 C CNN
F 1 "GND" H 3005 6027 50  0000 C CNN
F 2 "" H 3000 6200 50  0001 C CNN
F 3 "" H 3000 6200 50  0001 C CNN
	1    3000 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 6100 2600 6100
Wire Wire Line
	3000 6100 3000 6200
Wire Wire Line
	2800 6100 3000 6100
$Comp
L Device:C_Small C4
U 1 1 61E5CDF6
P 2500 5350
F 0 "C4" H 2408 5304 50  0000 R CNN
F 1 "100nF " H 2408 5395 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2500 5350 50  0001 C CNN
F 3 "~" H 2500 5350 50  0001 C CNN
	1    2500 5350
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 61E5D115
P 1300 5350
F 0 "C1" H 1209 5304 50  0000 R CNN
F 1 "100nF " H 1209 5395 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1300 5350 50  0001 C CNN
F 3 "~" H 1300 5350 50  0001 C CNN
	1    1300 5350
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0125
U 1 1 61E5D901
P 1300 5500
F 0 "#PWR0125" H 1300 5250 50  0001 C CNN
F 1 "GND" H 1305 5327 50  0000 C CNN
F 2 "" H 1300 5500 50  0001 C CNN
F 3 "" H 1300 5500 50  0001 C CNN
	1    1300 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0126
U 1 1 61E6CD59
P 2500 5500
F 0 "#PWR0126" H 2500 5250 50  0001 C CNN
F 1 "GND" H 2505 5327 50  0000 C CNN
F 2 "" H 2500 5500 50  0001 C CNN
F 3 "" H 2500 5500 50  0001 C CNN
	1    2500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 5500 2500 5450
Wire Wire Line
	2500 5250 2100 5250
Connection ~ 2100 5250
Wire Wire Line
	1300 5500 1300 5450
Wire Wire Line
	1300 5250 1800 5250
$Comp
L Diode:1N4001 D1
U 1 1 61EB245A
P 1800 5100
F 0 "D1" V 1850 5350 50  0000 R CNN
F 1 "1N4001" V 1750 5450 50  0000 R CNN
F 2 "Diode_SMD:D_SMA_Handsoldering" H 1800 4925 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 1800 5100 50  0001 C CNN
	1    1800 5100
	0    -1   -1   0   
$EndComp
Connection ~ 1800 5250
Wire Wire Line
	1800 4950 1800 4900
Text GLabel 8550 2450 0    50   Input ~ 0
LED_2
Text GLabel 8550 2550 0    50   Input ~ 0
LED_1
Wire Wire Line
	8550 2550 8600 2550
Wire Wire Line
	8600 2450 8550 2450
Text GLabel 1350 6500 0    50   Input ~ 0
LED_2
Text GLabel 1350 6600 0    50   Input ~ 0
LED_1
Wire Wire Line
	1350 6600 1400 6600
Wire Wire Line
	1400 6500 1350 6500
$Comp
L Connector_Generic:Conn_01x04 J6
U 1 1 61F0EF14
P 850 6800
F 0 "J6" H 750 6550 50  0000 C CNN
F 1 "Misc_D" H 750 6450 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 850 6800 50  0001 C CNN
F 3 "~" H 850 6800 50  0001 C CNN
	1    850  6800
	-1   0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 61F17D9F
P 1000 6050
F 0 "C11" H 850 6000 50  0000 C CNN
F 1 "100nF " H 800 6100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1000 6050 50  0001 C CNN
F 3 "~" H 1000 6050 50  0001 C CNN
	1    1000 6050
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 61F289C7
P 1000 6200
F 0 "#PWR01" H 1000 5950 50  0001 C CNN
F 1 "GND" H 850 6150 50  0000 C CNN
F 2 "" H 1000 6200 50  0001 C CNN
F 3 "" H 1000 6200 50  0001 C CNN
	1    1000 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 5950 1000 5900
Connection ~ 1000 5900
Wire Wire Line
	1000 5900 1400 5900
Wire Wire Line
	1000 6150 1000 6200
$Comp
L power:+5V #PWR03
U 1 1 61F39B79
P 8550 1250
F 0 "#PWR03" H 8550 1100 50  0001 C CNN
F 1 "+5V" H 8565 1423 50  0000 C CNN
F 2 "" H 8550 1250 50  0001 C CNN
F 3 "" H 8550 1250 50  0001 C CNN
	1    8550 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 1250 8550 1300
Wire Wire Line
	8550 1300 8600 1300
$Comp
L power:GND #PWR04
U 1 1 61F42C2A
P 8550 1450
F 0 "#PWR04" H 8550 1200 50  0001 C CNN
F 1 "GND" H 8555 1277 50  0000 C CNN
F 2 "" H 8550 1450 50  0001 C CNN
F 3 "" H 8550 1450 50  0001 C CNN
	1    8550 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 1450 8550 1400
Wire Wire Line
	8550 1400 8600 1400
$Comp
L power:GND #PWR05
U 1 1 61F5566E
P 8550 2100
F 0 "#PWR05" H 8550 1850 50  0001 C CNN
F 1 "GND" H 8555 1927 50  0000 C CNN
F 2 "" H 8550 2100 50  0001 C CNN
F 3 "" H 8550 2100 50  0001 C CNN
	1    8550 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2100 8550 2000
Wire Wire Line
	8550 1900 8600 1900
Wire Wire Line
	8600 2000 8550 2000
Connection ~ 8550 2000
Wire Wire Line
	8550 2000 8550 1900
$Comp
L power:HT #PWR02
U 1 1 61F70546
P 8550 700
F 0 "#PWR02" H 8550 820 50  0001 C CNN
F 1 "HT" H 8570 843 50  0000 C CNN
F 2 "" H 8550 700 50  0001 C CNN
F 3 "" H 8550 700 50  0001 C CNN
	1    8550 700 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 700  8550 750 
Wire Wire Line
	8550 850  8600 850 
Wire Wire Line
	8600 750  8550 750 
Connection ~ 8550 750 
Wire Wire Line
	8550 750  8550 850 
$EndSCHEMATC
