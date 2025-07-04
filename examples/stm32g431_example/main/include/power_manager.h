/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __POWER_MANAGERR_H
#define __POWER_MANAGERR_H

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "max77812.h"

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
#define VDD_CORE_PHASE EN_M1  // M1
#define VDD_NN_PHASE   EN_M3  // M3

// PMIC2配置
#define DDRV1V1_PHASE EN_M3  // M3
#define DDRV0V6_PHASE EN_M4  // M4
#define VDD_CPU_PHASE EN_M1  // M1

// 系统上电
void system_power_boot(void);
// 外设上电
void peripheral_power_boot(void);

#ifdef __cplusplus
}
#endif

#endif /* __POWER_MANAGERR_H */
