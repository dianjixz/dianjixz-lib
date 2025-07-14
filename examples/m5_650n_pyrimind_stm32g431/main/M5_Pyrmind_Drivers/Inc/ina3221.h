/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _INA3221_H_
#define _INA3221_H_

#include "config.h"

// Register address map
#define INA3221_REG_CONFIG              0x00
#define INA3221_REG_SHUNT_VOLTAGE_CH1   0x01
#define INA3221_REG_BUS_VOLTAGE_CH1     0x02
#define INA3221_REG_SHUNT_VOLTAGE_CH2   0x03
#define INA3221_REG_BUS_VOLTAGE_CH2     0x04
#define INA3221_REG_SHUNT_VOLTAGE_CH3   0x05
#define INA3221_REG_BUS_VOLTAGE_CH3     0x06
#define INA3221_REG_CRIT_CH1            0x07
#define INA3221_REG_CRIT_CH2            0x09
#define INA3221_REG_CRIT_CH3            0x0B
#define INA3221_REG_MANUFACTURER_ID     0xFE
#define INA3221_REG_DIE_ID              0xFF

#define INA3221_ADDR_A1A0_00   0x40  /**< A1=0, A0=0 */
#define INA3221_ADDR_A1A0_01   0x41  /**< A1=0, A0=1 */

typedef struct {
    I2C_HandleTypeDef *i2c;
    uint8_t address;
    uint16_t voltage[3];
    uint16_t shunt_raw[3];
    int32_t current[3];
} ina3221_t;

void ina3221_init(ina3221_t *dev, I2C_HandleTypeDef *i2c, uint8_t addr);
void ina3221_update_measurements(ina3221_t *dev);
void ina3221_update_ch0(ina3221_t *dev);
void ina3221_update_ch1(ina3221_t *dev);
void ina3221_update_ch2(ina3221_t *dev);
void ina3221_set_critical_limit(ina3221_t *dev, uint8_t channel, uint16_t current_mA);

#endif // _INA3221_H_