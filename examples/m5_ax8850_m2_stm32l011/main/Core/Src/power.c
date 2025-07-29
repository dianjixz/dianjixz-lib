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
/* (M1) - 0.8V */
const uint8_t pmic1_m1_08v_buffr[2] = {
    0x23,  // M1输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x03)
    0x6E   // (0.8-0.25)/0.005 = 0x6E
};

/* (M3) - 0.6V */
const uint8_t pmic1_m3_06v_buffr[2] = {
    0x25,  // M3输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x05)
    0x46   // (0.6-0.25)/0.005 = 0x46
};

/******************************* PMIC2 配置 ********************/
/* (M1) - 1.0V */
const uint8_t pmic2_m1_10v_buffr[2] = {
    0x23,  // M1输出电压寄存器
    0x96   // (1.0-0.25)/0.005 = 0x96
};

/* (M2) - maxV */
const uint8_t pmic2_m2_max_buffr[2] = {
    0x24,  // M2输出电压寄存器
    0xF9   // MAX
};

/* (M3) - 1.1V */
const uint8_t pmic2_m3_1v1_buffr[2] = {
    0x25,  // M3输出电压寄存器
    0xAA   // (1.1-0.25)/0.005 = 0xAA
};

/* (M4) - 0.6V */
const uint8_t pmic2_m4_06v_buffr[2] = {
    0x26,  // M4输出电压寄存器
    0x46   // (0.6-0.25)/0.005 = 0x46
};

static HAL_StatusTypeDef max77812_write_and_verify(max77812_pmic_t pmic, uint8_t reg, uint8_t value)
{
    uint8_t read_val = 0;
    HAL_StatusTypeDef status;

    while (1) {
        status = max77812_write_reg(pmic, reg, value);
        if (status != HAL_OK) {
            HAL_Delay(100);
            continue;
        }

        status = max77812_read_reg(pmic, reg, &read_val);
        if (status != HAL_OK) {
            HAL_Delay(100);
            continue;
        }

        if (read_val == value) {
            break;
        }

        HAL_Delay(100);
    }

    return HAL_OK;
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
    // 等待20ms
//    HAL_Delay(20);

//    // 等待PCIE0_RST_IN电平为高
//    while (HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin) == GPIO_PIN_RESET) {
//    }

    // 使能 PMIC1、PMIC2（拉高）
    LL_GPIO_SetOutputPin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin);
    LL_GPIO_SetOutputPin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin);

    // 软件复位PMIC1、PMIC2
    if (max77812_write_reg(MAX77812_PMIC1, pmic_reset_cmd[0], pmic_reset_cmd[1]) != HAL_OK ||
        max77812_write_reg(MAX77812_PMIC2, pmic_reset_cmd[0], pmic_reset_cmd[1]) != HAL_OK) {
        LL_GPIO_ResetOutputPin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin);
        LL_GPIO_ResetOutputPin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin);
        while (1) {
            // 拉高：LED亮
            LL_GPIO_SetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);
            HAL_Delay(1000);  
            // 拉低：LED灭
            LL_GPIO_ResetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);
            HAL_Delay(1000);  
        }
    }

    // 关闭PMIC1、PMIC2所有通道输出
    max77812_write_and_verify(MAX77812_PMIC1, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出
    max77812_write_and_verify(MAX77812_PMIC2, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出

    // 设置PMIC2 M2 MxVout到0xF9，并开启通道
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m2_max_buffr[0], pmic2_m2_max_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, PMIC2_VOUT_MXV);

    // 设置PMIC2 M3到1.1V，M4到0.6V，并同时开启两个通道
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m3_1v1_buffr[0], pmic2_m3_1v1_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m4_06v_buffr[0], pmic2_m4_06v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, PMIC2_VOUT_MXV | PMIC2_VOUT_06V | PMIC2_VOUT_11V);

    // 设置PMIC1 M1到0.8V，并开启通道
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_m1_08v_buffr[0], pmic1_m1_08v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, PMIC1_VOUT_08V);

    // 设置PMIC2 M1到1V，并开启通道
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_m1_10v_buffr[0], pmic2_m1_10v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL,
                              PMIC2_VOUT_10V | PMIC2_VOUT_MXV | PMIC2_VOUT_06V | PMIC2_VOUT_11V);

    // 设置PMIC1 M3到0.6V，并开启通道
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_m3_06v_buffr[0], pmic1_m3_06v_buffr[1]);
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, PMIC1_VOUT_06V | PMIC1_VOUT_08V);


    HAL_Delay(50); 
    // 拉高AX_RST_OUT
    LL_GPIO_SetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin);

    // 开启风扇PWM 100%
    LL_TIM_OC_SetCompareCH3(TIM21, 1280);  // 占空比100%

    // 拉高RUN LED
    LL_GPIO_SetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin);
    LL_GPIO_ResetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);
    HAL_Delay(500);
    LL_GPIO_ResetOutputPin(RUN_LED_GPIO_Port, RUN_LED_Pin);
}
