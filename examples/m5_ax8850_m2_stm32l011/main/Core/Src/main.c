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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "power.h"
/* USER CODE END Includes */
static int time = 0;
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */




struct plc_reg_t
{
  unsigned char I0_0_MASTER:1;
  unsigned char I0_1_PCIE0_RST:1;
  unsigned char Q0_0_PWR_LED:1;
  unsigned char Q0_0_RUN_LED:1;
  unsigned char Q0_1_AX_RST:1;
  unsigned char Q0_2_VDD_CORE:1;
  unsigned char Q0_3_NNVDD:1;
  unsigned char Q0_3_VDD_CPU:1;
  unsigned char Q0_3_VDD_1_8V:1;
  unsigned char Q0_3_VDD_1_1V:1;
  unsigned char Q0_3_VDD_0_6V:1;
  unsigned short TON_0;//通电延时定时器
  unsigned short TOF_0;//断电延时定时器
  unsigned short TP_1;//脉冲定时器
} plc_reg;

static int plc_a(struct pt *pt) {
  static unsigned char old; 
  PT_BEGIN(pt);
  while (1) {
    if (plc_reg.I0_0_MASTER)
    {
      plc_reg.TON_0 = 100;
    }
    else
    {
      plc_reg.TON_0 = -1;
    }
    old = plc_reg.I0_0_MASTER;
    PT_YIELD_UNTIL(pt, old != plc_reg.I0_0_MASTER);
      


      // printf("LED ON\n");
      // delay(500); // 模拟500ms延时
      // printf("LED OFF\n");
      // delay(500); // 模拟500ms延时
      // PT_YIELD(pt); // 挂起、等待下一次调度
  }
  PT_END(pt);
}
static int plc_b(struct pt *pt) {
  static unsigned short old; 
  static unsigned short TON_0; 
  PT_BEGIN(pt);
  while (1) {
    if ((plc_reg.TON_0 != -1)&& plc_reg.I0_0_MASTER)
    {
        if(time - TON_0 > plc_reg.TON_0)
        {
          plc_reg.Q0_0_PWR_LED = 1;
          old = plc_reg.I0_0_MASTER;
          PT_YIELD_UNTIL(pt, (old != plc_reg.I0_0_MASTER)||(plc_reg.TON_0 == -1));
        }
        else
        {
          PT_YIELD(pt);
          continue;
        }
    }
    plc_reg.Q0_0_PWR_LED = 0;
    TON_0 = time;
    PT_YIELD(pt);
  }
  PT_END(pt);
}



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
void Check_PCIE_RST_Event(void)
{
    static GPIO_PinState lastState = GPIO_PIN_SET;  // 上一次状态，初始为高电平
    GPIO_PinState currentState = HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin);

    // 状态有变化
    if (currentState != lastState)
    {
        HAL_Delay(20);  // 简单消抖
        GPIO_PinState confirmState = HAL_GPIO_ReadPin(PCIE_RST_IN_GPIO_Port, PCIE_RST_IN_Pin);

        if (confirmState == currentState)
        {
            lastState = currentState;  // 确认变化后再更新状态缓存

            if (currentState == GPIO_PIN_RESET)
            {
                // RST 被拉低
                LL_GPIO_SetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);         // 开启电源灯
                LL_GPIO_ResetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin); // 拉低RST
            }
            else
            {
                // RST 恢复高电平
                LL_GPIO_ResetOutputPin(PWR_LED_GPIO_Port, PWR_LED_Pin);       // 关闭电源灯
                LL_GPIO_SetOutputPin(AX_RST_OUT_GPIO_Port, AX_RST_OUT_Pin);   // 拉高RST
            }
        }
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
  /* USER CODE BEGIN 2 */
  LL_TIM_EnableCounter(TIM21);   //使能计数
  LL_TIM_CC_EnableChannel(TIM21,LL_TIM_CHANNEL_CH1);  //使能输出比较通道
  LL_TIM_OC_SetCompareCH3(TIM21,0); // 占空比100%
  system_power_boot(); // 系统上电

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	Check_PCIE_RST_Event();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
