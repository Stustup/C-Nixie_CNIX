# Changelog for the Nixie Tube Clock Control Board

## Version 2.0.0
- New MCU (STM32G051C8T3)
- New fully integrated high voltage power supply
- Fully integrated RTC
- USB-C power supply

### V2.1.0
- Switchted from AMS1117 to AP7361C voltage regulator (drop in replacement) for better stability

### V2.1.1
- Fixed CR2025 label

### V2.2.0
- Added detection for addon modules via pins D2 and D3 (2 bit). DCF77 board has the id 0x0
- Added pin header with jumper for the HT-PSU for safe debugging and prototyping