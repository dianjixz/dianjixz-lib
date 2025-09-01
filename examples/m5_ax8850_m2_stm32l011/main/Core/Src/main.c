/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "power.h"
#include "i2c_ex.h"
#include "stm32l0xx_ll_adc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
    uint8_t reg_addr;
    uint8_t data_len;
    const uint8_t *data_ptr;
} reg_info_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FAN_PWM_DUTY_REG    (0x00)  // R/W
#define FAN_RPM_REG         (0x10)  // R
#define VIN_SYS_VOLTAGE_REG (0x20)  // R
#define TEMPERATURE_REG     (0x30)  // R
#define PMIC_VOLT_FINE_REG  (0x40)  // R/W
#define HW_VERSION_REG      (0xFD)  // R
#define SW_VERSION_REG      (0xFE)  // R
#define I2C_ADDRESS_REG     (0xFF)  // R

#define FAN_PWM_DUTY_MIN    (20)
#define FAN_PWM_DUTY_MAX    (100)
#define FW_PWM_DUTY_DEFAULT (0x64)
#define HW_VERSION_DEFAULT  (0x01)
#define SW_VERSION_DEFAULT  (0x01)
#define I2C_DEFAULT_ADDR    (0x5C)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
__IO uint8_t fan_pwm_duty_reg      = FW_PWM_DUTY_DEFAULT;
__IO uint16_t fan_rpm_reg          = 0;
__IO uint16_t vin_sys_voltage_reg  = 0;
__IO uint16_t temperature_reg      = 0;
__IO uint8_t pmic_volt_fine_reg[4] = {0};
__IO uint8_t version_reg[2]        = {HW_VERSION_DEFAULT, SW_VERSION_DEFAULT};
__IO uint8_t i2c_address_reg       = I2C_DEFAULT_ADDR;

__IO uint8_t map_pmic[2] = {0};

__IO uint8_t fan_pwm_duty_update_flag      = 0;
__IO uint8_t pmic_volt_fine_update_flag[4] = {0};

__IO uint8_t tx_buf[4]                        = {0};
__IO uint32_t i2c_stop_timeout_delay          = 0;
__IO uint16_t adc_value_buf[ADC_CHANNEL_NUMS] = {0};
__IO uint32_t adc_collect_timer               = 0;
__IO uint32_t crr1 = 0, crr2 = 0, crr3 = 0;
__IO uint32_t freq       = 0;
__IO uint32_t duty       = 0;
__IO uint8_t measure_cnt = 1;
__IO uint32_t fan_rpm_timer;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void capture(void)
{
    uint32_t diff1 = 0, diff2 = 0;
    uint32_t max_cnt = __HAL_TIM_GET_AUTORELOAD(&htim2);

    if (crr2 >= crr1){
    	diff1 = crr2 - crr1;
    }else{
    	diff1 = (max_cnt + 1) + crr2 - crr1;
    }

    if (crr3 >= crr1){
    	diff2 = crr3 - crr1;
    }else{
    	diff2 = (max_cnt + 1) + crr3 - crr1;
    }

    if (diff2 == 0) {
        freq        = 0;
        fan_rpm_reg = 0;
        duty        = 0;
        return;
    }

    freq        = TIMER_CLOCK_HZ / diff2;
    duty        = diff1 * 100 / diff2;
    fan_rpm_reg = freq * (60 / FAN_PULSE_PER_REV);
}

static void fan_pwm_duty_update(void)
{
    uint16_t compare_value = (uint16_t)(fan_pwm_duty_reg * 16);
    LL_TIM_OC_SetCompareCH1(TIM21, compare_value);
}

static void check_pcie_rst_event(void)
{
    static GPIO_PinState lastState = GPIO_PIN_SET;  // 上一次状态，初始为高电平
    GPIO_PinState currentState     = HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin);

    if (currentState != lastState) {
        GPIO_PinState confirmState = HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin);

        if (confirmState == currentState) {
            lastState = currentState;  // 确认变化后再更新状态缓存

            if (currentState == GPIO_PIN_RESET) {
                // RST 被拉低,拉低8850的复位，等待10ms之后拉高
                LL_GPIO_ResetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin);  // 拉低RST

                // 关PMIC所有电源
                system_power_shutdown();

                LL_mDelay(10);

                // 软件复位
                NVIC_SystemReset();

                while(1){

                }

//                LL_GPIO_SetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin);  // 拉高RST
            }
        }
    }
}

