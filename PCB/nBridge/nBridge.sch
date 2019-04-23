EESchema Schematic File Version 4
LIBS:nBridge-cache
EELAYER 29 0
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
L Connector:USB_C_Receptacle_USB2.0 J1
U 1 1 5C9F1990
P 1150 6500
F 0 "J1" H 1257 7367 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 1257 7276 50  0000 C CNN
F 2 "SharedFootprints:12401610E4#2A" H 1300 6500 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 1300 6500 50  0001 C CNN
	1    1150 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 5900 2150 5900
Wire Wire Line
	2150 5900 2150 6450
Wire Wire Line
	2150 6450 2500 6450
$Comp
L power:GND #PWR02
U 1 1 5C9F3EAE
P 1150 7400
F 0 "#PWR02" H 1150 7150 50  0001 C CNN
F 1 "GND" H 1155 7227 50  0000 C CNN
F 2 "" H 1150 7400 50  0001 C CNN
F 3 "" H 1150 7400 50  0001 C CNN
	1    1150 7400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5C9F4381
P 850 7400
F 0 "#PWR01" H 850 7150 50  0001 C CNN
F 1 "GND" H 855 7227 50  0000 C CNN
F 2 "" H 850 7400 50  0001 C CNN
F 3 "" H 850 7400 50  0001 C CNN
	1    850  7400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5C9F45FA
P 2150 6600
F 0 "C2" H 2265 6646 50  0000 L CNN
F 1 "1uF" H 2265 6555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2188 6450 50  0001 C CNN
F 3 "~" H 2150 6600 50  0001 C CNN
	1    2150 6600
	1    0    0    -1  
$EndComp
Connection ~ 2150 6450
$Comp
L power:GND #PWR04
U 1 1 5C9F492F
P 2150 7050
F 0 "#PWR04" H 2150 6800 50  0001 C CNN
F 1 "GND" H 2155 6877 50  0000 C CNN
F 2 "" H 2150 7050 50  0001 C CNN
F 3 "" H 2150 7050 50  0001 C CNN
	1    2150 7050
	1    0    0    -1  
$EndComp
$Comp
L SharedSymbols:BQ25100YFPR U2
U 1 1 5C9E9548
P 2500 6350
F 0 "U2" H 3400 6737 60  0000 C CNN
F 1 "BQ25100YFPR" H 3400 6631 60  0000 C CNN
F 2 "SharedFootprints:BQ25100YFPR" H 3400 6590 60  0001 C CNN
F 3 "" H 2500 6350 60  0000 C CNN
	1    2500 6350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5C9F55FD
P 2500 6700
F 0 "R1" H 2570 6746 50  0000 L CNN
F 1 "10k" H 2570 6655 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2430 6700 50  0001 C CNN
F 3 "~" H 2500 6700 50  0001 C CNN
	1    2500 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 6350 2500 5550
Wire Wire Line
	4300 5850 4300 6350
Wire Wire Line
	2500 6850 2500 7050
Text Label 2500 7050 0    50   ~ 0
VSS
$Comp
L Device:C C4
U 1 1 5C9F9AB8
P 2750 5700
F 0 "C4" H 2865 5746 50  0000 L CNN
F 1 "1uF" H 2865 5655 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2788 5550 50  0001 C CNN
F 3 "~" H 2750 5700 50  0001 C CNN
	1    2750 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 5550 2750 5550
$Comp
L power:GND #PWR08
U 1 1 5C9FA8C3
P 2750 5850
F 0 "#PWR08" H 2750 5600 50  0001 C CNN
F 1 "GND" H 2755 5677 50  0000 C CNN
F 2 "" H 2750 5850 50  0001 C CNN
F 3 "" H 2750 5850 50  0001 C CNN
	1    2750 5850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5C9FAB92
P 2500 7050
F 0 "#PWR06" H 2500 6800 50  0001 C CNN
F 1 "GND" H 2505 6877 50  0000 C CNN
F 2 "" H 2500 7050 50  0001 C CNN
F 3 "" H 2500 7050 50  0001 C CNN
	1    2500 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 6750 2150 7050
Text Label 2750 5550 0    50   ~ 0
V_BAT
$Comp
L Device:R R2
U 1 1 5C9FBFCC
P 4300 6700
F 0 "R2" H 4370 6746 50  0000 L CNN
F 1 "1k35" H 4370 6655 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4230 6700 50  0001 C CNN
F 3 "~" H 4300 6700 50  0001 C CNN
	1    4300 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 6450 4700 6550
