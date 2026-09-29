#ifndef DCF77_H_
#define DCF77_H_

#include "stm32g0xx_hal.h"

// --- Configuration ---
#define DCF77_TIMER             TIM2            // Timer used for input capture
#define DCF77_TIMER_CHANNEL     TIM_CHANNEL_2   // Capture channel
#define DCF77_GPIO_PORT         GPIOA           // GPIO port for DCF77 data pin
#define DCF77_GPIO_PIN          GPIO_PIN_2      // GPIO pin for DCF77 data pin
#define DCF77_TIMER_IRQn        TIM3_IRQn       // Timer IRQ number
#define DCF77_TIMER_IRQHandler  TIM2_IRQHandler // Timer IRQ handler

// --- Timing Constants (in microseconds) ---
#define DCF77_BIT_0_LOW_US      100000  // 100ms low for bit 0
#define DCF77_BIT_0_HIGH_US     200000  // 200ms high for bit 0
#define DCF77_BIT_1_LOW_US      200000  // 200ms low for bit 1
#define DCF77_BIT_1_HIGH_US     100000  // 100ms high for bit 1
#define DCF77_SECOND_MARK_US    500000  // 500ms low/high for second mark

// Tolerance for timing detection (in microseconds)
#define DCF77_TOLERANCE_US      20000   // 20ms tolerance

// --- Data Structure ---
typedef struct {
    uint8_t second;         // 0-59
    uint8_t minute;         // 0-59
    uint8_t hour;           // 0-23
    uint8_t parity_ok;      // 1 if parity bits are valid
    uint8_t data_valid;     // 1 if complete valid data received
} DCF77_TimeTypeDef;

// --- Function Prototypes ---
void DCF77_Init(void);
void DCF77_Start(void);
void DCF77_Stop(void);
DCF77_TimeTypeDef DCF77_GetTime(void);
uint8_t DCF77_IsDataValid(void);

// --- Callback ---
typedef void (*DCF77_CallbackTypeDef)(DCF77_TimeTypeDef*);
void DCF77_RegisterCallback(DCF77_CallbackTypeDef callback);

#endif