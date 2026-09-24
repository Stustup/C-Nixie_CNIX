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

 /** TO DO LIST
  * //TODO: Implement indicator LEDS on the front and their function
  * //TODO: Implement manual time setting via buttons
  * //TODO: Implement On/Off automatic
  * //TODO: Implement ID system for submodules
  * //TODO: Implement DCF77 Code
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
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "output_tube.h"
#include "menu.h"
#include "display.h"
#include "timeStuff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define DISPLAY_TIME_DATE_X_OFFSET 42
#define DISPLAY_TIME_Y_OFFSET       0
#define DISPLAY_DATE_Y_OFFSET      11
#define DISPLAY_MISC_Y_OFFSET      22
#define DISPLAY_MISC_X_OFFSET     112

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

time_date_DataDigital TD_data;

volatile menu menu_position = menuTIME;
menu menu_position_old = menuTIME;

tubeDisplay nixieDisplay;

volatile uint8_t tick_flag = isNotSet;
volatile uint8_t counter_seconds = 0;
volatile uint16_t tick_count = 0;

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

uint8_t btn_pressed_flag = isNotPressed;

//for when the sensor is being used
volatile uint8_t sensor_flag = 0;

uint8_t id_read = 0b0011;
//IDs for all available driver boards
uint8_t addon_dcf77 = 0;

char timeData[15];
char dateData[15];
char miscData[2];


void check_for_addons(void);

uint16_t set_tube_numbers_time(time_date_DataDigital* _time_date_data);
uint16_t set_tube_numbers_date(time_date_DataDigital* _time_date_data);
//uint16_t combine_4bit_numbers(uint8_t num0, uint8_t num1, uint8_t num2, uint8_t num3);
//void output_to_tubes(uint16_t _data);

//void output_front_led(uint8_t led0, uint8_t led1);
//void output_blink_front_leds(blink_mode _mode);

/**
 * @brief: Main menu structure for the whole clock
 * @param 0 -> normal time display
 * @param 1 -> Temperature and humidity sensor (Timeout 5s)
 * @param 2 -> Start/stop Time (time setting like above, but for start AND stop time) (Maybe multiple ones for morning and evening times?) (Timeout 30s) (Override with long press till next shutoff?)
 * @param 3 -> Time set (GETS DISABLED WHEN DCF77 PLUGIN BOARD IS USED) (1. hours tens; 2. hours ones; 3. minutes tens; 4. minutes ones) (Timeout 30s)
 */
uint8_t menu_pos = 0;
uint8_t submenu_pos = 0;
uint8_t menu_pos_old = 0;
uint8_t menu_0_submenu_flag = 0;

uint16_t menu_time_set[4] = {1010,110,101,11};

//Menu subfunctions
void menu_mainTime();
void menu_mainDate();
void menu_Sensor();
void menu_startStop(uint8_t _submenu_pos, time_date_DataDigital* _Tdata_start);
void menu_timeSet(uint8_t _submenu_pos);
void menu_timeout(uint8_t _timeoutValue);
uint8_t blinkState(void);
void resetBtnFlags();

/**
 * main counter for getTick function
 */
uint32_t start_ms_counter = 0;
uint32_t start_ms_counter_blink = 0;
uint32_t start_ms_counter_menuTimeout = 0;
uint32_t start_ms_counter_menuTimeout_DATE = 0;
uint8_t  error_count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

