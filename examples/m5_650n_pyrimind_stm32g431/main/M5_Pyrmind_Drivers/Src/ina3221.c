/**
 * @file ina3221.c
 * @brief INA3221 multi-device implementation
 */
#include "ina3221.h"

#define SHUNT_RESISTOR_MOHM 10  

/**
 * @brief Swap byte order of 16-bit register
 */
static uint16_t swap_bytes(uint16_t val) {
    return (val << 8) | (val >> 8);
}

/**
 * @brief Write a 16-bit register
 */
static void write_reg(ina3221_t *dev, uint8_t reg_addr, uint16_t val) {
    uint16_t swapped = swap_bytes(val);
    HAL_I2C_Mem_Write(dev->i2c, dev->address, reg_addr, 1, (uint8_t *)&swapped, 2, 100);
}

/**
 * @brief Read a 16-bit register
 */
static void read_reg(ina3221_t *dev, uint8_t reg_addr, uint16_t *val) {
    uint8_t buf[2] = {0};
    if (HAL_I2C_Mem_Read(dev->i2c, dev->address, reg_addr, 1, buf, 2, 100) == HAL_OK) {
        *val = (buf[0] << 8) | buf[1];
    } else {
        *val = 0; // 出错时置0
    }
}

/**
 * @brief Update voltage and current for a specific channel (0 ~ 2)
 */
static void update_channel_measurement(ina3221_t *dev, uint8_t ch) {
    if (ch >= 3) return;

    // Read shunt voltage
    uint16_t raw_shunt;
    read_reg(dev, INA3221_REG_SHUNT_VOLTAGE_CH1 + 2 * ch, &raw_shunt);
    dev->shunt_raw[ch] = raw_shunt;

    // Convert to current
    int32_t val = (int16_t)raw_shunt;
    val >>= 3;
    val *= 40;
    dev->current[ch] = val / SHUNT_RESISTOR_MOHM;

    // Read bus voltage
    uint16_t raw_bus;
    read_reg(dev, INA3221_REG_BUS_VOLTAGE_CH1 + 2 * ch, &raw_bus);
    dev->voltage[ch] = raw_bus;
}

/**
 * @brief Initialize INA3221
 */
void ina3221_init(ina3221_t *dev, I2C_HandleTypeDef *i2c, uint8_t addr) {
    dev->i2c = i2c;
    dev->address = addr;

    // 复位
    write_reg(dev, INA3221_REG_CONFIG, 0x8000);
    HAL_Delay(10);

    // 三通道都开启、平均16次、电压采集周期1.1ms、持续采集
    uint16_t cfg = 0x7527;
    write_reg(dev, INA3221_REG_CONFIG, cfg);

}

/**
 * @brief Update all voltage and current measurements
 */
void ina3221_update_measurements(ina3221_t *dev) {
    for (int ch = 0; ch < 3; ch++) {
        // Read shunt voltage
        uint16_t raw_shunt;
        read_reg(dev, INA3221_REG_SHUNT_VOLTAGE_CH1 + 2 * ch, &raw_shunt);
        dev->shunt_raw[ch] = raw_shunt;

        // Convert to current
        int32_t val = (int16_t)raw_shunt;
        val >>= 3;
        val *= 40;
        dev->current[ch] = val / SHUNT_RESISTOR_MOHM;

        // Read bus voltage
        uint16_t raw_bus;
        read_reg(dev, INA3221_REG_BUS_VOLTAGE_CH1 + 2 * ch, &raw_bus);
        dev->voltage[ch] = raw_bus;
    }
}

/**
 * @brief Update CH1 voltage and current
 */
void ina3221_update_ch0(ina3221_t *dev) {
    update_channel_measurement(dev, 0);
}

/**
 * @brief Update CH2 voltage and current
 */
void ina3221_update_ch1(ina3221_t *dev) {
    update_channel_measurement(dev, 1);
}

/**
 * @brief Update CH3 voltage and current
 */
void ina3221_update_ch2(ina3221_t *dev) {
    update_channel_measurement(dev, 2);
}

/**
 * @brief Set critical current limit
 */
void ina3221_set_critical_limit(ina3221_t *dev, uint8_t channel, uint16_t current_mA) {
    if (channel < 3) {
        uint16_t reg = INA3221_REG_CRIT_CH1 + 2 * channel;
        uint16_t val = (current_mA * SHUNT_RESISTOR_MOHM / 40) << 3;
        write_reg(dev, reg, val);
    }
}
