/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

typedef enum
{
    LED_MODE_ARM_MAP  = 0x00, /**< ARM映射模式 */
    LED_MODE_VOLTAGE  = 0x01, /**< 电压模式 */
    LED_MODE_IP       = 0x02  /**< IP模式 */
} led_mode_t;

bool led_mode_check(uint16_t mode);

void set_led_arm_flag(bool flag);

bool get_led_arm_flag(void);

#ifdef __cplusplus
}
#endif

#endif /* _LED_H_ */
