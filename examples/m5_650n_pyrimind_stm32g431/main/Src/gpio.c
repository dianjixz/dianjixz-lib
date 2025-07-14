/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, PWR_GRV1_EN_Pin|DS3_PWR_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWR_GRV2_EN_GPIO_Port, PWR_GRV2_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PMIC2_CE_Pin|PMIC1_CE_Pin|V5VE_PWR_EN_Pin|V5V_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PMIC2_CS_Pin|PMIC1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, VDD3_EN_Pin|VDD18_EN_Pin|NGFF1_PWR_EN_Pin|PCIE_REFC_EN_Pin
                          |GL_RESET_J_Pin|DS1_HS_EN_Pin|DS2_HS_EN_Pin|NGFF2_PWR_EN_Pin
                          |DL_MODE_Pin|SYS_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DS1_PWR_EN_Pin|DS2_PWR_EN_Pin|PWR_GRV3_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PWR_GRV1_EN_Pin PWR_GRV2_EN_Pin DS3_PWR_EN_Pin */
  GPIO_InitStruct.Pin = PWR_GRV1_EN_Pin|PWR_GRV2_EN_Pin|DS3_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : VDD18_PG_Pin */
  GPIO_InitStruct.Pin = VDD18_PG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(VDD18_PG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PMIC2_CE_Pin PMIC2_CS_Pin PMIC1_CS_Pin PMIC1_CE_Pin
                           V5VE_PWR_EN_Pin V5V_PWR_EN_Pin */
  GPIO_InitStruct.Pin = PMIC2_CE_Pin|PMIC2_CS_Pin|PMIC1_CS_Pin|PMIC1_CE_Pin
                          |V5VE_PWR_EN_Pin|V5V_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : VDD3_EN_Pin VDD18_EN_Pin NGFF1_PWR_EN_Pin PCIE_REFC_EN_Pin
                           GL_RESET_J_Pin DS1_HS_EN_Pin DS1_PWR_EN_Pin DS2_PWR_EN_Pin
                           DS2_HS_EN_Pin NGFF2_PWR_EN_Pin DL_MODE_Pin PWR_GRV3_EN_Pin */
  GPIO_InitStruct.Pin = VDD3_EN_Pin|VDD18_EN_Pin|NGFF1_PWR_EN_Pin|PCIE_REFC_EN_Pin
                          |GL_RESET_J_Pin|DS1_HS_EN_Pin|DS1_PWR_EN_Pin|DS2_PWR_EN_Pin
                          |DS2_HS_EN_Pin|NGFF2_PWR_EN_Pin|DL_MODE_Pin|PWR_GRV3_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PMIC_IRQ_Pin */
  GPIO_InitStruct.Pin = PMIC_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PMIC_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PCIE0_PRSNTN_Pin */
  GPIO_InitStruct.Pin = PCIE0_PRSNTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PCIE0_PRSNTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EC_BUTTON_Pin */
  GPIO_InitStruct.Pin = EC_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EC_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCIE1_PRSNTN_Pin VDD3_PG_Pin */
  GPIO_InitStruct.Pin = PCIE1_PRSNTN_Pin|VDD3_PG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SYS_RST_Pin */
  GPIO_InitStruct.Pin = SYS_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SYS_RST_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