#if DEBUG_DISPLAY
void ssd1306_writeTime(char* time);
void ssd1306_writeDate(char* date);
void ssd1306_writeTimeDate(char* time, char* date);
void ssd1306_writeMisc(char* _data);
#endif

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
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim1);

  //Initialize the RTC
  HAL_RTC_Init(&hrtc);

  /**
   * If the random number stored in the backup register is diffrent from the current value, its not updating the Time and Date. 
   * Seed, Time and Date are stored in makros 
   */
   if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RANDOM_SEED_UPDATE) {
    setTime(CURRENT_TIME_HOURS, CURRENT_TIME_MINUTES, CURRENT_TIME_SECONDS);
    setDate(CURRENT_DATE_YEAR, CURRENT_DATE_MONTH, CURRENT_DATE_WEEKDAY, CURRENT_DATE_DAY);
  }
  
  
  /**
   * Retrieve time and date data from the running RTC
   * Try 10 times or till a HAL_OK is retrieved
   * Disable IRQ while doing this to prevent faulty time data
   */
  __disable_irq();
  
  if(getTimeDate(timeData, dateData, &TD_data) != HAL_OK) {
    HAL_StatusTypeDef _Status = HAL_ERROR;

    for(uint8_t tries = 0; tries < 10; tries++) {
      _Status = getTimeDate(timeData, dateData, &TD_data);
      if(_Status == HAL_OK) break;
    }
  }
  __enable_irq();


  //Set the Front LEDs On
  output_front_led(1, 1);

  //Output current time to tubes TODO: CHECK FOR WEEKEND MODE AND START STOP TIME.
  nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);
  output_to_tubesNEW(&nixieDisplay);
  
  miscData[0] = '0';

  #if DEBUG_DISPLAY
  /**
   * Constructor for the basic oled menu with predeceeding welcome message
   * // TODO LATER create a module check and modules (e.g. WIFI time sync, smart home features, etc.)
   */
  
  ssd1306_Init();
  ssd1306_SetCursor(10, 10);
  ssd1306_WriteString("Nixie Clock V2", Font_7x10, White);
  ssd1306_UpdateScreen();
  HAL_Delay(500);
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Time: ", Font_7x10, White);
  ssd1306_writeTime(timeData);
  ssd1306_SetCursor(0, 11);
  ssd1306_WriteString("Date: ", Font_7x10, White);
  ssd1306_writeDate(dateData);
  ssd1306_SetCursor(0, 22);
  ssd1306_WriteString("Menu Position: ", Font_7x10, White);
  ssd1306_writeMisc(miscData);
  ssd1306_UpdateScreen();
  #endif

  check_for_addons();

  //Turn On HT PSU
  ht_supply_state(&nixieDisplay);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    #if DEBUG_DISPLAY
    /**
     * Reset button number after some time (defined in DISPLAY_MENU_RESET_TIME)
     */
    if(btn_pressed_flag && ((HAL_GetTick()-start_ms_counter) > DISPLAY_MENU_RESET_TIME)) {

      miscData[0] = '0';
      ssd1306_writeMisc(miscData);
      btn_pressed_flag = isNotPressed;
    }
    #endif

    //only start one time per menu change
    /*
    if(menu_pos != menu_pos_old)  { 
      start_ms_counter_menuTimeout = HAL_GetTick();
      menu_pos_old = menu_pos;
    }
    */

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
      case menuStartStop:
        menu_startStop(submenu_pos, &TD_data);
        menu_timeout(DISPLAY_MENU_X_TIMEOUT);
        break;
      case menuTimeEdit:
        menu_timeSet(submenu_pos);
        menu_timeout(DISPLAY_MENU_X_TIMEOUT);
        break;
      case menuOVERFLOW:
        menu_position = menuTIME;
        break;
      default: 
        menu_position = menuTIME;
        break;
    }

   

    if(nixieDisplay.displayStatus != nixieDisplay.displayStatus_old) {
      ht_supply_state(&nixieDisplay);
      nixieDisplay.displayStatus_old = nixieDisplay.displayStatus;
    }

    output_to_tubesNEW(&nixieDisplay);  //Updates the tube display

    if(btn_pressed_flag) {
      resetBtnFlags();
    }

    /*
    switch(menu_pos) {
      case 0: //Main time display menu, always is the fallback for timeouts!
        output_blink_front_leds(solid);
        
        //IDEA: When pushing plus or minus button: show date! Realized with submenu condition.
        if(btn_flag_minus || btn_flag_plus || menu_0_submenu_flag) {
          if(!menu_0_submenu_flag) {
            start_ms_counter_menuTimeout_DATE = HAL_GetTick();
            menu_0_submenu_flag = 1;
          }
          
          menu_mainDate();

          if(HAL_GetTick() > start_ms_counter_menuTimeout_DATE+5000) {
            menu_0_submenu_flag = 0;
          }
          break;
        }

        if(!menu_0_submenu_flag) {
          menu_mainTime();
        }
        
        break;

      case 1: //Display Temp and Hmd on nixie tubes. Left Temp, right hmd. TIMEOUT = 5s
        if(!addon_dcf77) {
          menu_pos++;
          break;  //Skips this menu when no DCF77 addon board is connected
        }
        
        menu_timeout(1);
        menu_Sensor();
        output_blink_front_leds(blinkBoth);

        break; 

      case 2: case 4: //Set start time of the clock. Can be set multiple times for a morning and evening routine TIMEOUT = 30s         
        menu_timeout(2);

        if(menu_pos == 2) menu_startStop(submenu_pos, &TD_data);
        if(menu_pos == 4) menu_startStop(submenu_pos, &TD_data);

        output_blink_front_leds(blinkTop);
        
        break;  

      case 3: case 5: //Set stop time of the clock.
        menu_timeout(2);

        if(menu_pos == 3) menu_startStop(submenu_pos, &TD_data);
        if(menu_pos == 5) menu_startStop(submenu_pos, &TD_data);

        output_blink_front_leds(blinkBot);

        break;

      case 6: case 7: //Set the time manually. must be disabled when module '00' is used (DCF77) TIMEOUT = 30s   
        menu_timeout(3);
        menu_timeSet(submenu_pos);
        output_blink_front_leds(blinkBoth);
        break;

      default: break;
    }
    */
    #if DEBUG_BOARD
    sprintf(miscData, "%01d", menu_pos);
    #endif

    #if DEBUG_DISPLAY
    ssd1306_writeMisc(miscData);    
    #endif
    
    /**
     * Debug function for development of the buttons
     */
    /*
    if((btn_flag_plus == isPressed) || (btn_flag_menu == isPressed) || (btn_flag_minus == isPressed)) {

      if(btn_flag_menu) {
        miscData[0] = 'M';
        ssd1306_writeMisc(miscData);
      }
      if(btn_flag_plus) {
        miscData[0] = '+';
        ssd1306_writeMisc(miscData);
      }
      if(btn_flag_minus) {
        miscData[0] = '-';
        ssd1306_writeMisc(miscData);
      }
      */
      
      
      /*Turns on the HT PSU
      switch (btn_flag) {
        case 2:

          HAL_GPIO_TogglePin(ht_EN_GPIO_Port, ht_EN_Pin);
          break;

        default:
          break;
      } */

      //start_ms_counter = HAL_GetTick();
      //btn_pressed_flag = isPressed;
      /*
      btn_flag_menu =  reset;
      btn_flag_minus = reset;
      btn_flag_plus =  reset;
    }
    */
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
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
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
  HAL_GPIO_WritePin(GPIOA, ht_EN_Pin|pwr_led_Pin, GPIO_PIN_SET);

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

  /*Configure GPIO pins : addon_data_Pin addon_en_Pin */
  GPIO_InitStruct.Pin = addon_data_Pin|addon_en_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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

  /*Configure GPIO pins : led_sig_bot_Pin led_sig_top_Pin pwr_led_Pin */
  GPIO_InitStruct.Pin = led_sig_bot_Pin|led_sig_top_Pin|pwr_led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