$Comp
L Device:R R3
U 1 1 5C9FD05C
P 4700 6700
F 0 "R3" H 4770 6746 50  0000 L CNN
F 1 "6k" H 4770 6655 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4630 6700 50  0001 C CNN
F 3 "~" H 4700 6700 50  0001 C CNN
	1    4700 6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5C9FD69A
P 4300 7050
F 0 "#PWR010" H 4300 6800 50  0001 C CNN
F 1 "GND" H 4305 6877 50  0000 C CNN
F 2 "" H 4300 7050 50  0001 C CNN
F 3 "" H 4300 7050 50  0001 C CNN
	1    4300 7050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5C9FE3DE
P 4700 7050
F 0 "#PWR012" H 4700 6800 50  0001 C CNN
F 1 "GND" H 4705 6877 50  0000 C CNN
F 2 "" H 4700 7050 50  0001 C CNN
F 3 "" H 4700 7050 50  0001 C CNN
	1    4700 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 6850 4300 7050
Wire Wire Line
	4700 6850 4700 7050
Wire Wire Line
	4300 6450 4700 6450
Wire Wire Line
	4300 5850 4550 5850
$Comp
L power:GND #PWR011
U 1 1 5CA014D7
P 4550 5850
F 0 "#PWR011" H 4550 5600 50  0001 C CNN
F 1 "GND" H 4555 5677 50  0000 C CNN
F 2 "" H 4550 5850 50  0001 C CNN
F 3 "" H 4550 5850 50  0001 C CNN
	1    4550 5850
	1    0    0    -1  
$EndComp
Text Notes 2550 5300 0    50   ~ 0
CHARGER\n
Text Label 4300 5850 0    50   ~ 0
VSS
$Comp
L power:+3V3 #PWR013
U 1 1 5C9E8A13
P 5600 7000
F 0 "#PWR013" H 5600 6850 50  0001 C CNN
F 1 "+3V3" H 5615 7173 50  0000 C CNN
F 2 "" H 5600 7000 50  0001 C CNN
F 3 "" H 5600 7000 50  0001 C CNN
	1    5600 7000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5C9E8706
P 5600 7500
F 0 "#PWR014" H 5600 7250 50  0001 C CNN
F 1 "GND" H 5605 7327 50  0000 C CNN
F 2 "" H 5600 7500 50  0001 C CNN
F 3 "" H 5600 7500 50  0001 C CNN
	1    5600 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 7000 5600 7000
Text Label 6000 7400 2    50   ~ 0
SWDIO
Text Label 6000 7300 2    50   ~ 0
SWDCLK
Text Label 10250 3950 0    50   ~ 0
SWDIO
Text Label 10250 3850 0    50   ~ 0
SWDCLK
$Comp
L power:GND #PWR016
U 1 1 5C9E772E
P 9300 6150
F 0 "#PWR016" H 9300 5900 50  0001 C CNN
F 1 "GND" H 9305 5977 50  0000 C CNN
F 2 "" H 9300 6150 50  0001 C CNN
F 3 "" H 9300 6150 50  0001 C CNN
	1    9300 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 5900 9300 5900
Connection ~ 9250 5900
Wire Wire Line
	9250 5750 9250 5900
Wire Wire Line
	9350 5900 9300 5900
Connection ~ 9350 5900
Wire Wire Line
	9350 5750 9350 5900
Connection ~ 9300 5900
Wire Wire Line
	9450 5900 9350 5900
Wire Wire Line
	9450 5750 9450 5900
Wire Wire Line
	9300 5900 9300 6150
Wire Wire Line
	9150 5900 9250 5900
Wire Wire Line
	9150 5750 9150 5900
Wire Wire Line
	9250 3650 9250 3350
$Comp
L power:+3V3 #PWR015
U 1 1 5C9E524A
P 9250 3350
F 0 "#PWR015" H 9250 3200 50  0001 C CNN
F 1 "+3V3" H 9265 3523 50  0000 C CNN
F 2 "" H 9250 3350 50  0001 C CNN
F 3 "" H 9250 3350 50  0001 C CNN
	1    9250 3350
	1    0    0    -1  
