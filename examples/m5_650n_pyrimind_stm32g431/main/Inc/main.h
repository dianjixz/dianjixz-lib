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
#include "stm32g4xx_hal.h"

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
#define PWR_GRV1_EN_Pin GPIO_PIN_14
#define PWR_GRV1_EN_GPIO_Port GPIOC
#define PWR_GRV2_EN_Pin GPIO_PIN_15
#define PWR_GRV2_EN_GPIO_Port GPIOC
#define EC_I2C_SDA_Pin GPIO_PIN_0
#define EC_I2C_SDA_GPIO_Port GPIOF
#define VDD18_PG_Pin GPIO_PIN_1
#define VDD18_PG_GPIO_Port GPIOF
#define FAN_TACO_Pin GPIO_PIN_0
#define FAN_TACO_GPIO_Port GPIOA
#define PMIC2_CE_Pin GPIO_PIN_1
#define PMIC2_CE_GPIO_Port GPIOA
#define PMIC2_CS_Pin GPIO_PIN_2
#define PMIC2_CS_GPIO_Port GPIOA
#define PMIC1_CS_Pin GPIO_PIN_3
#define PMIC1_CS_GPIO_Port GPIOA
#define PMIC1_CE_Pin GPIO_PIN_4
#define PMIC1_CE_GPIO_Port GPIOA
#define EX_I2C_SCL_Pin GPIO_PIN_4
#define EX_I2C_SCL_GPIO_Port GPIOC
#define VDD3_EN_Pin GPIO_PIN_0
#define VDD3_EN_GPIO_Port GPIOB
#define VDD18_EN_Pin GPIO_PIN_1
#define VDD18_EN_GPIO_Port GPIOB
#define NGFF1_PWR_EN_Pin GPIO_PIN_2
#define NGFF1_PWR_EN_GPIO_Port GPIOB
#define PCIE_REFC_EN_Pin GPIO_PIN_10
#define PCIE_REFC_EN_GPIO_Port GPIOB
#define GL_RESET_J_Pin GPIO_PIN_11
#define GL_RESET_J_GPIO_Port GPIOB
#define DS1_HS_EN_Pin GPIO_PIN_12
#define DS1_HS_EN_GPIO_Port GPIOB
#define DS1_PWR_EN_Pin GPIO_PIN_13
#define DS1_PWR_EN_GPIO_Port GPIOB
#define DS2_PWR_EN_Pin GPIO_PIN_14
#define DS2_PWR_EN_GPIO_Port GPIOB
#define DS2_HS_EN_Pin GPIO_PIN_15
#define DS2_HS_EN_GPIO_Port GPIOB
#define PMIC_IRQ_Pin GPIO_PIN_6
#define PMIC_IRQ_GPIO_Port GPIOC
#define PMIC_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define FAN_PWM_Pin GPIO_PIN_8
#define FAN_PWM_GPIO_Port GPIOA
#define AX_UART3_RX_Pin GPIO_PIN_9
#define AX_UART3_RX_GPIO_Port GPIOA
#define AX_UART3_TX_Pin GPIO_PIN_10
#define AX_UART3_TX_GPIO_Port GPIOA
#define V5VE_PWR_EN_Pin GPIO_PIN_11
#define V5VE_PWR_EN_GPIO_Port GPIOA
#define PCIE0_PRSNTN_Pin GPIO_PIN_12
#define PCIE0_PRSNTN_GPIO_Port GPIOA
#define V5V_PWR_EN_Pin GPIO_PIN_15
#define V5V_PWR_EN_GPIO_Port GPIOA
#define EC_BUTTON_Pin GPIO_PIN_10
#define EC_BUTTON_GPIO_Port GPIOC
#define DS3_PWR_EN_Pin GPIO_PIN_11
#define DS3_PWR_EN_GPIO_Port GPIOC
#define PCIE1_PRSNTN_Pin GPIO_PIN_3
#define PCIE1_PRSNTN_GPIO_Port GPIOB
#define NGFF2_PWR_EN_Pin GPIO_PIN_4
#define NGFF2_PWR_EN_GPIO_Port GPIOB
#define DL_MODE_Pin GPIO_PIN_5
#define DL_MODE_GPIO_Port GPIOB
#define VDD3_PG_Pin GPIO_PIN_6
#define VDD3_PG_GPIO_Port GPIOB
#define PWR_GRV3_EN_Pin GPIO_PIN_7
#define PWR_GRV3_EN_GPIO_Port GPIOB
#define SYS_RST_Pin GPIO_PIN_9
#define SYS_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
