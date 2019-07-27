EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:test-cache
EELAYER 25 0
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
L R R1
U 1 1 5C1455EE
P 2250 2650
F 0 "R1" V 2330 2650 50  0000 C CNN
F 1 "R" V 2250 2650 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0617_L17.0mm_D6.0mm_P30.48mm_Horizontal" V 2180 2650 50  0001 C CNN
F 3 "" H 2250 2650 50  0001 C CNN
	1    2250 2650
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5C145653
P 2650 2300
F 0 "R2" V 2730 2300 50  0000 C CNN
F 1 "R" V 2650 2300 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0617_L17.0mm_D6.0mm_P25.40mm_Horizontal" V 2580 2300 50  0001 C CNN
F 3 "" H 2650 2300 50  0001 C CNN
	1    2650 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	2250 2500 2250 2300
Wire Wire Line
	2250 2300 2500 2300
Wire Wire Line
	2250 2800 2800 2800
Wire Wire Line
	2800 2800 2800 2300
$EndSCHEMATC
