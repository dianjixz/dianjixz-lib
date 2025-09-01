#include "power.h"
#include "max77812.h"

/******************************* PMIC 基础控制命令 *****************************/
/* 复位命令数组 [寄存器地址, 值] */
const uint8_t pmic_reset_cmd[2] = {
    0x00,  // 复位寄存器地址
    0x01   // 写入1触发软件复位
};

/* 关闭所有输出命令数组 [寄存器地址, 值] */
const uint8_t pmic_disable_all_cmd[2] = {
    0x06,  // 使能控制寄存器地址
    0x00   // 关闭全部输出
};

/******************************* PMIC1 配置 **********************/
///* (M1) - 0.8V */
// const uint8_t pmic1_m1_08v_buffr[2] = {
//     0x23,  // M1输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x03)
//     0x6E   // (0.8-0.25)/0.005 = 0x6E
// };

/* (M1) - 0.85V */
const uint8_t pmic1_m1_085v_buffr[2] = {
    0x23,  // M1输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x03)
    0x78   // (0.85-0.25)/0.005 = 0x78
};

///* (M3) - 0.6V */
// const uint8_t pmic1_m3_06v_buffr[2] = {
//     0x25,  // M3输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x05)
//     0x46   // (0.6-0.25)/0.005 = 0x46
// };

/* (M3) - 0.65V */
const uint8_t pmic1_m3_065v_buffr[2] = {
    0x25,  // M3输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x05)
    0x50   // (0.65-0.25)/0.005 = 0x50
};

/******************************* PMIC2 配置 ********************/
///* (M1) - 1.0V */
// const uint8_t pmic2_m1_10v_buffr[2] = {
//     0x23,  // M1输出电压寄存器
//     0x96   // (1.0-0.25)/0.005 = 0x96
// };

/* (M1) - 1.05V */
const uint8_t pmic2_m1_105v_buffr[2] = {
    0x23,  // M1输出电压寄存器
    0xA0   // (1.05-0.25)/0.005 = 0xA0
};

/* (M2) - maxV */
const uint8_t pmic2_m2_max_buffr[2] = {
    0x24,  // M2输出电压寄存器
    0xF9   // MAX
};

///* (M3) - 1.1V */
// const uint8_t pmic2_m3_1v1_buffr[2] = {
//     0x25,  // M3输出电压寄存器
//     0xAA   // (1.1-0.25)/0.005 = 0xAA
// };

/* (M3) - 1.15V */
const uint8_t pmic2_m3_1v15_buffr[2] = {
    0x25,  // M3输出电压寄存器
    0xB4   // (1.15-0.25)/0.005 = 0xB4
};

///* (M4) - 0.6V */
// const uint8_t pmic2_m4_06v_buffr[2] = {
//     0x26,  // M4输出电压寄存器
//     0x46   // (0.6-0.25)/0.005 = 0x46
// };

/* (M4) - 1.1V */
const uint8_t pmic2_m4_1v1_buffr[2] = {
    0x26,  // M4输出电压寄存器
    0xAA   // 1.1-0.25)/0.005 = 0xAA
};

static void power_error_fatal(void)
{
    LL_GPIO_ResetOutputPin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin);
    LL_GPIO_ResetOutputPin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin);

    while (1) {
        LL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
        LL_mDelay(500);
    }
}

static void max77812_write_and_verify(max77812_pmic_t pmic, uint8_t reg, uint8_t value)
{
    uint8_t read_val = 0;
    HAL_StatusTypeDef status;
    uint32_t tick_start = HAL_GetTick();

    while (1) {
        status = max77812_write_reg(pmic, reg, value);
        if (status != HAL_OK) {
            HAL_Delay(3);
            continue;
        }

        status = max77812_read_reg(pmic, reg, &read_val);
        if (status != HAL_OK) {
            HAL_Delay(3);
            continue;
        }

        if (read_val == value) {
            return;
        }

        if ((HAL_GetTick() - tick_start) >= 100) {
            power_error_fatal();
        }
    }
}

