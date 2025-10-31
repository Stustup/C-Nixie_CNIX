/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ht_EN_Pin GPIO_PIN_0
#define ht_EN_GPIO_Port GPIOA
#define btn_plus_Pin GPIO_PIN_5
#define btn_plus_GPIO_Port GPIOA
#define btn_plus_EXTI_IRQn EXTI4_15_IRQn
#define btn_menu_Pin GPIO_PIN_6
#define btn_menu_GPIO_Port GPIOA
#define btn_menu_EXTI_IRQn EXTI4_15_IRQn
#define btn_minus_Pin GPIO_PIN_7
#define btn_minus_GPIO_Port GPIOA
#define btn_minus_EXTI_IRQn EXTI4_15_IRQn
#define co1_3_Pin GPIO_PIN_0
#define co1_3_GPIO_Port GPIOB
#define co1_2_Pin GPIO_PIN_1
#define co1_2_GPIO_Port GPIOB
#define co1_1_Pin GPIO_PIN_2
#define co1_1_GPIO_Port GPIOB
#define co1_0_Pin GPIO_PIN_10
#define co1_0_GPIO_Port GPIOB
#define co0_3_Pin GPIO_PIN_11
#define co0_3_GPIO_Port GPIOB
#define co0_2_Pin GPIO_PIN_12
#define co0_2_GPIO_Port GPIOB
#define co0_1_Pin GPIO_PIN_13
#define co0_1_GPIO_Port GPIOB
#define co0_0_Pin GPIO_PIN_14
#define co0_0_GPIO_Port GPIOB
#define co2_0_Pin GPIO_PIN_15
#define co2_0_GPIO_Port GPIOB
#define pwr_led_Pin GPIO_PIN_15
#define pwr_led_GPIO_Port GPIOA
#define co2_1_Pin GPIO_PIN_3
#define co2_1_GPIO_Port GPIOB
#define co2_2_Pin GPIO_PIN_4
#define co2_2_GPIO_Port GPIOB
#define co2_3_Pin GPIO_PIN_5
#define co2_3_GPIO_Port GPIOB
#define co3_0_Pin GPIO_PIN_6
#define co3_0_GPIO_Port GPIOB
#define co3_1_Pin GPIO_PIN_7
#define co3_1_GPIO_Port GPIOB
#define co3_2_Pin GPIO_PIN_8
#define co3_2_GPIO_Port GPIOB
#define co3_3_Pin GPIO_PIN_9
#define co3_3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
