#ifndef _OUTPUT_TUBE_H
#define _OUTPUT_TUBE_H

//Corresponding pin setting to the decimal numers on the tube
//Mappings are correct, no DCBA reversal

//Tens of Hours
#define tube0_D1 GPIO_BSRR_BS11
#define tube0_D0 GPIO_BSRR_BR11

#define tube0_C1 GPIO_BSRR_BS12
#define tube0_C0 GPIO_BSRR_BR12

#define tube0_B1 GPIO_BSRR_BS13
#define tube0_B0 GPIO_BSRR_BR13

#define tube0_A1 GPIO_BSRR_BS14
#define tube0_A0 GPIO_BSRR_BR14

//Ones of Hours
#define tube1_D1 GPIO_BSRR_BS0

#define tube1_C1 GPIO_BSRR_BS1

#define tube1_B1 GPIO_BSRR_BS2

#define tube1_A1 GPIO_BSRR_BS10


//Tens of minutes
#define tube2_D1 GPIO_BSRR_BS5

#define tube2_C1 GPIO_BSRR_BS4

#define tube2_B1 GPIO_BSRR_BS3

#define tube2_A1 GPIO_BSRR_BS15


//Ones of minutes
#define tube3_D1 GPIO_BSRR_BS9

#define tube3_C1 GPIO_BSRR_BS8

#define tube3_B1 GPIO_BSRR_BS7

#define tube3_A1 GPIO_BSRR_BS6

//Actual Port mapping of PORTB of GPIO
/*
#define uC_PB0  CO1_3 //Tube 1 D
#define uC_PB1  CO1_2 //Tube 1 C
#define uC_PB2  CO1_1 //Tube 1 B
#define uC_PB3  CO2_1 //Tube 2 B
#define uC_PB4  CO2_2 //Tube 2 C
#define uC_PB5  CO2_3 //Tube 2 D
#define uC_PB6  CO3_0 //Tube 3 A
#define uC_PB7  CO3_1 //Tube 3 B
#define uC_PB8  CO3_2 //Tube 3 C
#define uC_PB9  CO3_3 //Tube 3 D
#define uC_PB10 CO1_0 //Tube 1 A
#define uC_PB11 CO0_3 //Tube 0 D
#define uC_PB12 CO0_2 //Tube 0 C
#define uC_PB13 CO0_1 //Tube 0 B
#define uC_PB14 CO0_0 //Tube 0 A
#define uC_PB15 CO2_0 //Tube 2 A
*/

//Tube 0
#define uC_PB11 CO0_3 //Tube 0 D
#define uC_PB12 CO0_2 //Tube 0 C
#define uC_PB13 CO0_1 //Tube 0 B
#define uC_PB14 CO0_0 //Tube 0 A

//Tube 1
#define uC_PB0  CO1_3 //Tube 1 D
#define uC_PB1  CO1_2 //Tube 1 C
#define uC_PB2  CO1_1 //Tube 1 B
#define uC_PB10 CO1_0 //Tube 1 A

//Tube 2
#define uC_PB5  CO2_3 //Tube 2 D
#define uC_PB4  CO2_2 //Tube 2 C
#define uC_PB3  CO2_1 //Tube 2 B
#define uC_PB15 CO2_0 //Tube 2 A

//Tube 3
#define uC_PB9  CO3_3 //Tube 3 D
#define uC_PB8  CO3_2 //Tube 3 C
#define uC_PB7  CO3_1 //Tube 3 B
#define uC_PB6  CO3_0 //Tube 3 A

#endif