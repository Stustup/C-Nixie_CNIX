/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
 * WHEN GENERATING NEW CODE VIA CUBEMX ------> COMMENT OUT THE TIME SETTING IN RTC INIT
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_gcc.h"
#include "stm32g051xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_rtc.h"
#include "stm32g0xx_hal_rtc_ex.h"
#include "stm32g0xx_hal_tim.h"
#include <stdint.h>

#include "stdio.h"
#include "output_tube.h"
#include "menu.h"
#include "display.h"
#include "timeStuff.h"
#include "dcf77.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

time_date_DataDigital TD_data = {0};
time_date_DataDigital TD_data_TEST = {0};

menu menu_position = menuTIME;
menu menu_position_old = menuTIME;

uint8_t submenu_pos = 0;

tubeDisplay nixieDisplay = {0};

DCF77_TimeTypeDef dcf_time;

volatile uint8_t tick_flag = isNotSet;
volatile uint8_t counter_seconds = 0;
volatile uint16_t tick_count = 0;

uint8_t recal_failed_cnt = 0;

/**
 * Value of the button being pressed.
 * 1: MINUS
 * 2: MENU
 * 3: PLUS
 */
volatile int8_t btn_flag_menu  = 0;
volatile int8_t btn_flag_plus  = 0;
volatile int8_t btn_flag_minus = 0;

uint8_t time_update_flag = 0;
volatile uint8_t timeDate_recal_flag = 0;
uint8_t out_of_calibration_flag = 0;

/**
 * @brief Flag to signal a change in the system via I/O or Timeout
 *  gets set whenever a button is pressed and should be reset when it is used inside a function, not everytime the loop repeats
 */
volatile uint8_t sys_update_flag = 0;

uint8_t btn_pressed_flag = isNotPressed;

//for when the sensor is being used
volatile uint8_t sensor_flag = 0;

uint8_t id_read = 0b0011;
//IDs for all available driver boards
uint8_t addon_dcf77 = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/**
 * @brief Callback for when the dcf77 driver completes a valid minute readout
 * @param time -> the time struct from dcf77 driver which contains the current time.
 */
void DCF77_MinuteCallback(DCF77_TimeTypeDef* time);

/**
 * @brief: Check for installed addons via ID pins D2 and D3 and sets corresponding flag
 * Addons:
 * 0 -> DCF77
 */
void check_for_addons(void);

/**
 * @brief Uses the time data from main time and date structure to give a 16 bit representation of the current time
 * @param: _time_date_data -> pointer to main time and date containing struct.
 * @retval Return 16 bit time data fit for direct outputting onto PORTB of the micro
 */
uint16_t set_tube_numbers_time(time_date_DataDigital* _time_date_data);

/**
 * @brief Uses the time data from main time and date structure to give a 16 bit representation of the current date
 * @param: _time_date_data -> pointer to main time and date containing struct.
 * @retval Return 16 bit date data fit for direct outputting onto PORTB of the micro
 */
uint16_t set_tube_numbers_date(time_date_DataDigital* _time_date_data);

//-----------------------------------------------------------Menu Stuff

/**
 * @brief: Dispalys the current time on the nixies. Nixie illumination is based upon start and stop times. 
 */
void menu_mainTime();

/**
 * @brief: Dispalys the current date on the nixies. Nixie illumination is based upon start and stop times.
 */
void menu_mainDate();

/**
 * @brief: TODO: Dispalys the current temperaturee (left) and humidity (right) on the nixies. Nixie illumination is based upon start and stop times.
 */
void menu_Sensor();

/**
 * @brief: When out of timeframe for on times, display current time. Should have a timeout of 10s
 */
void menu_peekTime();

/**
 * @brief: Lets one manually set start and stop times. There are 2 sets of Start/Stop times, one for the morning, one for the evening.
 * @param: _submenu_pos -> pointer to the global variable submenu_pos to cicle through the numbers.
 * @param: _Tdata_startStop -> pointer to main time containing struct.
 */
void menu_startStop(uint8_t* _submenu_pos, time_date_DataDigital* _Tdata_startStop);

/**
 * @brief: Lets one manually set the time. TODO: Gets disabled when DCF77 module is used.
 * @param: _submenu_pos -> pointer to the global variable submenu_pos to cicle through the numbers.
 * @param: _Tdata -> pointer to main time containing struct.
 */
void menu_timeSet(uint8_t* _submenu_pos, time_date_DataDigital* _Tdata);

/**
 * @brief: Timeout function to return to menuTIME after a set amount of seconds. Depends on the tick_count global variable (increased by RTC 1Hz out, reset every time a button is pressed via IRQ)
 * @param: _timeoutValue -> timeout value in seconds
 */
void menu_timeout(uint8_t _timeoutValue);

/**
 * @brief: Handles blinking of display elements
 * @retval: 1 or 0, alternating every BLINK_TIME
 */
uint8_t blinkState(void);

/**
 * @brief: Handles blinking of display elements
 * @retval: 1 or 0, alternating every _blink_time
 */