/**
 * Sets the Time of the RTC. IMPORTANT: When regenerating the code through CubeMX comment out the time setting in the predefined function.
 * This is to only update the time when needed and not every time you program the MCU. The check is made through the Backup register DR1, to which a 
 * random number is written. Only update the time when this number is not the same on startup!
 */
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

/**
 * Sets the Date of the RTC. IMPORTANT: When regenerating the code through CubeMX comment out the date setting in the predefined function.
 * This is to only update the date when needed and not every time you program the MCU. The check is made through the Backup register DR1, to which a 
 * random number is written. Only update the Date when this number is not the same on startup!
 */
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

/**
 * Function to get time and date from RTC module. IMPORTANT: Always get time and then date TOGETHER! otherwize the druids of the forest will hunt you
 * Creates strings in predefined vhar arrays to directly print to an oled.
 * TODO: put time and date in an integer struct to push to the nixies 
 */
HAL_StatusTypeDef getTimeDate(char* time, char* date, time_date_DataDigital* dTimeDate) {
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

  #if DEBUG_DISPLAY
  /* Display time Format: hh:mm:ss */
  sprintf(time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);

  /* Display date Format: dd-mm-yyyy */
  sprintf(date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);
  #endif

  if(_error_count != 0) return HAL_ERROR;
  return HAL_OK;
}