$EndComp
$Comp
L RF_Bluetooth:BL652 U3
U 1 1 5C9A7A9B
P 9250 4750
F 0 "U3" H 9250 6031 50  0000 C CNN
F 1 "BL652" H 9250 5940 50  0000 C CNN
F 2 "RF_Module:Laird_BL652" H 9250 3550 50  0001 C CNN
F 3 "http://cdn.lairdtech.com/home/brandworld/files/Datasheet%20-%20BL652.pdf" H 9250 3450 50  0001 C CNN
	1    9250 4750
	1    0    0    -1  
$EndComp
Wire Notes Line
	6950 6300 5150 6300
Wire Notes Line
	500  7700 6950 7700
Text Notes 5800 6550 0    50   ~ 0
SWD interface
$Comp
L SharedSymbols:TPS7A0533PYKAR U1
U 1 1 5CA16441
P 1950 4100
F 0 "U1" H 1975 4465 50  0000 C CNN
F 1 "TPS7A0533PYKAR" H 1975 4374 50  0000 C CNN
F 2 "SharedFootprints:TPS7A0533PYKAR" H 1950 4100 50  0001 C CNN
F 3 "" H 1950 4100 50  0001 C CNN
	1    1950 4100
	1    0    0    -1  
$EndComp
Text Label 600  3750 0    50   ~ 0
V_BAT
Wire Wire Line
	2300 4000 2650 4000
Wire Wire Line
	2300 4150 2300 4500
$Comp
L power:GND #PWR05
U 1 1 5CA18BD3
P 2300 4500
F 0 "#PWR05" H 2300 4250 50  0001 C CNN
F 1 "GND" H 2305 4327 50  0000 C CNN
F 2 "" H 2300 4500 50  0001 C CNN
F 3 "" H 2300 4500 50  0001 C CNN
	1    2300 4500
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR09
U 1 1 5CA190BE
P 2800 4000
F 0 "#PWR09" H 2800 3850 50  0001 C CNN
F 1 "+3V3" H 2815 4173 50  0000 C CNN
F 2 "" H 2800 4000 50  0001 C CNN
F 3 "" H 2800 4000 50  0001 C CNN
	1    2800 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5CA19979
P 800 3900
F 0 "C1" H 915 3946 50  0000 L CNN
F 1 "1uF" H 915 3855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 838 3750 50  0001 C CNN
F 3 "~" H 800 3900 50  0001 C CNN
	1    800  3900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5CA1A09E
P 2650 4150
F 0 "C3" H 2765 4196 50  0000 L CNN
F 1 "1uF" H 2765 4105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2688 4000 50  0001 C CNN
F 3 "~" H 2650 4150 50  0001 C CNN
	1    2650 4150
	1    0    0    -1  
$EndComp
Connection ~ 2650 4000
Wire Wire Line
	2650 4000 2800 4000
Wire Wire Line
	800  3750 600  3750
$Comp
L power:GND #PWR03
U 1 1 5CA1D1B4
P 800 4250
F 0 "#PWR03" H 800 4000 50  0001 C CNN
F 1 "GND" H 805 4077 50  0000 C CNN
F 2 "" H 800 4250 50  0001 C CNN
F 3 "" H 800 4250 50  0001 C CNN
	1    800  4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5CA1D762
P 2650 4500
F 0 "#PWR07" H 2650 4250 50  0001 C CNN
F 1 "GND" H 2655 4327 50  0000 C CNN
F 2 "" H 2650 4500 50  0001 C CNN
F 3 "" H 2650 4500 50  0001 C CNN
	1    2650 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 4300 2650 4500
Wire Wire Line
	800  4050 800  4250
Text Notes 1800 3400 0    50   ~ 0
REGULATOR
Text Notes 2150 5400 0    50   ~ 0
(Configured for 100mA fast charge)
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5C9F8D2A
P 6400 5650
F 0 "J3" H 6480 5642 50  0000 L CNN
F 1 "Conn_01x02" H 6480 5551 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6400 5650 50  0001 C CNN
F 3 "~" H 6400 5650 50  0001 C CNN
	1    6400 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 5750 6000 5750
$Comp
L power:GND #PWR020
U 1 1 5C9FAA63
P 6000 5750
F 0 "#PWR020" H 6000 5500 50  0001 C CNN
F 1 "GND" H 6005 5577 50  0000 C CNN
F 2 "" H 6000 5750 50  0001 C CNN
F 3 "" H 6000 5750 50  0001 C CNN
	1    6000 5750
	1    0    0    -1  