uint8_t blinkState_custom(uint16_t _blink_time);

/**
 * @brief Checks if current time is in the range of on hours. If no, turn display off. Has to be called after restoring backed up start stop data
 * @param _TD_data -> Main struct for time data to check for start and stop hours
 * @param _nixieDisplay -> Main struct for display data to change status state. 
 * @retval 0 if current state should be off, 1 if current state should be on.
 */
uint8_t startStop_check(time_date_DataDigital* _TD_data, tubeDisplay* _nixieDisplay);


//-----------------------------------------------------------Time Stuff

/**
 * @brief Checks for triggers of daylight saving time (last sunday in march or october ect.) and sets the RTC flags correspondingly
 * @param _DST      -> value from check_for_DST or 0 for summertime and 1 for wintertime. 2 for Error (does nothing)
 * @param hrtc      -> RTC Struct from HAL
 */
void set_for_DST(RTC_HandleTypeDef* hrtc, uint8_t _DST);

/**
 * @brief Checks for triggers of daylight saving time (last sunday in march or october ect.)
 * @param _TD_data  -> Main struct for time data
 * @param hrtc      -> RTC Struct from HAL
 * @retval 0 if sumemrtime trigger, 1 if wintertime trigger
 */
uint8_t check_for_DST(RTC_HandleTypeDef* hrtc, time_date_DataDigital* _TD_data);

//RTC related functions
/**
 * @brief reads the BKP Register DR2, which contains a 32bit combination of all start and stop times.
 * Puts them into TD_data main data structure in order
 * @param _TD_data main time structure containing all start and stop times
 */
void time_read_startStop_bkp  (time_date_DataDigital* _TD_data);

/**
 * @brief writes to the BKP Register DR2 from main time structure, which contains a 32bit combination of all start and stop times.
 * @param _TD_data main time structure containing all start and stop times
 */
void time_write_startStop_bkp (time_date_DataDigital* _TD_data);

//-----------------------------------------------------------DCF77 time calibration stuff

/**
 * @brief Check for the number of failed recalibration attempts in the RTC_BKUP_REG DR0 
 * @param _hrtc -> RTC handle
 * @retval Number of previous recalibration attempts (if 0, then rtc battery is dead)
 */
uint16_t check_for_reCalAttempts(RTC_HandleTypeDef* _hrtc);

/**
 * @brief ncrements the number of recalibration attempts by one in the bkp register DR0
 * @param _hrtc -> RTC handle
 * @retval Error status (0 = OK, 1 = ERROR)
 */
HAL_StatusTypeDef increment_reCalAttempts(RTC_HandleTypeDef* _hrtc);

/**
 * @brief sets the bkp register DR0 to 1. If it is 0 -> RTC battery is dead
 * @param _hrtc -> RTC handle
 * @retval Error status (0 = OK, 1 = ERROR)
 */
HAL_StatusTypeDef clear_reCalAttempts(RTC_HandleTypeDef* _hrtc);

/**
 * @brief Recalibrates the time and date from DCF77 module
 * @param _TD_data main time and date struct 
 * @param _timeout_ms timeout value in seconds after which an error is returned
 * @retval HAL_StatusTypeDef 0 = HAL_OK, 1 = HAL_ERROR
 */
HAL_StatusTypeDef time_recalibration(time_date_DataDigital* _TD_data, const uint16_t _timeout_s);

/**
 * @brief checks if recal attempt value is in acceptable borders
 * @param _hrtc -> RTC handle
 * @retval 0 -> in calibration, 1 -> out of calibration
 */
uint8_t check_for_calibration(RTC_HandleTypeDef* _hrtc);

/**
 * @brief: combines multiple dcf77 reclatibration functions in one. Recalibrates time and sets the bkp register dr0 correspondingly
 * @param: _TD_data main time and date struct 
 * @param: _timeout_ms timeout value in seconds after which an error is returned
 * @param: _hrtc -> RTC handle
 */
HAL_StatusTypeDef DCF77_TimeRecalibration(time_date_DataDigital* _TD_data, const uint16_t _timeout_s, RTC_HandleTypeDef* _hrtc);

//-----------------------------------------------------------Button Stuff

/**
 * @brief Resets all button flags at once
 */
void resetBtnFlags();

