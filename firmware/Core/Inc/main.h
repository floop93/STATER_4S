/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOB
#define ADC1_IN0_POT1_Pin GPIO_PIN_0
#define ADC1_IN0_POT1_GPIO_Port GPIOA
#define ADC1_IN1_POT2_Pin GPIO_PIN_1
#define ADC1_IN1_POT2_GPIO_Port GPIOA
#define TRIG1_Pin GPIO_PIN_2
#define TRIG1_GPIO_Port GPIOA
#define TRIG2_Pin GPIO_PIN_3
#define TRIG2_GPIO_Port GPIOA
#define TRIG3_Pin GPIO_PIN_4
#define TRIG3_GPIO_Port GPIOA
#define TRIG4_Pin GPIO_PIN_5
#define TRIG4_GPIO_Port GPIOA
#define ECHO_ALL_Pin GPIO_PIN_0
#define ECHO_ALL_GPIO_Port GPIOB
#define ECHO_ALL_EXTI_IRQn EXTI0_1_IRQn
#define OUT_R_Pin GPIO_PIN_9
#define OUT_R_GPIO_Port GPIOA
#define OUT_L_Pin GPIO_PIN_10
#define OUT_L_GPIO_Port GPIOA
#define OUT_UP_Pin GPIO_PIN_11
#define OUT_UP_GPIO_Port GPIOA
#define OUT_DW_Pin GPIO_PIN_12
#define OUT_DW_GPIO_Port GPIOA
#define SW3_Pin GPIO_PIN_3
#define SW3_GPIO_Port GPIOB
#define SW2_Pin GPIO_PIN_4
#define SW2_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_5
#define SW1_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_6
#define LED4_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_7
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_8
#define LED2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
