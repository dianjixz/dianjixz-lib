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
#include "stm32l0xx_hal.h"

#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"

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
#define PMIC1_IRQ_Pin LL_GPIO_PIN_9
#define PMIC1_IRQ_GPIO_Port GPIOA
#define PMIC2_IRQ_Pin LL_GPIO_PIN_3
#define PMIC2_IRQ_GPIO_Port GPIOB
#define PMIC1_CS_Pin LL_GPIO_PIN_4
#define PMIC1_CS_GPIO_Port GPIOA
#define AX_RST_OUT_Pin LL_GPIO_PIN_1
#define AX_RST_OUT_GPIO_Port GPIOA
#define RUN_LED_Pin LL_GPIO_PIN_14
#define RUN_LED_GPIO_Port GPIOC
#define FAN_TACO_Pin LL_GPIO_PIN_8
#define FAN_TACO_GPIO_Port GPIOA
#define FAN_PWM_Pin LL_GPIO_PIN_10
#define FAN_PWM_GPIO_Port GPIOA
#define PWR_LED_Pin LL_GPIO_PIN_15
#define PWR_LED_GPIO_Port GPIOC
#define PMIC2_CE_Pin LL_GPIO_PIN_1
#define PMIC2_CE_GPIO_Port GPIOB
#define VIN_SEN_Pin LL_GPIO_PIN_2
#define VIN_SEN_GPIO_Port GPIOA
#define PMIC1_CE_Pin LL_GPIO_PIN_0
#define PMIC1_CE_GPIO_Port GPIOB
#define PMIC2_CS_Pin LL_GPIO_PIN_3
#define PMIC2_CS_GPIO_Port GPIOA
#define PCIE_RST_IN_Pin LL_GPIO_PIN_0
#define PCIE_RST_IN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