void handle_btn (menu* _pos);
void handle_btnPlus (menu* _pos);
void handle_btnMinus (menu* _pos);
void handle_btnMenu (menu* _pos);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim1);

  //Initialize the RTC
  HAL_RTC_Init(&hrtc);

  /**
   * If the random number stored in the backup register is diffrent from the current value, its not updating the Time and Date. 
   * Seed, Time and Date are stored in makros 
   */
   if(HAL_RTCEx_BKUPRead(&hrtc, RTC_SEED_BKP_REGISTER) != RANDOM_SEED_UPDATE) {
    setTime(CURRENT_TIME_HOURS, CURRENT_TIME_MINUTES, CURRENT_TIME_SECONDS);
    setDate(CURRENT_DATE_YEAR, CURRENT_DATE_MONTH, CURRENT_DATE_WEEKDAY, CURRENT_DATE_DAY);
  }

  //If cal wasn't successful for 3 days or rtc battery is dead -> out of calibration
  if(check_for_calibration(&hrtc)) out_of_calibration_flag = 1;
  
  //Check the D2 and D3 bits for addon boards. No board -> 0b11
  check_for_addons();

  //If module is found -> initiate it. When out of calibration -> calibrate it
  if(addon_dcf77) {
    DCF77_Init(&htim2);
    DCF77_RegisterCallback(DCF77_MinuteCallback);

    if(out_of_calibration_flag) DCF77_TimeRecalibration(&TD_data, 5*60, &hrtc);
  }

  /**
   * Retrieve time and date data from the running RTC
   * Try 10 times or till a HAL_OK is retrieved
   * Disable IRQ while doing this to prevent faulty time data
   */
  __disable_irq();
  
  if(getTimeDate(&TD_data) != HAL_OK) {
    HAL_StatusTypeDef _Status = HAL_ERROR;

    for(uint8_t tries = 0; tries < 10; tries++) {
      _Status = getTimeDate(&TD_data);
      if(_Status == HAL_OK) break;
    }
  }
  __enable_irq();

  //Set the Front LEDs On
  output_front_led(1, 1);

  //Check for daylight saving time
  set_for_DST(&hrtc, check_for_DST(&hrtc, &TD_data));

  //Retrieve start and stop time from RTC backup register and push them to the main time struct
  time_read_startStop_bkp(&TD_data);

  //Check if start stop times are reached and change the corresponding bit in the nixieDisplay struct
  startStop_check(&TD_data, &nixieDisplay);

  //Output current time to main time struct
  nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);

  //output time data to the display struct
  output_to_tubesNEW(&nixieDisplay);

  //Set the HT supply state to the corresponding state in display struct
  ht_supply_state(&nixieDisplay);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

    if(addon_dcf77) {
      //does something once a day at 2 (set by Alarm A interrupt handler)
      if(timeDate_recal_flag) {
        DCF77_TimeRecalibration(&TD_data, 20*60, &hrtc);
        timeDate_recal_flag = 0;
      }
    }

    switch(menu_position) {
      case menuTIME:
        menu_mainTime();
        break; 
      case menuDATE:
        menu_mainDate();
        menu_timeout(DISPLAY_MENU_TimeDateSensor_TIMEOUT);
        break;
      case menuSENSOR:
        menu_Sensor();
        menu_timeout(DISPLAY_MENU_TimeDateSensor_TIMEOUT);
        break;
      case menuPEEKTIME:
        menu_peekTime();
        menu_timeout(10);
        break;
      case menuStartStop:
        menu_startStop(&submenu_pos, &TD_data);
        menu_timeout(DISPLAY_MENU_X_TIMEOUT);
        break;
      case menuTimeEdit:
        if(out_of_calibration_flag || !addon_dcf77) {     //If clock is out of calibration OR dcf77 addon is not pluged in -> enable timeSet menu
          menu_timeSet(&submenu_pos, &TD_data);
          menu_timeout(DISPLAY_MENU_X_TIMEOUT);
          break;
        }
        menu_position++;
        break;
      case menuOVERFLOW:
        menu_position = menuTIME;
        break;
      default: 
        menu_position = menuTIME;
        break;
    }

    if((nixieDisplay.displayStatus != nixieDisplay.displayStatus_old) || (menu_position != menuTIME)) {
      if(menu_position != menuTIME) {
        nixieDisplay.displayStatus = 1;
        ht_supply_state(&nixieDisplay);
      } else {
        ht_supply_state(&nixieDisplay);
      }
      nixieDisplay.displayStatus_old = nixieDisplay.displayStatus;
    }

    if(menu_position != menuStartStop && menu_position != menuTimeEdit) {
      output_to_tubesNEW(&nixieDisplay);  //Updates the tube display only when it should be automatically updated
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10B17DB5;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  //RTC_TimeTypeDef sTime = {0};
  //RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
 /*
  sTime.Hours = 0x0;
  sTime.Minutes = 0x5;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_SEPTEMBER;
  sDate.Date = 0x19;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
    */

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x2;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable Calibration
  */
  if (HAL_RTCEx_SetCalibrationOutPut(&hrtc, RTC_CALIBOUTPUT_1HZ) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 1000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 48000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 64000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 15;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ht_EN_Pin|addon_en_Pin|pwr_led_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, co1_3_Pin|co1_2_Pin|co1_1_Pin|co1_0_Pin
                          |co0_3_Pin|co0_2_Pin|co0_1_Pin|co0_0_Pin
                          |co2_0_Pin|co2_1_Pin|co2_2_Pin|co2_3_Pin
                          |co3_0_Pin|co3_1_Pin|co3_2_Pin|co3_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, led_sig_bot_Pin|led_sig_top_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ht_EN_Pin */
  GPIO_InitStruct.Pin = ht_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ht_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : addon_en_Pin led_sig_bot_Pin led_sig_top_Pin pwr_led_Pin */
  GPIO_InitStruct.Pin = addon_en_Pin|led_sig_bot_Pin|led_sig_top_Pin|pwr_led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : btn_minus_Pin btn_menu_Pin btn_plus_Pin */
  GPIO_InitStruct.Pin = btn_minus_Pin|btn_menu_Pin|btn_plus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : co1_3_Pin co1_2_Pin co1_1_Pin co1_0_Pin
                           co0_3_Pin co0_2_Pin co0_1_Pin co0_0_Pin
                           co2_0_Pin co2_1_Pin co2_2_Pin co2_3_Pin
                           co3_0_Pin co3_1_Pin co3_2_Pin co3_3_Pin */
  GPIO_InitStruct.Pin = co1_3_Pin|co1_2_Pin|co1_1_Pin|co1_0_Pin
                          |co0_3_Pin|co0_2_Pin|co0_1_Pin|co0_0_Pin
                          |co2_0_Pin|co2_1_Pin|co2_2_Pin|co2_3_Pin
                          |co3_0_Pin|co3_1_Pin|co3_2_Pin|co3_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : id_bit0_Pin id_bit1_Pin */
  GPIO_InitStruct.Pin = id_bit0_Pin|id_bit1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minute, uint8_t second) {
  RTC_TimeTypeDef sTime = {0};
  sTime.Hours = hour;
  sTime.Minutes = minute;
  sTime.Seconds = second;
  sTime.SubSeconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }
  //HAL_RTC_DST_Sub1Hour(&hrtc);  // TODO Daylight Saving Time,check for last sunday of march and last sunday in october to set opr unset DST. 
  return HAL_OK;
}