$EndComp
Text Label 5650 5300 0    50   ~ 0
V_BAT
Wire Notes Line
	500  5050 6950 5050
Text Notes 5550 5200 0    50   ~ 0
BATTERY CONNECTOR
Wire Wire Line
	1650 4150 1600 4150
Wire Wire Line
	800  3750 1200 3750
Wire Wire Line
	1200 3750 1200 4000
Wire Wire Line
	1200 4000 1650 4000
Connection ~ 800  3750
Wire Wire Line
	1200 4000 1200 4050
Connection ~ 1200 4000
$Comp
L power:GND #PWR019
U 1 1 5CA06B84
P 1200 4500
F 0 "#PWR019" H 1200 4250 50  0001 C CNN
F 1 "GND" H 1205 4327 50  0000 C CNN
F 2 "" H 1200 4500 50  0001 C CNN
F 3 "" H 1200 4500 50  0001 C CNN
	1    1200 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 4500 1200 4250
$Comp
L SharedSymbols:Switch-SPDT SW1
U 1 1 5CA0959C
P 1400 4150
F 0 "SW1" H 1400 4435 50  0000 C CNN
F 1 "Switch-SPDT" H 1400 4344 50  0000 C CNN
F 2 "SharedFootprints:CSS-121XB" H 1400 4150 50  0001 C CNN
F 3 "~" H 1400 4150 50  0001 C CNN
	1    1400 4150
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5CA132F4
P 6200 7200
F 0 "J2" H 6280 7192 50  0000 L CNN
F 1 "Conn_01x06" H 6280 7101 50  0000 L CNN
F 2 "SharedFootprints:Conn_Pas_01x06_1.27mm" H 6200 7200 50  0001 C CNN
F 3 "~" H 6200 7200 50  0001 C CNN
	1    6200 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 5250 10500 5250
Text Label 10500 5250 0    50   ~ 0
nRESET
Text Label 6000 7200 2    50   ~ 0
nRESET
Wire Wire Line
	10250 5550 10500 5550
Text Label 10500 5550 0    50   ~ 0
SWO
Text Label 6000 7100 2    50   ~ 0
SWO
$Comp
L Device:R R4
U 1 1 5CA4C0AF
P 4000 4200
F 0 "R4" H 4070 4246 50  0000 L CNN
F 1 "220R" H 4070 4155 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3930 4200 50  0001 C CNN
F 3 "~" H 4000 4200 50  0001 C CNN
	1    4000 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5CA4C820
P 4400 4200
F 0 "R5" H 4470 4246 50  0000 L CNN
F 1 "220R" H 4470 4155 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4330 4200 50  0001 C CNN
F 3 "~" H 4400 4200 50  0001 C CNN
	1    4400 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5CA4D182
P 4000 4500
F 0 "D1" V 4039 4383 50  0000 R CNN
F 1 "LED" V 3948 4383 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 4000 4500 50  0001 C CNN
F 3 "~" H 4000 4500 50  0001 C CNN
	1    4000 4500
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 5CA4D88D
P 4400 4500
F 0 "D2" V 4439 4383 50  0000 R CNN
F 1 "LED" V 4348 4383 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 4400 4500 50  0001 C CNN
F 3 "~" H 4400 4500 50  0001 C CNN
	1    4400 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4000 4050 4000 3900
Wire Wire Line
	4400 4050 4400 3900
Wire Wire Line
	4000 3900 4200 3900
Wire Wire Line
	4200 3900 4200 3800
Connection ~ 4200 3900
Wire Wire Line
	4200 3900 4400 3900
$Comp
L power:+3.3V #PWR021
U 1 1 5CA51DDC
P 4200 3800
F 0 "#PWR021" H 4200 3650 50  0001 C CNN
F 1 "+3.3V" H 4215 3973 50  0000 C CNN
F 2 "" H 4200 3800 50  0001 C CNN
F 3 "" H 4200 3800 50  0001 C CNN
	1    4200 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 4650 4000 4800
Wire Wire Line
	4400 4650 4400 4800
Text Label 4000 4800 0    50   ~ 0
LED0
Text Label 4400 4800 0    50   ~ 0
LED1
Text Label 7850 4350 2    50   ~ 0
LED0
Text Label 7850 4450 2    50   ~ 0
LED1
Wire Wire Line
	7850 4450 8250 4450
Wire Wire Line
	8250 4350 7850 4350
Wire Notes Line
	5150 3150 5150 7700
