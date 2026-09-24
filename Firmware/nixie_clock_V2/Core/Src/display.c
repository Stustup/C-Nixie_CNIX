#include "display.h"

uint16_t combine_4bit_numbers(uint8_t num0, uint8_t num1, uint8_t num2, uint8_t num3) {

  uint16_t result = 0;

  // Bit 0: bit 3 von num1
  result |= ((num1 >> 3) & 0x1) << 0;
  // Bit 1: bit 2 von num1
  result |= ((num1 >> 2) & 0x1) << 1;
  // Bit 2: bit 1 von num1
  result |= ((num1 >> 1) & 0x1) << 2;
  // Bit 3: bit 1 von num2
  result |= ((num2 >> 1) & 0x1) << 3;
  // Bit 4: bit 2 von num2
  result |= ((num2 >> 2) & 0x1) << 4;
  // Bit 5: bit 3 von num2
  result |= ((num2 >> 3) & 0x1) << 5;
  // Bit 6: bit 0 von num3
  result |= ((num3 >> 0) & 0x1) << 6;
  // Bit 7: bit 1 von num3
  result |= ((num3 >> 1) & 0x1) << 7;
  // Bit 8: bit 2 von num3
  result |= ((num3 >> 2) & 0x1) << 8;
  // Bit 9: bit 3 von num3
  result |= ((num3 >> 3) & 0x1) << 9;
  // Bit 10: bit 0 von num1
  result |= ((num1 >> 0) & 0x1) << 10;
  // Bit 11: bit 3 von num0
  result |= ((num0 >> 3) & 0x1) << 11;
  // Bit 12: bit 2 von num0
  result |= ((num0 >> 2) & 0x1) << 12;
  // Bit 13: bit 1 von num0
  result |= ((num0 >> 1) & 0x1) << 13;
  // Bit 14: bit 0 von num0
  result |= ((num0 >> 0) & 0x1) << 14;
  // Bit 15: bit 0 von num2
  result |= ((num2 >> 0) & 0x1) << 15;

  return result;
}

void output_to_tubes(uint16_t _data) {
  //Output on the whole PORTB via the ODR (Output Data Register)
  GPIOB->ODR = _data;
}

void output_to_tubeDigits(uint8_t num0, uint8_t num1, uint8_t num2, uint8_t num3) {
  output_to_tubes(combine_4bit_numbers(num0, num1, num2, num3));
}

void output_to_tubesNEW(tubeDisplay* _data) {
  output_to_tubes(_data->displayDigitOutput);
}

void output_front_led(uint8_t led0, uint8_t led1) {

  if(led0)  GPIOA->BSRR = GPIO_BSRR_BS11;
  if(!led0) GPIOA->BSRR = GPIO_BSRR_BR11;

  if(led1)  GPIOA->BSRR = GPIO_BSRR_BS12;
  if(!led1) GPIOA->BSRR = GPIO_BSRR_BR12;
}

void output_blink_front_leds(blink_mode _mode) {
  switch(_mode) {
    case solid: 
      output_front_led(1, 1);
      break;

    case blinkBoth: 
      output_front_led(blinkState(), blinkState());
      break;

    case blinkTop:
      output_front_led(blinkState(), 1);
      break;

    case blinkTopEx:
      output_front_led(blinkState(), 0);
      break;

    case blinkBot:
      output_front_led(1, blinkState());
      break;

    case blinkBotEx:
      output_front_led(0, blinkState());
      break;

    case solidBot:
      output_front_led(0, 1);
      break;

    case solidTop:
      output_front_led(1, 0);
      break;

    default: 
      output_front_led(1, 1);
      break;
  }
}

void ht_supply_state(tubeDisplay* _data) {
  if(_data->displayStatus != 1 && _data->displayStatus != 0) return;
  HAL_GPIO_WritePin(ht_EN_GPIO_Port, ht_EN_Pin, !_data->displayStatus);
}

uint8_t blinkState(void) {
  return ((HAL_GetTick() / BLINK_TIME) % 2);
}
