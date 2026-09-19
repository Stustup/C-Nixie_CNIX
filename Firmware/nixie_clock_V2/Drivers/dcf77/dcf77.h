#ifndef DCF77_H_
#define DCF77_H_

#include "stm32g0xx_hal.h"

//Main struct for dcf77 data
typedef struct {
    uint8_t second;   // 0-59
    uint8_t minute;   // 0-59
    uint8_t hour;     // 0-23
    uint8_t parity_ok; // 1 if parity bits are valid
    uint8_t data_valid; // 1 if complete valid data received
} DCF77_TimeTypeDef;



#endif