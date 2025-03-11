# CNIX - Nixie Tube Clock

CNIX is a nixie tube clock which accepts many diffrent nixie tubes with only slight modifications to the tube carrier board. 

![Nixie Clock tube board](./Docs/CNIX%20V2%20tube%20board%20side.png)

The motivation was, that there are not many clocks out there, that let you use almost any nixie tube you like. Most have fixed power supplies, 
underdesigned driver stages or inflexible microcontrollers. I hope to tackle these problems with an adjustable boost converter PSU, extremely 
flexible (and electrically protected) driver stages and an open source firmware written with free tools.

## Schematic

The control board consists of 3 parts:
- Power Supply
- Microcontroller
- I/O
- GPIO header for future ideas and feature expansion

![Control board schematic](./Docs/CNIX%20V2%20control%20board%20schematic.png)

Power comes from a simple USB-C plug configured for 5V 3A max with the 5k1 CC line resistors. I thought about adding a PD controller for higher input voltages,
but this would just have added complexity and locked you into more expensive USB PD power supplies.

The µC is a STM32G051, because it has an included RTC *with* added Vbat pin for an external battery, so that the RTC remembers time even when powered off. 
The unused GPIOs are broken out to a connector, so that in the future one could add cool stuff like a radio time reciever, a display or wifi.
Time is set manually with the push buttons on top. 

The PSU is heavily inspired by (this site)[link_placeholder].  

![Control board 3d view](./Docs/CNIX%20V2%20control%20board%20front.png)