/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "main.h"
#include "math.h"
#include "spi.h"
#include "stdbool.h"
#include "stdio.h"
#include "tim.h"
#include "usart.h"

#define DEBUG

#define POWER_DEBUG
// #define FLASH_DEBUG
// #define FAN_DEBUG
// #define USE_UART_DEBUG

#define FIRMWARE_VERSION (0x00F0)

#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