static void wait_pmic_pok(max77812_pmic_t pmic)
{
    uint8_t read_val = 0;
    HAL_StatusTypeDef status;
    uint32_t tick_start = HAL_GetTick();
    while (1) {
        status = max77812_read_reg(pmic, PMIC_POK_STATUS_REGISTER, &read_val);
        if (status == HAL_OK && read_val == 0) {
            return;
        }
        if ((HAL_GetTick() - tick_start) >= 100) {
            power_error_fatal();
        }
    }
}

// 系统上电
// 1. 等待20ms
// 2. 等待PCIE0_RST_IN电平为高
// 3. 使你PMIC1、PMIC2使能
// 4. 软件复位PMIC1、PMIC2
// 5. 关闭PMIC1、PMIC2所有通道
// 6. 设置PMIC2 M2 MxVout到0xF9，并开启通道
// 7. 设置PMIC2 M3到1.1V，M4到0.6V，并同时开启两个通道
// 8. 设置PMIC1 M1到0.8V，并开启通道
// 9. 设置PMIC2 M1到1V，并开启通道
// 10. 设置PMIC1 M3到0.6V，并开启通道
// 11. 拉高AX_RST_OUT
// 12. 开启风扇PWM 100%
// 13. 拉高RUN LED
void system_power_boot(void)
{
    // 1. 等待20ms
    // LL_mDelay(20);

    // 2. 等待PCIE0_RST_IN为高
    // while (HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin) == GPIO_PIN_RESET);

    // 3. 使能PMIC
    LL_GPIO_SetOutputPin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin);
    LL_GPIO_SetOutputPin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin);

    // 4. 软件复位
    if (max77812_write_reg(MAX77812_PMIC1, pmic_reset_cmd[0], pmic_reset_cmd[1]) != HAL_OK ||
        max77812_write_reg(MAX77812_PMIC2, pmic_reset_cmd[0], pmic_reset_cmd[1]) != HAL_OK) {
        power_error_fatal();
    }

    // 写ILIM、FPWM
    // PMIC1
    max77812_write_and_verify(MAX77812_PMIC1, 0x2F, 0xF3);  // M1
    max77812_write_and_verify(MAX77812_PMIC1, 0x31, 0xF3);  // M3
    // PMIC2
    max77812_write_and_verify(MAX77812_PMIC2, 0x2F, 0xF3);  // M1
    max77812_write_and_verify(MAX77812_PMIC2, 0x30, 0xF3);  // M2
    max77812_write_and_verify(MAX77812_PMIC2, 0x31, 0xF3);  // M3
    max77812_write_and_verify(MAX77812_PMIC2, 0x32, 0xF3);  // M4

    // 5. 关全部通道
    max77812_write_and_verify(MAX77812_PMIC1, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);
    max77812_write_and_verify(MAX77812_PMIC2, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);

    // 6. 设置PMIC2 M2
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m2_max_buffr[0], pmic2_m2_max_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, PMIC2_VOUT_MXV);
    wait_pmic_pok(MAX77812_PMIC2);

    // 7. 设置PMIC2 M3 M4
    // max77812_write_and_verify(MAX77812_PMIC2, pmic2_m3_1v1_buffr[0], pmic2_m3_1v1_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m3_1v15_buffr[0], pmic2_m3_1v15_buffr[1]);
    // max77812_write_and_verify(MAX77812_PMIC2, pmic2_m4_06v_buffr[0], pmic2_m4_06v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m4_1v1_buffr[0], pmic2_m4_1v1_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, PMIC2_VOUT_MXV | PMIC2_VOUT_06V | PMIC2_VOUT_11V);
    wait_pmic_pok(MAX77812_PMIC2);

    // 8. 设置PMIC1 M1
    // max77812_write_and_verify(MAX77812_PMIC1, pmic1_m1_08v_buffr[0], pmic1_m1_08v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_m1_085v_buffr[0], pmic1_m1_085v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, PMIC1_VOUT_08V);
    wait_pmic_pok(MAX77812_PMIC1);

    // 9. 设置PMIC2 M1
    // max77812_write_and_verify(MAX77812_PMIC2, pmic2_m1_10v_buffr[0], pmic2_m1_10v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m1_105v_buffr[0], pmic2_m1_105v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL,
                              PMIC2_VOUT_10V | PMIC2_VOUT_MXV | PMIC2_VOUT_06V | PMIC2_VOUT_11V);
    wait_pmic_pok(MAX77812_PMIC2);

    // 10. 设置PMIC1 M3
    // max77812_write_and_verify(MAX77812_PMIC1, pmic1_m3_06v_buffr[0], pmic1_m3_06v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_m3_065v_buffr[0], pmic1_m3_065v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, PMIC1_VOUT_06V | PMIC1_VOUT_08V);
    wait_pmic_pok(MAX77812_PMIC1);

    // 11. 拉高AX_RST_OUT
    LL_mDelay(10);
    LL_GPIO_SetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin);

    // 12. 风扇全速
    LL_TIM_OC_SetCompareCH1(TIM21, FAN_COMPARE_VALUE_MAX);

    // 13. RUN灯
    // LL_GPIO_SetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin);
    LL_GPIO_ResetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);
    // LL_mDelay(500);
    // LL_GPIO_ResetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin);
}

