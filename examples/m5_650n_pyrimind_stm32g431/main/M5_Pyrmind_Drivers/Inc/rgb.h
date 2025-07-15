/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _RGB_H_
#define _RGB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#define RGB_NUM_MAX             (48)
#define RGB_NUM_MIN             (1)
#define RGB_DEFAULT_NUM         (37)

#define RGB_UPDATE_INTERVAL_MS 50  // 刷新周期: 100ms

#define RGB_GRADIENT_COLOR_COUNT (6)
#define RGB_GRADIENT_STEP_MAX    (360)
#define RGB_GRADIENT_BRIGHTNESS  (180)

#define RGB_BLINK_BRIGHTNESS_MAX (250)
#define RGB_BLINK_BRIGHTNESS_MIN (20)

#define RGB_RAINFLOW_TAIL_LENGTH (20)
#define RGB_RAINFLOW_TAIL_STEP   (50)

typedef enum
{
    RGB_MODE_ARM_MAP   = 0x00, /**< ARM映射模式 */
    RGB_MODE_GRADIENT  = 0x01, /**< 渐变模式 */
    RGB_MODE_BLINK     = 0x02, /**< 闪烁模式 */
    RGB_MODE_RAINFLOW  = 0x03  /**< 流水模式 */
} rgb_mode_t;

void rgb_init(void);

void rgb_show(void);

void rgb_power_start_indicate(void);

void rgb_power_complete_indicate(void);

bool rgb_num_check(uint16_t num);

bool rgb_mode_check(uint16_t mode);

void set_rgb_arm_flag(bool flag);

bool get_rgb_arm_flag(void);

void set_rgb_mode(rgb_mode_t mode);

rgb_mode_t get_rgb_mode(void);

void rgb_update(void);

void rgb_test(void);

#ifdef __cplusplus
}
#endif

#endif /* _RGB_H_ */