static void adc_read_all_channels(void)
{
    uint32_t sum[ADC_CHANNEL_NUMS] = {0};

    if (HAL_ADC_Start(&hadc) != HAL_OK) {
        return;
    }

    for (uint8_t sample = 0; sample < ADC_SAMPLE_COUNT; sample++) {
        for (uint8_t ch = 0; ch < ADC_CHANNEL_NUMS; ch++) {
            if (HAL_ADC_PollForConversion(&hadc, 100) == HAL_OK) {
                sum[ch] += HAL_ADC_GetValue(&hadc);
            }
        }
    }

    HAL_ADC_Stop(&hadc);

    for (uint8_t ch = 0; ch < ADC_CHANNEL_NUMS; ch++) {
        adc_value_buf[ch] = (uint16_t)(sum[ch] / ADC_SAMPLE_COUNT);
    }
}

static void adc_value_process(void)
{
    uint16_t vref_mV    = __LL_ADC_CALC_VREFANALOG_VOLTAGE(adc_value_buf[1], LL_ADC_RESOLUTION_12B);
    vin_sys_voltage_reg = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_mV, adc_value_buf[0], LL_ADC_RESOLUTION_12B) * 2;
    temperature_reg =
        __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(1610, 670, 130, vref_mV, adc_value_buf[2], LL_ADC_RESOLUTION_12B);
}

static void handle_write_operation(uint8_t reg, uint8_t *rx_data, uint16_t len)
{
    if (reg == FAN_PWM_DUTY_REG && len == 2) {
        uint8_t new_duty = rx_data[1];
        if (fan_pwm_duty_reg != new_duty && new_duty >= FAN_PWM_DUTY_MIN && new_duty <= FAN_PWM_DUTY_MAX) {
            fan_pwm_duty_reg         = new_duty;
            fan_pwm_duty_update_flag = 1;
        }
        return;
    }

    if (reg >= PMIC_VOLT_FINE_REG && reg <= (PMIC_VOLT_FINE_REG + 3)) {
        uint8_t start_idx   = reg - PMIC_VOLT_FINE_REG;
        uint8_t max_count   = 4 - start_idx;
        uint8_t write_count = len - 1;

        if (write_count > max_count) write_count = max_count;

        for (uint8_t i = 0; i < write_count; i++) {
            uint8_t idx     = start_idx + i;
            uint8_t new_val = rx_data[1 + i];
            uint8_t low7    = new_val & 0x7F;

            if (pmic_volt_fine_reg[idx] != new_val && low7 <= 20) {
                pmic_volt_fine_reg[idx]         = new_val;
                pmic_volt_fine_update_flag[idx] = 1;
            }
        }
    }
}

static void handle_read_operation(uint8_t reg)
{
    static const reg_info_t reg_table[] = {{FAN_RPM_REG, 2, (uint8_t *)&fan_rpm_reg},
                                           {VIN_SYS_VOLTAGE_REG, 2, (uint8_t *)&vin_sys_voltage_reg},
                                           {TEMPERATURE_REG, 2, (uint8_t *)&temperature_reg},
                                           {HW_VERSION_REG, 2, version_reg}};

    if (reg == FAN_PWM_DUTY_REG) {
        tx_buf[0] = fan_pwm_duty_reg;
        i2c1_set_send_data(tx_buf, 1);
        return;
    }

    if (reg == I2C_ADDRESS_REG) {
        tx_buf[0] = i2c_address_reg;
        i2c1_set_send_data(tx_buf, 1);
        return;
    }

//    if (reg == 0x50) {
//        tx_buf[0] = map_pmic[0];
//        i2c1_set_send_data(tx_buf, 1);
//        return;
//    }
//
//    if (reg == 0x60) {
//        tx_buf[0] = map_pmic[1];
//        i2c1_set_send_data(tx_buf, 1);
//        return;
//    }

    if (reg >= PMIC_VOLT_FINE_REG && reg <= (PMIC_VOLT_FINE_REG + 3)) {
        uint8_t start_idx = reg - PMIC_VOLT_FINE_REG;
        uint8_t data_len  = 4 - start_idx;

        for (uint8_t i = 0; i < data_len; i++) {
            tx_buf[i] = pmic_volt_fine_reg[start_idx + i];
        }
        i2c1_set_send_data(tx_buf, data_len);
        return;
    }

    for (uint8_t i = 0; i < sizeof(reg_table) / sizeof(reg_table[0]); i++) {
        const reg_info_t *info = &reg_table[i];
        if (reg >= info->reg_addr && reg <= (info->reg_addr + info->data_len - 1)) {
            uint8_t offset   = reg - info->reg_addr;
            uint8_t data_len = info->data_len - offset;
            if (info->data_len == 2) {
                uint16_t val = *(uint16_t *)info->data_ptr;
                tx_buf[0]    = val & 0xFF;
                tx_buf[1]    = (val >> 8) & 0xFF;
                i2c1_set_send_data(tx_buf + offset, data_len);
            } else {
                memcpy(tx_buf, info->data_ptr + offset, data_len);
                i2c1_set_send_data(tx_buf, data_len);
            }
            return;
        }
    }
    tx_buf[0] = 0xFF;
    i2c1_set_send_data(tx_buf, 1);
}

