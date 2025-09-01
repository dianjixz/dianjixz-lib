/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _I2C_EX_H_
#define _I2C_EX_H_

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern volatile uint32_t i2c_timeout_counter;
extern volatile uint32_t i2c_stop_timeout_flag;
extern volatile uint32_t i2c_stop_timeout_counter;

extern void i2c1_it_enable(void);
extern void i2c1_it_disable(void);
extern void i2c1_set_send_data(uint8_t *tx_ptr, uint16_t len);
extern void set_i2c_slave_address(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif /* _I2C_EX_H_ */
