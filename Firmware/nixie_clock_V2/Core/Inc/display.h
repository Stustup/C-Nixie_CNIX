#ifndef DISPLAY_H_
#define DISPLAY_H_

//includes
#include "stm32g051xx.h"
#include "stm32g0xx_hal.h"
#include "main.h"

//makros
#define BLINK_TIME 600

//typedef

typedef struct {
  uint16_t  displayDigitOutput;
  uint8_t   displayStatus, displayStatus_old;

  uint8_t   d0, d1, d2, d3;
} tubeDisplay;

typedef enum {
  solid = 0,
  blinkTop,
  blinkTopEx,
  blinkBot,
  blinkBotEx,
  blinkBoth,
  solidBot,
  solidTop
} blink_mode;

//function prototypes

/**
 * @brief Sorts the bits to the correct spot for the output register TODO: Update PCB next time to have a nicer output register format not needing this shit -_-
 * @param num0-3 correstponds to the display tubes 0-3
 * @return 16 bit number, which can be written directly to PORTB
 */
uint16_t combine_4bit_numbers(uint8_t num0, uint8_t num1, uint8_t num2, uint8_t num3);

//---------------------------Output functions to manipulate front display

/**
 * @brief Outputs 16 bit value to PORTB
 * @param 16 Bit int to put on PORTB
 */
void output_to_tubes(uint16_t _data);

/**
 * @brief  Lets you output single digits to the display in
 * @param  num0-3 digits from 0-9 to output
 */
void output_to_tubeDigits(uint8_t num0, uint8_t num1, uint8_t num2, uint8_t num3);

/**
 * @brief Outputs 16 bit number to PORTB of the micro. Checks with displayStatus wheather the supply should be on or off 
 * @param tubeDisplay struct, in which the 16 bit displayDigitOutput can be used to store the current output
 */
void output_to_tubesNEW(tubeDisplay* _data);

/**
 * @brief: Function to manipulate the front leds.
 * @param led0: top led
 * @param led1: bottom led
 * @param led_status: 0->off; 1->on
 */
void output_front_led(uint8_t led0, uint8_t led1);

/**
 * @brief: Function to blink the front leds.
 * @param mode: solid, blinkTop, blinkBot, blinkBoth
 */
void output_blink_front_leds(blink_mode _mode);

/**
 * @brief: Function to turn on or off the HT supply
 * @param: tubeDisplay pointer to the display struct
 */
void ht_supply_state(tubeDisplay* _data);

/**
 * @brief Changes state when TICK_INTERVAL is reached. Works without timer and globally
 * @return 0 or 1 depending on TICK_INTERVAL
 */
uint8_t blinkState(void);

#endif