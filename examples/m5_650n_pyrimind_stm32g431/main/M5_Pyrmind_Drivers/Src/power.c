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

/******************************* PMIC1 配置 (2+2 Phase) **********************/
/* VDD Core (M1) - 0.8V */
const uint8_t pmic1_vdd_core_buffr[2] = {
    0x23,  // M1输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x03)
    0x6E   // (0.8-0.25)/0.005 = 0x6E
};

/* NNVDD (M3) - 0.6V */
const uint8_t pmic1_nnvdd_buffr[2] = {
    0x25,  // M3输出电压寄存器 (PMIC_REG_OUTPUT_CTRL + 0x05)
    0x46   // (0.6-0.25)/0.005 = 0x46
};

/******************************* PMIC2 配置 (2+1+1 Phase) ********************/
/* VDD CPU (M1) - 1.0V */
const uint8_t pmic2_vdd_cpu_buffr[2] = {
    0x23,  // M1输出电压寄存器
    0x96   // (1.0-0.25)/0.005 = 0x96
};

/* VDDR 1V1 (M3) - 1.1V */
const uint8_t pmic2_vddr_1v1_buffr[2] = {
    0x25,  // M3输出电压寄存器
    0xAA   // (1.1-0.25)/0.005 = 0xAA
};

/* VDDR 0V6 (M4) - 0.6V */
const uint8_t pmic2_vddr_0v6_buffr[2] = {
    0x26,  // M4输出电压寄存器
    0x46   // (0.6-0.25)/0.005 = 0x46
};

static uint16_t vdd_cpu         = VDD_CORR_DEFAULT;
static bool vdd_cpu_update_flag = false;
static uint16_t power_off_time  = SYSTEM_POWER_ON_TIMER_MIN;

static HAL_StatusTypeDef max77812_write_and_verify(max77812_pmic_t pmic, uint8_t reg, uint8_t value)
{
    uint8_t read_val = 0;
    HAL_StatusTypeDef status;

    while (1) {
        status = max77812_write_reg(pmic, reg, value);
        if (status != HAL_OK) {
#ifdef POWER_DEBUG
            printf("PMIC%d WRITE 0x%02X ERROR\r\n", pmic, reg);
#endif
            HAL_Delay(500);
            continue;
        }

        status = max77812_read_reg(pmic, reg, &read_val);
        if (status != HAL_OK) {
#ifdef POWER_DEBUG
            printf("PMIC%d READ 0x%02X ERROR\r\n", pmic, reg);
#endif
            HAL_Delay(500);
            continue;
        }

        if (read_val == value) {
#ifdef POWER_DEBUG
            printf("PMIC%d VERIFY SUCCESE REG = 0x%02X: WRITE 0x%02X == READ 0x%02X\r\n", pmic + 1, reg, value,
                   read_val);
#endif
            break;  // 写入成功并校验通过
        }

#ifdef POWER_DEBUG
        printf("PMIC%d VERIFY FAIL: WRITE 0x%02X != READ 0x%02X\r\n", pmic, value, read_val);
#endif
        HAL_Delay(500);
    }

    return HAL_OK;
}

// 循环读取某寄存器
void loop_readback(void)
{
    while (1) {
        uint8_t temp = 0;
        if (max77812_read_reg(MAX77812_PMIC1, 0x05, &temp) == HAL_OK) {
            printf("0x05 = 0x%02X (%d)\r\n", temp, temp);
        } else {
            printf("read error\r\n");
        }
        HAL_Delay(100);
    }
}

