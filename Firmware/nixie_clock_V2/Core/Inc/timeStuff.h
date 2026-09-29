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

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minute, uint8_t second);
HAL_StatusTypeDef setDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date);
HAL_StatusTypeDef getTimeDate(char* time, char* date, time_date_DataDigital* dTimeDate);


#endif