#if DEBUG_DISPLAY
/**
 * Combined oled writing function
 */
void ssd1306_writeTimeDate(char* time, char* date) {
  ssd1306_SetCursor(DISPLAY_TIME_DATE_X_OFFSET, DISPLAY_TIME_Y_OFFSET);
  ssd1306_WriteString(time, Font_7x10, White);
  ssd1306_UpdateScreen();

  ssd1306_SetCursor(DISPLAY_TIME_DATE_X_OFFSET, DISPLAY_DATE_Y_OFFSET);
  ssd1306_WriteString(date, Font_7x10, White);
  ssd1306_UpdateScreen();
}

/**
 * Simplified function to display the date on an oled screen.
 * Cursor offsets are stored in makros 
 */
void ssd1306_writeTime(char* time) {
  ssd1306_SetCursor(DISPLAY_TIME_DATE_X_OFFSET, DISPLAY_TIME_Y_OFFSET);
  ssd1306_WriteString(time, Font_7x10, White);
  ssd1306_UpdateScreen();
}

/**
 * Simplified function to display the time on an oled screen.
 * Cursor offsets are stored in makros 
 */
void ssd1306_writeDate(char* date) {
  ssd1306_SetCursor(DISPLAY_TIME_DATE_X_OFFSET, DISPLAY_DATE_Y_OFFSET);
  ssd1306_WriteString(date, Font_7x10, White);
  ssd1306_UpdateScreen();
}

void ssd1306_writeMisc(char* _data) {
  ssd1306_SetCursor(DISPLAY_MISC_X_OFFSET, DISPLAY_MISC_Y_OFFSET);
  ssd1306_WriteString(_data, Font_7x10, White);
  ssd1306_UpdateScreen();
}
#endif

/** Function for changing the tubes to the corresponding time values
 */
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

/** Function for changing the tubes to the corresponding date values
 */
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

/**
 * Sorts the bits to the correct spot for the output register TODO: Update PCB next time to have a nicer output register format not needing this shit -_-
 */
/*
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
*/

/*
void output_to_tubes(uint16_t _data) {
  //Output on the whole PORTB via the ODR (Output Data Register)
  GPIOB->ODR = _data;
}
*/

// TODO: FIX BUG WITH MENU_POSITION AND MENU_POSITION_OLD!!!!!!!!!!!!! 

void menu_mainTime() {
  output_blink_front_leds(solid);

  if(menu_position != menu_position_old) {
    getTimeDate(timeData, dateData, &TD_data);
    nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);

    menu_position_old = menu_position;
  }

  if(tick_flag == isSet) { //flag set by interrupt by RTC on 1Hz

    getTimeDate(timeData, dateData, &TD_data);      //Get time from RTC registers

    //Update display data, nixies dont have seconds!
    if(TD_data.seconds == 0) {
      nixieDisplay.displayDigitOutput = set_tube_numbers_time(&TD_data);
    }

    tick_flag = reset; //reset update flag
  }

  
}

void menu_mainDate() {
  output_blink_front_leds(solidBot);

  if(menu_position != menu_position_old) {
    getTimeDate(timeData, dateData, &TD_data);
    nixieDisplay.displayDigitOutput = set_tube_numbers_date(&TD_data);

    menu_position_old = menu_position;
  }

  if(tick_flag == isSet) {
    
    getTimeDate(timeData, dateData, &TD_data);

    nixieDisplay.displayDigitOutput = set_tube_numbers_date(&TD_data);

    tick_flag = reset;
  }
}

void menu_Sensor() {
  output_blink_front_leds(blinkBoth);

  if(menu_position != menu_position_old) {
    nixieDisplay.displayDigitOutput = combine_4bit_numbers(9, 9, 9, 9); //PLACEHOLDER

    nixieDisplay.displayStatus = 1;

    menu_position_old = menu_position;
  }

  if(tick_flag == isSet) {

    tick_flag = reset;
  }
}