HAL_StatusTypeDef setDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date) { //weekday->Monday = 1, date->which day in month (0-31) 
  RTC_DateTypeDef sDate = {0};
  sDate.WeekDay = weekday;
  sDate.Month = month;
  sDate.Date = date;
  sDate.Year = year;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RANDOM_SEED_UPDATE);
  
  return HAL_OK;
}

HAL_StatusTypeDef getTimeDate(time_date_DataDigital* dTimeDate) {
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;
  uint8_t _error_count = 0;

  //Get current time
  if(HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN) != HAL_OK) _error_count++;

  //Get current date
  if(HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN) != HAL_OK) _error_count++;

  dTimeDate->hours = gTime.Hours;
  dTimeDate->minutes = gTime.Minutes;
  dTimeDate->seconds = gTime.Seconds;

  dTimeDate->day = gDate.Date;
  dTimeDate->weekday = gDate.WeekDay;
  dTimeDate->month = gDate.Month;
  dTimeDate->year = gDate.Year;

  if(_error_count != 0) return HAL_ERROR;
  return HAL_OK;
}

uint16_t set_tube_numbers_time(time_date_DataDigital* _time_date_data) {

  //Start by extracting the single numbers out of the time struct to fuse them to a 16-Bit number later

  //Hours
  //calculate the tens
  uint8_t hours_tens = _time_date_data->hours / 10;
  //calculate the ones
  uint8_t hours_ones = _time_date_data->hours % 10;

  //Minutes
  //calculate the tens
  uint8_t minutes_tens = _time_date_data->minutes / 10;
  //calculate the ones
  uint8_t minutes_ones = _time_date_data->minutes % 10;

  return(combine_4bit_numbers(hours_tens, hours_ones, minutes_tens, minutes_ones));
}

uint16_t set_tube_numbers_date(time_date_DataDigital* _time_date_data) {

  //Start by extracting the single numbers out of the time struct to fuse them to a 16-Bit number later

  //Hours
  //calculate the tens
  uint8_t month_tens = _time_date_data->month / 10;
  //calculate the ones
  uint8_t month_ones = _time_date_data->month % 10;

  //Minutes
  //calculate the tens
  uint8_t day_tens = _time_date_data->day / 10;
  //calculate the ones
  uint8_t day_ones = _time_date_data->day % 10;

  /*Works miracilously perfectly :) but useless unless the PCB changes to accomondate for the correct pin order
  uint16_t _timeData_port = (hours_tens << 12) | (hours_ones << 8) | 
                            (minutes_tens << 4) | minutes_ones;
  */

  return(combine_4bit_numbers(day_tens, day_ones, month_tens, month_ones));
}

void menu_mainTime() {
  output_blink_front_leds(solid);

  if(sys_update_flag) {
    getTimeDate(&TD_data);
    nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);

    startStop_check(&TD_data, &nixieDisplay);

    set_for_DST(&hrtc, check_for_DST(&hrtc, &TD_data));

    sys_update_flag = 0;
  }

  if(tick_flag == isSet) { //flag set by interrupt by RTC on 1Hz

    getTimeDate(&TD_data);      //Get time from RTC registers

    //Update display data, nixies dont have seconds!
    if(TD_data.seconds == 0) {
      nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);
      set_for_DST(&hrtc, check_for_DST(&hrtc, &TD_data));
    }

    startStop_check(&TD_data, &nixieDisplay);

    tick_flag = reset; //reset update flag
  }

  handle_btn(&menu_position);
}