void system_power_shutdown(void){

	// 关掉 PMIC1 M3
	max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, PMIC1_VOUT_08V);
	max77812_write_and_verify(MAX77812_PMIC1, pmic1_m3_065v_buffr[0], 0);

	// 关掉 PMIC2 M1
	max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL,PMIC2_VOUT_MXV | PMIC2_VOUT_06V | PMIC2_VOUT_11V);
	max77812_write_and_verify(MAX77812_PMIC2, pmic2_m1_105v_buffr[0], 0);

	// 关掉 PMIC1 M1
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL,0);
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_m1_085v_buffr[0], 0);

    // 关掉 PMIC2 M3 M4
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, PMIC2_VOUT_MXV);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m3_1v15_buffr[0], 0);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m4_1v1_buffr[0], 0);

    // 关掉 PMIC2 M2
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, 0);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m2_max_buffr[0], 0);

    // 关闭PMIC1、PMIC2的使能
    LL_GPIO_ResetOutputPin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin);
    LL_GPIO_ResetOutputPin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin);

}

static inline void read_bit7_low5(uint8_t value, uint8_t *bit7, uint8_t *low5)
{
    if (bit7) *bit7 = (value >> 7) & 0x01;  // 取第 7 位（bit7）
    if (low5) *low5 = value & 0x1F;         // 取低 5 位
}

static void pmic_fine_tuning(uint8_t value, uint16_t base_voltage, max77812_pmic_t pmic_id, uint8_t reg_addr)
{
    uint8_t bit7, low5;
    read_bit7_low5(value, &bit7, &low5);

    if(low5 > 20){
    	low5 = 20;
    }
    // 低5位调节值（乘5 = (<< 2) + 自身）
    uint16_t adj = (low5 << 2) + low5;

    uint16_t final;
    if (bit7) {
        // bit7 = 1 => 减调
        final = (base_voltage - adj - 250) / 5;
    } else {
        // bit7 = 0 => 加调
        final = (base_voltage + adj - 250) / 5;
    }

    max77812_write_and_verify(pmic_id, reg_addr, final);
    wait_pmic_pok(pmic_id);
}

void pmic1_m1_fine_tuning(uint8_t value)
{
    pmic_fine_tuning(value, 850, MAX77812_PMIC1, 0x23);
}

void pmic1_m3_fine_tuning(uint8_t value)
{
    pmic_fine_tuning(value, 650, MAX77812_PMIC1, 0x25);
}

void pmic2_m1_fine_tuning(uint8_t value)
{
    pmic_fine_tuning(value, 1050, MAX77812_PMIC2, 0x23);
}

void pmic2_m3_fine_tuning(uint8_t value)
{
    pmic_fine_tuning(value, 1150, MAX77812_PMIC2, 0x25);
}
