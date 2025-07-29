/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _POWER_H_
#define _POWER_H_

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "max77812.h"
#include "tim.h"

/******************************* PMIC 寄存器定义 *******************************/
#define PMIC_REG_RESET   0x00
#define PMIC_REG_EN_CTRL 0x06

/******************************* 相位使能位定义 *******************************/
#define EN_M1 (1 << 0)
#define EN_M2 (1 << 2)
#define EN_M3 (1 << 4)
#define EN_M4 (1 << 6)

/******************************* 电源轨与相位映射 *****************************/
// PMIC1配置
#define PMIC1_VOUT_08V EN_M1  // M1
#define PMIC1_VOUT_06V EN_M3  // M3

// PMIC2配置
#define PMIC2_VOUT_10V EN_M1  // M1
#define PMIC2_VOUT_MXV EN_M2  // M2
#define PMIC2_VOUT_11V EN_M3  // M3
#define PMIC2_VOUT_06V EN_M4  // M4

// 系统上电
void system_power_boot(void);

#ifdef __cplusplus
}
#endif

#endif /* _POWER_H_ */
