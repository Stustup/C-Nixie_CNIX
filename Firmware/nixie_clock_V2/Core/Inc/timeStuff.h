#ifndef TIMESTUFF_H_
#define TIMESTUFF_H_
//includes

#include "stm32g0xx_hal_def.h"

//makros



//typedefs

typedef enum {
  monday    = 1,
  tuesday   = 2,
  wednesday = 3,
  thursday  = 4,
  friday    = 5,
  saturday  = 6,
  sunday    = 7
} weekday;

/**
 * DataDigital struct to hold digital values of the time from RTC
 */
typedef struct {
  uint8_t hours, minutes, seconds;

  uint8_t day, month, year;

  weekday weekday;

  uint8_t startHour1, stopHour1, startHour2, stopHour2;

  uint8_t in_timeframe_startStop;
} time_date_DataDigital;

/**
 * Struct to hold editable time and date
 */
typedef struct {
  uint8_t _hoursTens, _hoursOnes, _minutesTens, _minutesOnes, _daysTens, _daysOnes, _monthsTens, _monthsOnes;
} time_DataDigital;

//functions prototypes

/**
 * @brief: Sets the Time of the RTC. IMPORTANT: When regenerating the code through CubeMX comment out the time setting in the predefined function.
 * This is to only update the time when needed and not every time you program the MCU. The check is made through the Backup register DR1, to which a 
 * random number is written. Only update the time when this number is not the same on startup!
 */
HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * @brief: Sets the Date of the RTC. IMPORTANT: When regenerating the code through CubeMX comment out the date setting in the predefined function.
 * This is to only update the date when needed and not every time you program the MCU. The check is made through the Backup register DR1, to which a 
 * random number is written. Only update the Date when this number is not the same on startup!
 */
HAL_StatusTypeDef setDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date);

/**
 * @brief: Function to get time and date from RTC module. IMPORTANT: Always get time and then date TOGETHER! otherwize the druids of the forest will hunt you
 * Creates strings in predefined vhar arrays to directly print to an oled.
 * TODO: put time and date in an integer struct to push to the nixies 
 */
HAL_StatusTypeDef getTimeDate(time_date_DataDigital* dTimeDate);


#endif

