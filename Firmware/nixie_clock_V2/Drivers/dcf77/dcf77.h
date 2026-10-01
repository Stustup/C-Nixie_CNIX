#ifndef DCF77_H_
#define DCF77_H_

#include "stm32g0xx_hal.h"
#include "main.h"

// EN-Pin of DCF77-Module (Enable/Power) ACTIVE LOW
#define DCF77_EN_PORT       addon_en_GPIO_Port
#define DCF77_EN_PIN        addon_en_Pin
#define DCF77_EN_ACTIVE     0               // EN High = Modul off. 

#define DCF77_SIGNAL_PORT   GPIOA
#define DCF77_SIGNAL_PIN    GPIO_PIN_2

// Capture-channel
#define DCF77_TIM_CHANNEL  TIM_CHANNEL_3

typedef struct {
    uint8_t second;     // 0-59
    uint8_t minute;     // 0-59
    uint8_t hour;       // 0-23
    uint8_t day;        // 1-31
    uint8_t month;      // 1-12
    uint8_t year;       //0-99
    uint8_t weekday;    //1-7
    uint8_t is_dst;     // 1 = summertime (MESZ)
    uint8_t parity_ok;  // 1 = Parity ok
    uint8_t data_valid; // 1 = at least one good decoded minute
} DCF77_TimeTypeDef;

/**
 * @brief Initiates the DCF module
 * @param *htim -> Timer handle of configured capture compare timer 
 */
void DCF77_Init(TIM_HandleTypeDef *htim);

void DCF77_Enable(void);     // EN-Pin reset
void DCF77_Disable(void);    // EN-Pin set

/**
 * @brief Start recieving and decoding dcf77 signals.
 */
void DCF77_Start(void);

/**
 * @brief Stops timer and module
 */
void DCF77_Stop(void);

/**
 * @brief Returns decoded time as DCF77_TimeTypeDef
 */
DCF77_TimeTypeDef DCF77_GetTime(void);

/**
 * @brief returns 1 if a valid time got decoded, 0 if not.
 */
uint8_t DCF77_IsDataValid(void);

//Callback after successfully decoding a minute. Gets implemented in main file
typedef void (*DCF77_CallbackTypeDef)(DCF77_TimeTypeDef *time);
void DCF77_RegisterCallback(DCF77_CallbackTypeDef callback);

#endif