Text Notes 4400 3400 2    50   ~ 0
STATUS LEDS
$Comp
L Device:R R6
U 1 1 5CA9A3D5
P 5650 5550
F 0 "R6" H 5720 5596 50  0000 L CNN
F 1 "27k" H 5720 5505 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5580 5550 50  0001 C CNN
F 3 "~" H 5650 5550 50  0001 C CNN
	1    5650 5550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5CA9AACB
P 5650 5850
F 0 "R7" H 5720 5896 50  0000 L CNN
F 1 "10k" H 5720 5805 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5580 5850 50  0001 C CNN
F 3 "~" H 5650 5850 50  0001 C CNN
	1    5650 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 5650 5900 5400
Wire Wire Line
	5900 5400 5650 5400
Wire Wire Line
	5900 5650 6200 5650
Wire Wire Line
	5650 5400 5650 5300
Connection ~ 5650 5400
Text Label 5550 5700 2    50   ~ 0
BAT_MSR
Wire Wire Line
	5550 5700 5650 5700
Connection ~ 5650 5700
Text Label 10250 4350 0    50   ~ 0
MOSI
Text Label 10250 4450 0    50   ~ 0
BAT_MSR
Wire Wire Line
	5650 6000 5650 6100
Text Label 5650 6100 2    50   ~ 0
BAT_MSR_EN
Text Label 10250 4550 0    50   ~ 0
BAT_MSR_EN
$Comp
L nBridge:CC2500Module U4
U 1 1 5C9DFB7C
P 2050 2200
F 0 "U4" H 1550 2550 50  0000 R CNN
F 1 "CC2500Module" V 1300 2300 50  0000 R CNN
F 2 "nBridge:CC2500Module" H 2050 2200 50  0001 C CNN
F 3 "" H 2050 2200 50  0001 C CNN
	1    2050 2200
	-1   0    0    -1  
$EndComp
Wire Notes Line
	6950 3150 6950 7700
Wire Notes Line
	500  3150 6950 3150
Wire Wire Line
	2050 2000 1950 2000
Wire Wire Line
	1950 2000 1950 1650
Wire Wire Line
	1950 1650 1800 1650
Wire Wire Line
	2050 2100 1600 2100
Wire Wire Line
	1600 2100 1600 1650
Wire Wire Line
	2050 2200 1600 2200
Wire Wire Line
	2050 2300 1600 2300
Wire Wire Line
	2050 2400 1600 2400
Wire Wire Line
	2050 2500 1600 2500
Wire Wire Line
	2050 2600 1600 2600
Wire Wire Line
	2050 2700 1600 2700
$Comp
L power:GND #PWR0101
U 1 1 5CAC90E7
P 1800 1650
F 0 "#PWR0101" H 1800 1400 50  0001 C CNN
F 1 "GND" H 1805 1477 50  0000 C CNN
F 2 "" H 1800 1650 50  0001 C CNN
F 3 "" H 1800 1650 50  0001 C CNN
	1    1800 1650
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0102
U 1 1 5CAC987C
P 1600 1650
F 0 "#PWR0102" H 1600 1500 50  0001 C CNN
F 1 "+3.3V" H 1615 1823 50  0000 C CNN
F 2 "" H 1600 1650 50  0001 C CNN
F 3 "" H 1600 1650 50  0001 C CNN
	1    1600 1650
	1    0    0    -1  
$EndComp
Text Label 1600 2200 2    50   ~ 0
MOSI
Text Label 1600 2300 2    50   ~ 0
CLK
Text Label 1600 2400 2    50   ~ 0
MISO
Text Label 1600 2500 2    50   ~ 0
GDO2
Text Label 1600 2600 2    50   ~ 0
GDO0
Text Label 1600 2700 2    50   ~ 0
nCS
Text Label 10250 4250 0    50   ~ 0
CLK
Text Label 10250 5450 0    50   ~ 0
MISO
Text Label 8250 5550 2    50   ~ 0
GDO2
Text Label 8250 5350 2    50   ~ 0
GDO0
Text Label 8250 5150 2    50   ~ 0
nCS
Wire Wire Line
	5600 7500 6000 7500
Wire Notes Line
	3400 500  500  500 
Wire Notes Line
	3400 500  3400 5050
Wire Notes Line
	500  500  500  7700
Text Notes 1650 1000 0    50   ~ 0
CC2500 MODULE
$EndSCHEMATC
