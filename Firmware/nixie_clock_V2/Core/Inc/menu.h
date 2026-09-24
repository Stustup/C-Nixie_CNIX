#ifndef _MENU_H_
#define _MENU_H_

/**
 * @brief: Main menu structure for the whole clock
 * @param menuTIME -> normal time display
 * @param menuDATE -> normal date display
 * @param menuStartStop -> Start/stop Time (time setting like above, but for start AND stop time) (Maybe multiple ones for morning and evening times?) (Timeout 30s) (Override with long press till next shutoff?)
 * @param menuTimeEdit -> Time set (GETS DISABLED WHEN DCF77 PLUGIN BOARD IS USED) (1. hours tens; 2. hours ones; 3. minutes tens; 4. minutes ones) (Timeout 30s)
 * @param menuSensor: Temperature and humidity sensor (Timeout 5s) (gets enabled when DCF77 addon board or other sensor board is used)
 */
typedef enum {
  menuTIME = 0,
  menuDATE,
  menuStartStop,
  menuTimeEdit,
  menuSENSOR,
  menuOVERFLOW
} menu;

#endif