void menu_startStop(uint8_t _submenu_pos, time_date_DataDigital* _Tdata_start) {

  output_blink_front_leds(blinkTop);
  //uint8_t _hours_tens, _hours_ones, _minutes_tens, _minutes_ones;
  
}

/**
 * @brief: Cycles through a number in one of 3 modes
 * @param _number -> variable to cycle through
 * @param _modes -> 
 * 
 *  0: possible numbers: 0,1,2
 * 
 *  1: possible numbers: 0-9
 * 
 *  2: possible numbers: 0-5
 */
uint8_t circleNumbers(uint8_t _number, uint8_t _mode) {

  switch(_mode) {
    case 0:
    if(btn_flag_plus) {
      if(_number < 3) {
        _number ++;
      } else _number = 0;
      break;
    }
    if(btn_flag_minus) {
      if(_number < 3) {
        _number --;
      } else _number = 2;
      break;
    } 
    case 1: 
      
  }

  if(btn_flag_plus) _number++;
  else if (btn_flag_minus) _number--;
  return _number;
}

void menu_timeSet(uint8_t _submenu_pos) {  
  output_blink_front_leds(blinkBot);
}

void menu_timeout(uint8_t _timeoutValue) {

  if(tick_count >= _timeoutValue) {
    menu_position = menuTIME;
  }

  time_update_flag = 1;
}

/**
 * @brief: Function to blink the front leds.
 * @param mode: 0-> no blinking, 1-> both, 2-> top, 3-> bot
 */
/*
void output_blink_front_leds(blink_mode _mode) {
  switch(_mode) {
    case solid: 
      output_front_led(1, 1);
      break;

    case blinkBoth: 
      output_front_led(tick_blink, tick_blink);
      break;

    case blinkTop:
      output_front_led(tick_blink, 1);
      break;

    case blinkBot:
      output_front_led(1, tick_blink);
      break;
  }
}
*/

/**
 * @brief: Function to manipulate the front leds.
 * @param led0: top led
 * @param led1: bottom led
 * @param led_status: 0->off; 1->on
 */
/*
void output_front_led(uint8_t led0, uint8_t led1) {

  if(led0)  GPIOA->BSRR = GPIO_BSRR_BS11;
  if(!led0) GPIOA->BSRR = GPIO_BSRR_BR11;

  if(led1)  GPIOA->BSRR = GPIO_BSRR_BS12;
  if(!led1) GPIOA->BSRR = GPIO_BSRR_BR12;
}
*/

/**
 * @brief: Check for installed addons via ID pins D2 and D3
 * Addons:
 * 0 -> DCF77
 */
void check_for_addons(void) {
    
  uint8_t addon_id = ((GPIOD->IDR & GPIO_IDR_ID3) << 1) | (GPIOD->IDR & GPIO_IDR_ID2);

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

/**
 * @brief Changes state when TICK_INTERVAL is reached. Works without timer and globally
 * @return 0 or 1 depending on TICK_INTERVAL
 */
/*
uint8_t blinkState(void) {
  return ((HAL_GetTick() / TICK_INTERVAL) % 2);
}
*/

void resetBtnFlags() {
  if(btn_flag_menu || btn_flag_minus || btn_flag_plus) {
    btn_flag_menu =  reset;
    btn_flag_minus = reset;
    btn_flag_plus =  reset; 
  }
  btn_pressed_flag = reset;
}

/*
//Blinking timer for front LEDs for example
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tick_blink = !tick_blink;
}
*/

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

      switch(menu_position) {
        case menuTIME:
          menu_position = menuDATE;
          break;
        case menuDATE:
          menu_position = menuSENSOR;
          break;
        case menuSENSOR:
          menu_position = menuTIME;
          break;
        default: break;
      }

      break;
    
    case btn_minus_Pin:
      btn_flag_minus = 1;

      tick_count = 0;

      switch(menu_position) {
        case menuTIME:
          menu_position = menuSENSOR;
          break;
        case menuDATE:
          menu_position = menuTIME;
          break;
        case menuSENSOR:
          menu_position = menuDATE;
          break;
        default: break;
      }

      break;

    case btn_menu_Pin:
      btn_flag_menu = 1;

      tick_count = 0;

      if(menu_position == menuTIME) {
        menu_position = menuStartStop;
      }
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


