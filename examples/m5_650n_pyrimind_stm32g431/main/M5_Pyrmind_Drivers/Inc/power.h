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
#include "config.h"
#include "max77812.h"
#include "fan.h"

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

#define SYSTEM_POWER_ON_TIMER_MIN (1000)

#define VDD_CORE_MIN (1000)

#define VDD_CORE_MAX (1200)

#define VDD_CORR_DEFAULT (VDD_CORE_MIN)

// 系统上电
void system_power_boot(void);

void system_power_on(void);

void system_power_off(void);

// 外设上电
void peripheral_power_boot(void);

void peripheral_power_on(void);

void peripheral_power_off(void);


bool system_power_off_time_check(uint16_t power_off_timer);

void set_system_power_off_time(uint16_t time);

bool vdd_core_check(uint16_t vdd_core);

void set_vdd_cpu(uint16_t valtage);

void update_vdd_cup(void);

#ifdef __cplusplus
}
#endif

#endif /* _POWER_H_ */
