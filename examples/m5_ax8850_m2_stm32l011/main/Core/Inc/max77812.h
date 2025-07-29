/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _MX77812_H_
#define _MX77812_H_

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "stdio.h"

#define MAX77812_SPI_TIMEOUT (100)

#define MAX77812_RW_WRITE           (0x80) /**< R/W=1: Write */
#define MAX77812_RW_READ            (0x00) /**< R/W=0: Read */
#define MAX77812_SINGLE_OP          (0x00) /**< S/M=0: Single operation */
#define MAX77812_MULTIPLE_OP        (0x40) /**< S/M=1: Multiple operation */
#define MAX77812_SPI_HEADER(RW, SM) ((RW) | (SM))

#define PMIC1_CS_ENABLE()   LL_GPIO_ResetOutputPin(PMIC1_CS_GPIO_Port, PMIC1_CS_Pin)   // PMIC1 片选使能（拉低）
#define PMIC1_CS_DISABLE()  LL_GPIO_SetOutputPin(PMIC1_CS_GPIO_Port, PMIC1_CS_Pin)     // PMIC1 片选失能（拉高）

#define PMIC2_CS_ENABLE()   LL_GPIO_ResetOutputPin(PMIC2_CS_GPIO_Port, PMIC2_CS_Pin)   // PMIC2 片选使能（拉低）
#define PMIC2_CS_DISABLE()  LL_GPIO_SetOutputPin(PMIC2_CS_GPIO_Port, PMIC2_CS_Pin)     // PMIC2 片选失能（拉高）

typedef enum { MAX77812_PMIC1 = 0x00, MAX77812_PMIC2 = 0x01 } max77812_pmic_t;

HAL_StatusTypeDef max77812_write_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t data);

HAL_StatusTypeDef max77812_read_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t *data);

HAL_StatusTypeDef max77812_mult_write_reg(max77812_pmic_t pmic, uint8_t reg, const uint8_t *data, uint8_t size);

HAL_StatusTypeDef max77812_mult_read_reg(max77812_pmic_t pmic, uint8_t reg, uint8_t *data, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* _MX77812_H_ */