void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
    if (len == 0) return;

    uint8_t reg = rx_data[0];

    if (len > 1) {
        handle_write_operation(reg, rx_data, len);
    } else {
        handle_read_operation(reg);
    }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_TIM2_Init();
    MX_TIM21_Init();
    // MX_IWDG_Init();
    /* USER CODE BEGIN 2 */
    user_i2c_init();
    i2c1_it_enable();
    LL_TIM_EnableCounter(TIM21);
    LL_TIM_CC_EnableChannel(TIM21, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_SetCompareCH1(TIM21, 0);
    system_power_boot();
    HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
    MX_IWDG_Init();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        check_pcie_rst_event();

        if (fan_pwm_duty_update_flag) {
            fan_pwm_duty_update();
            fan_pwm_duty_update_flag = 0;
        }

        if (pmic_volt_fine_update_flag[0]) {
            pmic1_m1_fine_tuning(pmic_volt_fine_reg[0]);
            pmic_volt_fine_update_flag[0] = 0;
        }

        if (pmic_volt_fine_update_flag[1]) {
            pmic1_m3_fine_tuning(pmic_volt_fine_reg[1]);
            pmic_volt_fine_update_flag[1] = 0;
        }

        if (pmic_volt_fine_update_flag[2]) {
            pmic2_m1_fine_tuning(pmic_volt_fine_reg[2]);
            pmic_volt_fine_update_flag[2] = 0;
        }

        if (pmic_volt_fine_update_flag[3]) {
            pmic2_m3_fine_tuning(pmic_volt_fine_reg[3]);
            pmic_volt_fine_update_flag[3] = 0;
        }

        if (HAL_GetTick() - adc_collect_timer > ADC_COLLECT_INTERVAL_MS) {
            adc_collect_timer = HAL_GetTick();
            adc_read_all_channels();
            adc_value_process();

//            max77812_read_reg(MAX77812_PMIC1,0x05,&map_pmic[0]);
//            max77812_read_reg(MAX77812_PMIC2,0x05,&map_pmic[1]);

        }

        if ((HAL_GetTick() - fan_rpm_timer) > SAMPLE_INTERVAL) {
            capture();
            crr1 = crr2 = crr3 = 0;
            fan_rpm_timer      = HAL_GetTick();
            HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
        }

        i2c_timeout_counter = 0;
        if (i2c_stop_timeout_flag) {
            if (i2c_stop_timeout_delay < HAL_GetTick()) {
                i2c_stop_timeout_counter++;
                i2c_stop_timeout_delay = HAL_GetTick() + 10;
            }
        }
        if (i2c_stop_timeout_counter > 50) {
            LL_I2C_DeInit(I2C1);
            LL_I2C_DisableAutoEndMode(I2C1);
            LL_I2C_Disable(I2C1);
            LL_I2C_DisableIT_ADDR(I2C1);
            user_i2c_init();
            i2c1_it_enable();
            LL_mDelay(500);
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        LL_IWDG_ReloadCounter(IWDG);
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    while (LL_PWR_IsActiveFlag_VOS() != 0) {
    }
    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() != 1) {
    }
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_LSI_Enable();

    /* Wait till LSI is ready */
    while (LL_RCC_LSI_IsReady() != 1) {
    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    LL_SetSystemCoreClock(32000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2) {
        uint32_t value = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
        switch (measure_cnt) {
            case 1:
                crr1 = value;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
                                              TIM_INPUTCHANNELPOLARITY_FALLING);
                measure_cnt = 2;
                break;

            case 2:
                crr2 = value;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
                                              TIM_INPUTCHANNELPOLARITY_RISING);
                measure_cnt = 3;
                break;

            case 3:
                crr3 = value;
                HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
                measure_cnt = 1;
                break;

            default:
                HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
                measure_cnt = 1;
                break;
        }
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