//* tage 1： 系统上电
//* 打开 **内部5V降压使能**， 等待10ms
//* 打开 **PMIC1 使能**和**PMIC2 使能**，并初始化电压配置
//* 打开 **1.8V降压使能**，等待 **1.8V PowerGood**，并等待1ms
//* 打开 **3.3V降压使能**，等待 **3.3V PowerGood**
//* 打开 **DDRV1V1**和**DDRV0V6**，并等待PG
//* 打开 **VDD CORE**，并等待PG
//* 打开 **VDD CPU**，并等待PG
//* 打开 **VDD NN**，并等待PG
//* 等待10ms，拉高 **SoC复位输出**
//* 打开风扇PWM
static void _system_power_sequence(bool boot_mode)
{
    // 打开 **内部5V降压使能**， 等待10ms
    HAL_GPIO_WritePin(V5V_PWR_EN_GPIO_Port, V5V_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PA15
    HAL_Delay(100);

    // 打开 **PMIC1 使能**和**PMIC2 使能**，并初始化电压配置
    HAL_GPIO_WritePin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin, GPIO_PIN_SET);  // 拉高 PA4
    HAL_GPIO_WritePin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin, GPIO_PIN_SET);  // 拉高 PA1

    while (max77812_write_reg(MAX77812_PMIC1, pmic_reset_cmd[0], pmic_reset_cmd[1]) !=
           HAL_OK) {  // 初始化 PMIC1（复位）
#ifdef POWER_DEBUG
        printf("MAX77812_PMIC1 RESET ERROR\r\n");
#endif
        HAL_Delay(500);
    }

    max77812_write_and_verify(MAX77812_PMIC1, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_vdd_core_buffr[0], pmic1_vdd_core_buffr[1]);  // VDD Core
    max77812_write_and_verify(MAX77812_PMIC1, pmic1_nnvdd_buffr[0], pmic1_nnvdd_buffr[1]);        // NNVDD

    while (max77812_write_reg(MAX77812_PMIC2, pmic_reset_cmd[0], pmic_reset_cmd[1]) !=
           HAL_OK) {  // 初始化 PMIC2（复位）
#ifdef POWER_DEBUG
        printf("MAX77812_PMIC2 RESET ERROR\r\n");
#endif
        HAL_Delay(500);
    }
    max77812_write_and_verify(MAX77812_PMIC2, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_vdd_cpu_buffr[0], pmic2_vdd_cpu_buffr[1]);    // VDD CPU
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_vddr_1v1_buffr[0], pmic2_vddr_1v1_buffr[1]);  // VDDR 1V1
    max77812_write_and_verify(MAX77812_PMIC2, pmic2_vddr_0v6_buffr[0], pmic2_vddr_0v6_buffr[1]);  // VDDR 0V6

    // 打开 **1.8V降压使能**，等待 **1.8V PowerGood**，并等待1ms
    HAL_GPIO_WritePin(VDD18_EN_GPIO_Port, VDD18_EN_Pin, GPIO_PIN_SET);           // 拉高 PB1
    while (HAL_GPIO_ReadPin(VDD18_PG_GPIO_Port, VDD18_PG_Pin) == GPIO_PIN_SET);  // 等待 PF1 稳定高
    HAL_Delay(10);

    // 打开 **3.3V降压使能**，等待 **3.3V PowerGood**，并等待1ms
    HAL_GPIO_WritePin(VDD3_EN_GPIO_Port, VDD3_EN_Pin, GPIO_PIN_SET);           // 拉高 PB0
    while (HAL_GPIO_ReadPin(VDD3_PG_GPIO_Port, VDD3_PG_Pin) == GPIO_PIN_SET);  // 等待 PB6 稳定高
    HAL_Delay(10);

    // 打开 **DDRV1V1**和**DDRV0V6**，并等待PG
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, DDRV1V1_PHASE | DDRV0V6_PHASE);
    HAL_Delay(1);

    // 打开 **VDD CORE**，并等待PG
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, VDD_CORE_PHASE);
    HAL_Delay(1);

    // 打开 **VDD CPU**，并等待PG
    max77812_write_and_verify(MAX77812_PMIC2, PMIC_REG_EN_CTRL, DDRV1V1_PHASE | DDRV0V6_PHASE | VDD_CPU_PHASE);
    HAL_Delay(1);

    // 打开 **VDD NN**，并等待PG
    max77812_write_and_verify(MAX77812_PMIC1, PMIC_REG_EN_CTRL, VDD_CORE_PHASE | VDD_NN_PHASE);

    // 等待10ms
    HAL_Delay(10);

    // SoC复位逻辑
    if (boot_mode) {
        if (HAL_GPIO_ReadPin(EC_BUTTON_GPIO_Port, EC_BUTTON_Pin) == GPIO_PIN_RESET) {
            HAL_GPIO_WritePin(DL_MODE_GPIO_Port, DL_MODE_Pin, GPIO_PIN_SET);
            HAL_Delay(100);
            HAL_GPIO_WritePin(SYS_RST_GPIO_Port, SYS_RST_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(DL_MODE_GPIO_Port, DL_MODE_Pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(SYS_RST_GPIO_Port, SYS_RST_Pin, GPIO_PIN_SET);
        }
        set_fan_pwm(FAN_PWM_DEFINE);
    } else {
        HAL_GPIO_WritePin(SYS_RST_GPIO_Port, SYS_RST_Pin, GPIO_PIN_SET);
    }
}

void system_power_boot(void)
{
    _system_power_sequence(true);
}

void system_power_on(void)
{
    _system_power_sequence(false);
}

void system_power_off(void)
{
    HAL_GPIO_WritePin(SYS_RST_GPIO_Port, SYS_RST_Pin, GPIO_PIN_RESET);  // 拉低关闭
    HAL_Delay(1);

    // 关闭 **3.3V降压使能**
    HAL_GPIO_WritePin(VDD3_EN_GPIO_Port, VDD3_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB0
    HAL_Delay(1);

    // 关闭 **1.8V降压使能**
    HAL_GPIO_WritePin(VDD18_EN_GPIO_Port, VDD18_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB1
    HAL_Delay(1);

    max77812_write_and_verify(MAX77812_PMIC2, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出
    max77812_write_and_verify(MAX77812_PMIC1, pmic_disable_all_cmd[0], pmic_disable_all_cmd[1]);  // 关闭输出
    HAL_Dealy(1);

    // 关闭 **PMIC1 使能**和**PMIC2 使能**
    HAL_GPIO_WritePin(PMIC1_CE_GPIO_Port, PMIC1_CE_Pin, GPIO_PIN_RESET);  // 拉低 PA4
    HAL_GPIO_WritePin(PMIC2_CE_GPIO_Port, PMIC2_CE_Pin, GPIO_PIN_RESET);  // 拉低 PA1
    HAL_Dealy(1);

    // 关闭 **内部5V降压使能**， 等待10ms
    HAL_GPIO_WritePin(V5V_PWR_EN_GPIO_Port, V5V_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PA15
    HAL_Delay(1);
}

//* Stage 2: 外设上电
//* 打开 **外部5V降压使能**，等待10ms
//* 拉高 **USB3.0 HUB复位**
//* 拉低 **USB3.0 DS1口输出使能**，**USB3.0 DS2口输出使能**和**USB3.0 DS3口输出使能**
//* 拉高 **USB3.0 DS1口大电流模式**和**USB3.0 DS2口大电流模式**
//* 拉低 **Grove Port1输出使能**和**Grove Port3输出使能**
//* 拉高 **NGFF PCIE时钟使能**，并等待10ms
//* 拉高 **NGFF1口输出使能**和**NGFF2口输出使能**
void peripheral_power_boot(void)
{
    //* 打开 **外部5V降压使能**，等待10ms
    HAL_GPIO_WritePin(V5VE_PWR_EN_GPIO_Port, V5VE_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PA11
    HAL_Delay(100);

    //* 拉高 **USB3.0 HUB复位**
    HAL_GPIO_WritePin(GL_RESET_J_GPIO_Port, GL_RESET_J_Pin, GPIO_PIN_SET);  // 拉高  PB11

    //* 拉低 **USB3.0 DS1口输出使能**，**USB3.0 DS2口输出使能**和**USB3.0 DS3口输出使能**
    HAL_GPIO_WritePin(DS1_PWR_EN_GPIO_Port, DS1_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB13
    HAL_GPIO_WritePin(DS2_PWR_EN_GPIO_Port, DS2_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB14
    HAL_GPIO_WritePin(DS3_PWR_EN_GPIO_Port, DS3_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PC11

    //* 拉高 **USB3.0 DS1口大电流模式**和**USB3.0 DS2口大电流模式**
    HAL_GPIO_WritePin(DS1_HS_EN_GPIO_Port, DS1_HS_EN_Pin, GPIO_PIN_SET);  // 拉高 PB12
    HAL_GPIO_WritePin(DS2_HS_EN_GPIO_Port, DS2_HS_EN_Pin, GPIO_PIN_SET);  // 拉高 PB15

    //* 拉低 **Grove Port1输出使能**和**Grove Port3输出使能**
    HAL_GPIO_WritePin(PWR_GRV1_EN_GPIO_Port, PWR_GRV1_EN_Pin, GPIO_PIN_RESET);  // 拉低 PC14
    HAL_GPIO_WritePin(PWR_GRV3_EN_GPIO_Port, PWR_GRV3_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB7

    //* 拉高 **NGFF PCIE时钟使能**，并等待10ms
    HAL_GPIO_WritePin(PCIE_REFC_EN_GPIO_Port, PCIE_REFC_EN_Pin, GPIO_PIN_SET);  // 拉高 PB10
    HAL_Delay(10);

    //* 拉高 **NGFF1口输出使能**和**NGFF2口输出使能**
    HAL_GPIO_WritePin(NGFF1_PWR_EN_GPIO_Port, NGFF1_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PB2
    HAL_GPIO_WritePin(NGFF2_PWR_EN_GPIO_Port, NGFF2_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PB4
}

void peripheral_power_on(void)
{
    peripheral_power_boot();
}

//* 外设下电
//* 关闭 **外部5V降压使能**，等待10ms
//* 拉低 **USB3.0 HUB复位**
//* 拉高 **USB3.0 DS1口输出使能**，**USB3.0 DS2口输出使能**和**USB3.0 DS3口输出使能**
//* 拉低 **USB3.0 DS1口大电流模式**和**USB3.0 DS2口大电流模式**
//* 拉高 **Grove Port1输出使能**和**Grove Port3输出使能**
//* 拉低 **NGFF PCIE时钟使能**，并等待10ms
//* 拉低 **NGFF1口输出使能**和**NGFF2口输出使能**
void peripheral_power_off(void)
{
    //* 关闭 **外部5V降压使能**，等待10ms
    HAL_GPIO_WritePin(V5VE_PWR_EN_GPIO_Port, V5VE_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PA11
    HAL_Delay(100);

    //* 拉低 **USB3.0 HUB复位**
    HAL_GPIO_WritePin(GL_RESET_J_GPIO_Port, GL_RESET_J_Pin, GPIO_PIN_RESET);  // 拉低 PB11

    //* 拉高 **USB3.0 DS1口输出使能**，**USB3.0 DS2口输出使能**和**USB3.0 DS3口输出使能**
    HAL_GPIO_WritePin(DS1_PWR_EN_GPIO_Port, DS1_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PB13
    HAL_GPIO_WritePin(DS2_PWR_EN_GPIO_Port, DS2_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PB14
    HAL_GPIO_WritePin(DS3_PWR_EN_GPIO_Port, DS3_PWR_EN_Pin, GPIO_PIN_SET);  // 拉高 PC11

    //* 拉低 **USB3.0 DS1口大电流模式**和**USB3.0 DS2口大电流模式**
    HAL_GPIO_WritePin(DS1_HS_EN_GPIO_Port, DS1_HS_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB12
    HAL_GPIO_WritePin(DS2_HS_EN_GPIO_Port, DS2_HS_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB15

    //* 拉高 **Grove Port1输出使能**和**Grove Port3输出使能**
    HAL_GPIO_WritePin(PWR_GRV1_EN_GPIO_Port, PWR_GRV1_EN_Pin, GPIO_PIN_SET);  // 拉高 PC14
    HAL_GPIO_WritePin(PWR_GRV3_EN_GPIO_Port, PWR_GRV3_EN_Pin, GPIO_PIN_SET);  // 拉高 PB7

    //* 拉低 **NGFF PCIE时钟使能**，并等待10ms
    HAL_GPIO_WritePin(PCIE_REFC_EN_GPIO_Port, PCIE_REFC_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB10
    HAL_Delay(10);

    //* 拉低 **NGFF1口输出使能**和**NGFF2口输出使能**
    HAL_GPIO_WritePin(NGFF1_PWR_EN_GPIO_Port, NGFF1_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB2
    HAL_GPIO_WritePin(NGFF2_PWR_EN_GPIO_Port, NGFF2_PWR_EN_Pin, GPIO_PIN_RESET);  // 拉低 PB4
}

bool system_power_off_time_check(uint16_t power_off_timer)
{
    if (power_off_timer >= SYSTEM_POWER_ON_TIMER_MIN) {
        return true;
    }
    return false;
}

void set_system_power_off_time(uint16_t time)
{
    power_off_time = time;
}

bool vdd_core_check(uint16_t vdd_core)
{
    if (vdd_core >= VDD_CORE_MIN && vdd_core <= VDD_CORE_MAX) {
        return true;
    }
    return false;
}

void set_vdd_cpu(uint16_t valtage)
{
    if (vdd_cpu != valtage) {
        vdd_cpu_update_flag = true;
        vdd_cpu             = valtage;
    }
}

void update_vdd_cup(void)
{
    if (vdd_cpu_update_flag) {
        vdd_cpu_update_flag = false;
        // 干什么？
    }
}