void menu_mainDate() {
  output_blink_front_leds(solidBot);

  if(sys_update_flag) {
    getTimeDate(&TD_data);
    nixieDisplay.displayDigitOutput = set_tube_numbers_date(&TD_data);

    sys_update_flag = 0;
  }

  if(tick_flag == isSet) {
    
    getTimeDate(&TD_data);

    nixieDisplay.displayDigitOutput = set_tube_numbers_date(&TD_data);

    tick_flag = reset;
  }

  handle_btn(&menu_position);
}

void menu_Sensor() {
  output_blink_front_leds(blinkBoth);

  if(sys_update_flag) {
    nixieDisplay.displayDigitOutput = combine_4bit_numbers(9, BLANK, 9, BLANK); //PLACEHOLDER

    nixieDisplay.displayStatus = 1;

    if(nixieDisplay.displayStatus != nixieDisplay.displayStatus_old) {
      ht_supply_state(&nixieDisplay);
      nixieDisplay.displayStatus_old = nixieDisplay.displayStatus;
    }

    sys_update_flag = 0;
  }

  if(tick_flag == isSet) {

    tick_flag = reset;
  }

  handle_btn(&menu_position);
}

void menu_startStop(uint8_t* _submenu_pos, time_date_DataDigital* _Tdata_startStop) {

  output_blink_front_leds(blinkTop);

  static uint8_t temp_time;

  //if somethings need to happen only once getting into this menu
  if(menu_position_old != menu_position) {
    *_submenu_pos = 0;
    temp_time = _Tdata_startStop->startHour1;
    menu_position_old = menu_position;
  }

  //handle_btn(&menu_position);

  if(btn_flag_plus) {
    temp_time = (temp_time + 1) % 24;
    btn_flag_plus = 0;
  }

  if(btn_flag_minus) {
    temp_time = (temp_time + 23) % 24;
    btn_flag_minus = 0;
  }

  if(btn_flag_menu) {
    switch(*_submenu_pos) {
      case 0:
        _Tdata_startStop->startHour1 = temp_time;
        temp_time = _Tdata_startStop->stopHour1;
        (*_submenu_pos)++;
        break;
      case 1:
        _Tdata_startStop->stopHour1 = temp_time;
        temp_time = _Tdata_startStop->startHour2;
        (*_submenu_pos)++;
        break;
      case 2:
        _Tdata_startStop->startHour2 = temp_time;
        temp_time = _Tdata_startStop->stopHour2;
        (*_submenu_pos)++;
        break;
      case 3:
        _Tdata_startStop->stopHour2 = temp_time;
        *_submenu_pos = 0;
        time_write_startStop_bkp(_Tdata_startStop);
        menu_position++;
        break;
    }
    btn_flag_menu = 0;
  }

  output_to_tubes(combine_4bit_numbers((temp_time/10), (temp_time%10), 0, 0));
}

void menu_timeSet(uint8_t* _submenu_pos, time_date_DataDigital* _Tdata) {  

  output_blink_front_leds(blinkBot);

  static uint8_t temp_time;
  static uint8_t temp_time_combine;

  //if somethings need to happen only once getting into this menu
  if(menu_position_old != menu_position) {
    *_submenu_pos = 0;
    temp_time = (_Tdata->hours / 10);
    menu_position_old = menu_position;
  }

  if(btn_flag_plus) {
    switch (*_submenu_pos) {
      case 0:
        temp_time = (temp_time + 1) % 3;
        break;
      case 1: 
        if (temp_time == 3 && _Tdata->hours / 10 == 2) temp_time = 0;  // 23 -> 20, to prevent non possible hour values like 25, 26 ect.
        else temp_time = (temp_time + 1) % 10;
        break;
      case 2: 
        temp_time = (temp_time + 1) % 6;
        break;
      case 3:
        temp_time = (temp_time + 1) % 10;
        break;
    }
    btn_flag_plus = 0;
  }

  if(btn_flag_minus) {
    switch (*_submenu_pos) {
      case 0:
        temp_time = (temp_time + 2) % 3;
        break;
      case 1: 
        temp_time = (temp_time + 9) % 10;
        break;
      case 2: 
        temp_time = (temp_time + 5) % 6;
        break;
      case 3:
        temp_time = (temp_time + 9) % 10;
        break;
    }
    btn_flag_minus = 0;
  }

  switch(*_submenu_pos) {
    case 0:
      if(btn_flag_menu) {
        temp_time_combine = temp_time * 10;
        temp_time = _Tdata->hours % 10;
        (*_submenu_pos)++;
        btn_flag_menu = 0;
      }

      if(blinkState_custom(500)) output_to_tubes(combine_4bit_numbers(temp_time, _Tdata->hours % 10, _Tdata->minutes / 10, _Tdata->minutes % 10));
      else output_to_tubes(combine_4bit_numbers(BLANK, _Tdata->hours % 10, _Tdata->minutes / 10, _Tdata->minutes % 10));

      break;

    case 1:
      if(btn_flag_menu) {
        temp_time_combine = temp_time_combine + temp_time;
        _Tdata->hours = temp_time_combine;
        temp_time_combine = 0;
        temp_time = _Tdata->minutes / 10;
        (*_submenu_pos)++;
        btn_flag_menu = 0;
      }

      if(blinkState_custom(500)) output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, temp_time, _Tdata->minutes / 10, _Tdata->minutes % 10));
      else output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, BLANK, _Tdata->minutes / 10, _Tdata->minutes % 10));

      break;

    case 2:
      if(btn_flag_menu) {
        temp_time_combine = temp_time * 10;
        temp_time = _Tdata->minutes % 10;
        (*_submenu_pos)++;
        btn_flag_menu = 0;
      }

      if(blinkState_custom(500)) output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, _Tdata->hours % 10, temp_time, _Tdata->minutes % 10));
      else output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, _Tdata->hours % 10, BLANK, _Tdata->minutes % 10));

      break;

    case 3:
      if(btn_flag_menu) {
        temp_time_combine = temp_time_combine + temp_time;
        _Tdata->minutes = temp_time_combine;
        temp_time_combine = 0;

        setTime(_Tdata->hours, _Tdata->minutes, 0);

        *_submenu_pos = 0;
        out_of_calibration_flag = 0;
        menu_position++;
        btn_flag_menu = 0;
      }

      if(blinkState_custom(500)) output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, _Tdata->hours % 10, _Tdata->minutes / 10, temp_time));
      else output_to_tubes(combine_4bit_numbers(_Tdata->hours / 10, _Tdata->hours % 10, _Tdata->minutes / 10, BLANK));

      break;
  }
}

