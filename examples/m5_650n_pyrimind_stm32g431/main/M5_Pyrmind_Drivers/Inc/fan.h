/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _FAN_H_
#define _FAN_H_

#ifdef __cplusplus

extern "C" {
#endif

#include "config.h"

#define FAN_PWM_MIN        (20)
#define FAN_PWM_MAX        (100)
#define FAN_ARR_MAX        (6800)
#define FAN_PWM_DEFINE     (FAN_PWM_MAX)
#define MEASURE_TIMEOUT_MS (50)
#define TIMER_CLOCK_HZ     (1000000UL)
#define FAN_PULSE_PER_REV  (2)

void fan_init(void);

void set_fan_pwm(uint16_t pwm);

uint16_t get_fan_rpm(void);

bool fan_pwm_check(uint16_t pwm);

#ifdef __cplusplus
}
#endif

#endif /* _FAN_H_ */
