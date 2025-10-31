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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306_fonts.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_rtc.h"
#include "stm32g0xx_hal_rtc_ex.h"
#include <stdint.h>
#include "stdio.h"
#include "ssd1306.h"
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

/**
 * These should only be used one time at the first programming of the mcu. When you do have to manually set the time, just 
 * put the right data here and change the RANDOM_SEED_OFFSET to something diffrent than the current value (16bit)
 */
#define RANDOM_SEED_UPDATE 0x2344

#define CURRENT_TIME_HOURS   17
#define CURRENT_TIME_MINUTES  4
#define CURRENT_TIME_SECONDS  0

#define CURRENT_DATE_YEAR    25
#define CURRENT_DATE_MONTH   10
#define CURRENT_DATE_DAY     15
#define CURRENT_DATE_WEEKDAY  3

#define DISPLAY_MENU_RESET_TIME 1000

#define set 1
#define true 1
#define isSet 1
#define isPressed 1

#define reset 0
#define false 0
#define isNotSet 0
#define isNotPressed 0
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
volatile uint8_t update_flag = isNotSet;

/**
 * Value of the button being pressed.
 * 1: MINUS
 * 2: MENU
 * 3: PLUS
 */
volatile int8_t btn_flag = 0;
uint8_t btn_pressed_flag = isNotPressed;

/**
 * Char arrays to use strings directly on oled, could be omitted on final device.
 */
char timeData[15];
char dateData[15];

/**
 * DataDigital struct to hold digital values of the time from RTC
 */
struct time_date_DataDigital {
  uint8_t hours, minutes, seconds;

  uint8_t day, weekday, month, year;
} TD_data;

/**
 * main counter for getTick function
 */
uint32_t start_ms_counter = 0;

uint8_t error_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minute, uint8_t second);
HAL_StatusTypeDef setDate(uint8_t year, uint8_t month, uint8_t weekday, uint8_t date);
HAL_StatusTypeDef getTimeDate(char* time, char* date, struct time_date_DataDigital* dTime);

void ssd1306_writeTime(char* time);
void ssd1306_writeDate(char* date);
void ssd1306_writeTimeDate(char* time, char* date);
void ssd1306_writeMisc(int8_t data);
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
  /* USER CODE BEGIN 2 */


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
  
  //Try 10 times or till a HAL_OK is retrieved to get the time from RTC
  if(getTimeDate(timeData, dateData, &TD_data) != HAL_OK) {
    HAL_StatusTypeDef _Status = HAL_ERROR;
    uint8_t tries = 0;
    while((_Status == HAL_ERROR) || (tries <= 10)) {
      _Status = getTimeDate(timeData, dateData, &TD_data);
      tries++;
    }
  }
  
  /**
   * Constructor for the basic oled menu with predeceeding welcome message
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
  ssd1306_WriteString("Button pressed: ", Font_7x10, White);
  ssd1306_writeMisc(0);
  ssd1306_UpdateScreen();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(update_flag == isSet) { //flag set by interrupt by RTC on 1Hz

      getTimeDate(timeData, dateData, &TD_data);      //Get tiome from RTC registers
      ssd1306_writeTimeDate(timeData, dateData);

      update_flag = reset; //reset update flag
    }

    /**
     * Main switch case for the buttons 
     */
    if(btn_flag != isNotPressed) {
      ssd1306_writeMisc(btn_flag);

      switch (btn_flag) {
        case 2:

          HAL_GPIO_TogglePin(ht_EN_GPIO_Port, ht_EN_Pin);
          break;

        default:
          break;
      }

      start_ms_counter = HAL_GetTick();
      btn_pressed_flag = isPressed;
      btn_flag = false;
    }

    /**
     * Reset button number after some time (defined in DISPLAY_MENU_RESET_TIME)
     */
    if(btn_pressed_flag == isPressed) {

      if((HAL_GetTick()-start_ms_counter) > DISPLAY_MENU_RESET_TIME) {
        ssd1306_writeMisc(0);
        btn_pressed_flag = isNotPressed;
      }
    }

    /**
     * Update Nixies, they dont have seconds!
     */
    if(TD_data.seconds == 0) {
      
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
  hi2c1.Init.Timing = 0x00B07CB4;
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
  
  sTime.Hours = 0x14;
  sTime.Minutes = 0x10;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_ADD1H;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 0x15;
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
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_ADD1H;
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ht_EN_GPIO_Port, ht_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, co1_3_Pin|co1_2_Pin|co1_1_Pin|co1_0_Pin
                          |co0_3_Pin|co0_2_Pin|co0_1_Pin|co0_0_Pin
                          |co2_0_Pin|co2_1_Pin|co2_2_Pin|co2_3_Pin
                          |co3_0_Pin|co3_1_Pin|co3_2_Pin|co3_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(pwr_led_GPIO_Port, pwr_led_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : ht_EN_Pin pwr_led_Pin */
  GPIO_InitStruct.Pin = ht_EN_Pin|pwr_led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : btn_plus_Pin btn_menu_Pin btn_minus_Pin */
  GPIO_InitStruct.Pin = btn_plus_Pin|btn_menu_Pin|btn_minus_Pin;
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
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }
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
HAL_StatusTypeDef getTimeDate(char* time, char* date, struct time_date_DataDigital* dTime) {
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;
  uint8_t _error_count = 0;

  //Get current time
  if(HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN) != HAL_OK) _error_count++;

  //Get current date
  if(HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN) != HAL_OK) _error_count++;

  dTime->hours = gTime.Hours;
  dTime->minutes = gTime.Minutes;
  dTime->seconds = gTime.Seconds;

  dTime->day = gDate.Date;
  dTime->weekday = gDate.WeekDay;
  dTime->month = gDate.Month;
  dTime->year = gDate.Year;

  /* Display time Format: hh:mm:ss */
  sprintf(time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);

  /* Display date Format: dd-mm-yyyy */
  sprintf(date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);

  if(_error_count != 0) return HAL_ERROR;
  return HAL_OK;
}

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

void ssd1306_writeMisc(int8_t data) {
  char bufnum[2];
  sprintf(bufnum, "%d", btn_flag);
  ssd1306_SetCursor(DISPLAY_MISC_X_OFFSET, DISPLAY_MISC_Y_OFFSET);
  ssd1306_WriteString(bufnum, Font_7x10, White);
  ssd1306_UpdateScreen();
}

//Interrupt for triggering an update event every second
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
  update_flag = set;
}

/**
 * Interrupt Handler for the buttons
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  switch(GPIO_Pin) {
    case btn_plus_Pin:
      btn_flag = 3;
      break;
    
    case btn_minus_Pin:
      btn_flag = 1;
      break;

    case btn_menu_Pin:
      btn_flag = 2;
      break;

    default:
      break;
  }
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