void menu_peekTime() {
  output_blink_front_leds(solid);

  if(sys_update_flag) {
    getTimeDate(&TD_data);
    nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);

    menu_position_old = menuPEEKTIME;

    nixieDisplay.displayStatus = 1;
    sys_update_flag = 0;
  }

  if(tick_flag == isSet) { //flag set by interrupt by RTC on 1Hz

    getTimeDate(&TD_data);      //Get time from RTC registers

    //Update display data, nixies dont have seconds!
    if(TD_data.seconds == 0) {
      nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);
    }

    tick_flag = reset; //reset update flag
  }

  if(btn_flag_menu)  {
    menu_position = menuStartStop;
    sys_update_flag = 1;
  }  
  if(btn_flag_plus)  {
    menu_position = menuDATE;
    sys_update_flag = 1;
  }  
  if(btn_flag_minus) {
    menu_position = menuSENSOR;
    sys_update_flag = 1;
  }  

  resetBtnFlags();  
}

void handle_btn (menu* _pos) {
  if(btn_flag_plus)  handle_btnPlus(_pos);
  if(btn_flag_minus) handle_btnMinus(_pos);
  if(btn_flag_menu)  handle_btnMenu(_pos);
}

void handle_btnPlus (menu* _pos) {
  switch (*_pos) {
    case (menuTIME):
      menu_position = menuDATE;
      sys_update_flag = 1;
      break;
    case (menuDATE):
      menu_position = menuSENSOR;
      sys_update_flag = 1;
      break;
    case (menuSENSOR):
      menu_position = menuTIME;
      sys_update_flag = 1;
      break;
    case(menuStartStop):

      break;
    case(menuTimeEdit):

      break;
    default:
      break;
  }
  btn_flag_plus = 0;
}

void handle_btnMinus (menu* _pos) {
  switch (*_pos) {
    case (menuTIME):
      menu_position = menuSENSOR;
      sys_update_flag = 1;
      break;
    case (menuDATE):
      menu_position = menuTIME;
      sys_update_flag = 1;
      break;
    case (menuSENSOR):
      menu_position = menuDATE;
      sys_update_flag = 1;
      break;
    case(menuStartStop):

      break;
    case(menuTimeEdit):

      break;
    default:
      break;
  }
  btn_flag_minus = 0;
}

void handle_btnMenu (menu* _pos) {
  switch (*_pos) {
    case (menuTIME):
      if(TD_data.in_timeframe_startStop) {
        menu_position = menuStartStop;
      } else if (!TD_data.in_timeframe_startStop) {
        menu_position = menuPEEKTIME;
        sys_update_flag = 1;
      }
      menu_position_old = menuTIME;
      break;
    case (menuDATE):

      break;
    case (menuSENSOR):

      break;
    case(menuStartStop):
      if(submenu_pos < 4) submenu_pos++; 
      else {
        submenu_pos = 0;
        menu_position++;
        menu_position_old = menuStartStop;
      }
      break;
    case(menuTimeEdit):
      if(submenu_pos < 4) submenu_pos++;
      else {
        submenu_pos = 0;
        menu_position++;
        menu_position_old = menuTimeEdit;
      }
      break;
    case(menuOVERFLOW):
      menu_position = menuTIME;
      break;
    default:
      break;
  }
  btn_flag_menu = 0;
}

void menu_timeout(uint8_t _timeoutValue) {

  if(tick_count >= _timeoutValue) {
    menu_position = menuTIME;
    sys_update_flag = 1;
  }

  time_update_flag = 1;
}

uint8_t startStop_check(time_date_DataDigital* _TD_data, tubeDisplay* _nixieDisplay) {

  //Overwrite to disable start stop automatics (if all times are set to 0)
  if((_TD_data->startHour1 == 0) && (_TD_data->stopHour1 == 0) && (_TD_data->startHour2 == 0) && (_TD_data->stopHour2 == 0)) {
    _nixieDisplay->displayStatus = 1;
    return 1;
  }

  uint8_t _tempHour = _TD_data->hours;

  //Weekend routine (first start hour and second stop hour)
  if((_TD_data->weekday == saturday) || (_TD_data->weekday == sunday)) {
    uint8_t in_timeframe  = _TD_data->startHour1 < _TD_data->stopHour2
                          ? (_tempHour >= _TD_data->startHour1 && _tempHour < _TD_data->stopHour2)
                          : (_tempHour >= _TD_data->startHour1 || _tempHour < _TD_data->stopHour2);
    if(in_timeframe) {
      _nixieDisplay->displayStatus = 1;
      _TD_data->in_timeframe_startStop = 1;
      return 1;
    } else {
      _nixieDisplay->displayStatus = 0;
      _TD_data->in_timeframe_startStop = 0;
      return 0;
    }
  }

  uint8_t in_timeframe1 = (_TD_data->startHour1 < _TD_data->stopHour1)
                        ? (_tempHour >= _TD_data->startHour1 && _tempHour < _TD_data->stopHour1)    //If normal daytime hours
                        : (_tempHour >= _TD_data->startHour1 || _tempHour < _TD_data->stopHour1);   //If wrap around over midnight
  uint8_t in_timeframe2 = (_TD_data->startHour2 < _TD_data->stopHour2) 
                        ? (_tempHour >= _TD_data->startHour2 && _tempHour < _TD_data->stopHour2)    //If normal daytime hours
                        : (_tempHour >= _TD_data->startHour2 || _tempHour < _TD_data->stopHour2);   //If wrap around over midnight

  if(in_timeframe1 || in_timeframe2) {
    _nixieDisplay->displayStatus = 1;
    _TD_data->in_timeframe_startStop = 1;
    return 1;
  } else {
    _nixieDisplay->displayStatus = 0;
    _TD_data->in_timeframe_startStop = 0;
    return 0;
  }
}

void check_for_addons(void) {
    
  uint8_t addon_id = ((HAL_GPIO_ReadPin(id_bit0_GPIO_Port, id_bit0_Pin) << 1) | (HAL_GPIO_ReadPin(id_bit1_GPIO_Port, id_bit1_Pin)));

  switch(addon_id) {
  case 0b0000: 
      addon_dcf77 = 1;
      break;
  case 0b0001: 
      break;
  default:
      break;
  }
}

void resetBtnFlags() {

  btn_flag_menu =  reset;
  btn_flag_minus = reset;
  btn_flag_plus =  reset; 

  btn_pressed_flag = reset;
}

void time_read_startStop_bkp (time_date_DataDigital* _TD_data) {

    uint32_t temp_read = HAL_RTCEx_BKUPRead(&hrtc, RTC_START_STOP_BKP_REGISTER);

    _TD_data->startHour1 = ((temp_read >> 24) & 0xFF);
    _TD_data->stopHour1  = ((temp_read >> 16) & 0xFF);
    _TD_data->startHour2 = ((temp_read >> 8)  & 0xFF);
    _TD_data->stopHour2  = ((temp_read >> 0)  & 0xFF);
}

void time_write_startStop_bkp (time_date_DataDigital* _TD_data) {

    uint32_t temp_write = ((uint32_t)_TD_data->startHour1 << 24)
                        | ((uint32_t)_TD_data->stopHour1 << 16)
                        | ((uint32_t)_TD_data->startHour2 <<  8)
                        |  (uint32_t)_TD_data->stopHour2;

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_START_STOP_BKP_REGISTER, temp_write);
}

uint8_t check_for_calibration(RTC_HandleTypeDef* _hrtc) {
  if(HAL_RTCEx_BKUPRead(_hrtc, RTC_RECAL_BKP_REGISTER) == 0 || HAL_RTCEx_BKUPRead(_hrtc, RTC_RECAL_BKP_REGISTER) >3) return 1;
  return 0;
}

uint16_t check_for_reCalAttempts(RTC_HandleTypeDef* _hrtc) {

  return (HAL_RTCEx_BKUPRead(_hrtc, RTC_RECAL_BKP_REGISTER));
}

HAL_StatusTypeDef increment_reCalAttempts(RTC_HandleTypeDef* _hrtc) {

  uint32_t _prevRecalAttempts = HAL_RTCEx_BKUPRead(_hrtc, RTC_RECAL_BKP_REGISTER);
  _prevRecalAttempts++;
  HAL_RTCEx_BKUPWrite(_hrtc, RTC_RECAL_BKP_REGISTER, _prevRecalAttempts);
  
  return 0;
}

HAL_StatusTypeDef clear_reCalAttempts(RTC_HandleTypeDef* _hrtc) {

  //Set to one to differentiate between battery charge loss and restart from RTC battery
  HAL_RTCEx_BKUPWrite(_hrtc, RTC_RECAL_BKP_REGISTER, 1);

  return 0;
}

void set_for_DST(RTC_HandleTypeDef* hrtc, uint8_t _DST) {
  if(!_DST) {
    HAL_RTC_DST_ClearStoreOperation(hrtc);
    HAL_RTC_DST_Add1Hour(hrtc);
    return;

  } else if (_DST == 1) {
    HAL_RTC_DST_SetStoreOperation(hrtc);
    HAL_RTC_DST_Sub1Hour(hrtc);
    return;

  } else if (_DST == 2) {
    return;
  }
}

uint8_t check_for_DST(RTC_HandleTypeDef* hrtc, time_date_DataDigital* _TD_data) {
  if((_TD_data->month == 3) && (_TD_data->weekday == sunday) && ((_TD_data->day + 7) > 31) && (HAL_RTC_DST_ReadStoreOperation(hrtc)) && (_TD_data->hours >= 2)) return 0;         //summertime
  else if((_TD_data->month == 10) && (_TD_data->weekday == sunday) && ((_TD_data->day + 7) > 31) && (!HAL_RTC_DST_ReadStoreOperation(hrtc)) && (_TD_data->hours >= 3)) return 1;  //wintertime
  return 2;
}

HAL_StatusTypeDef time_recalibration(time_date_DataDigital* _TD_data, const uint16_t _timeout_s) {
  
  //If the supply is on -> Error (EMF reasons)
  if(HAL_GPIO_ReadPin(ht_EN_GPIO_Port, ht_EN_Pin) == GPIO_PIN_RESET) return 1;

  DCF77_Start();

  uint32_t _start = HAL_GetTick();

  while(!DCF77_IsDataValid()) {
    if((HAL_GetTick() - _start) >= (_timeout_s * 1000)) {
      DCF77_Stop();   // Can't be that bad, but should already be stopped in the callback funtion DCF77_MinuteCallback
      return 1;
    } 
    HAL_Delay(100);
  }

  //getting time from dcf77 successful -> set clock time

  DCF77_TimeTypeDef _dcf_time = DCF77_GetTime();

  _TD_data->hours     = _dcf_time.hour;
  _TD_data->minutes   = _dcf_time.minute;
  _TD_data->seconds   = 0;
  _TD_data->day       = _dcf_time.day;
  _TD_data->month     = _dcf_time.month;
  _TD_data->year      = _dcf_time.year;
  _TD_data->weekday   = _dcf_time.weekday;

  if(setTime(_TD_data->hours, _TD_data->minutes, 0)) return 1;

  if(setDate(_TD_data->year, _TD_data->month, _TD_data->weekday, _TD_data->day)) return 1;



  return 0;
}

HAL_StatusTypeDef DCF77_TimeRecalibration(time_date_DataDigital* _TD_data, const uint16_t _timeout_s, RTC_HandleTypeDef* _hrtc) {
  if(!time_recalibration(_TD_data, _timeout_s)) {
    clear_reCalAttempts(_hrtc);
    out_of_calibration_flag = 0;
    return 0;

  } else {
    if(!(check_for_reCalAttempts(_hrtc) == 0 || check_for_reCalAttempts(_hrtc) > 3)) {
      increment_reCalAttempts(_hrtc);
      out_of_calibration_flag = 0;
      return 1;
    }

    out_of_calibration_flag = 1;
    return 1;
  }
}

void DCF77_MinuteCallback(DCF77_TimeTypeDef *time) {

  //Recieved valid signal
  dcf_time.parity_ok = time->parity_ok;

  DCF77_Stop();   // Modul aus (EN HIGH), spart Strom
}

void RTC_Alarm_IRQHandler(void) {
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

//Every Night at 2 set the recal flag
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  if(addon_dcf77) timeDate_recal_flag = 1;
}

//Interrupt for triggering an update event every second
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
  tick_flag = set;

  if(menu_position != menuTIME) tick_count++;

  //counter_seconds++;
}

/**
 * Interrupt Handler for the buttons
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  switch(GPIO_Pin) {
    case btn_plus_Pin:
      btn_flag_plus = 1;

      tick_count = 0;
      break;
    
    case btn_minus_Pin:
      btn_flag_minus = 1;

      tick_count = 0;
      break;

    case btn_menu_Pin:
      btn_flag_menu = 1;

      tick_count = 0;
      break;

    default:
      break;
  }
  btn_pressed_flag = 1;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
