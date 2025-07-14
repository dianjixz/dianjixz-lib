/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _DEMO_H_
#define _DEMO_H_

#ifdef __cplusplus

extern "C" {
#endif

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stdio.h"
#include "config.h"
#include "rgb.h"
#include "ina3221.h"
#include "fan.h"
#include "power.h"
#include "usart.h"
#include <string.h>
#include "ip.h"
#include "led.h"
#include "flash.h"

/* ----------------------- Def44ines ------------------------------------------*/
// 线圈寄存器基地址和数量
#define MB_COIL_COUNT (18)

// 离散输入寄存器基地址和数量
#define MB_DISCRETE_INPUT_COUNT (5)

// 输入寄存器基地址和数量
#define MB_INPUT_REG_COUNT (14)

// 保持寄存器基地址和数量
#define MB_HOLDING_REG_START_OFFSET (1)     // Holding register start address
#define MB_HOLDING_REG_COUNT        (1023)  // Total number of holding registers

// 线圈寄存器定义
#define MB_COIL_EXT_5V_SUPPLY      (1)   // [RW] 外设5V供电 (PA11)
#define MB_COIL_INT_5V_MAIN_POWER  (2)   // [RW] 内部5V主电源，写0时延时关闭 (PA15)
#define MB_COIL_PCIE1_POWER        (3)   // [RW] PCIE1供电 (PB2)
#define MB_COIL_PCIE2_POWER        (4)   // [RW] PCIE2供电 (PB4)
#define MB_COIL_GROVE_PORT3_SUPPLY (5)   // [RW] Grove Port3供电 (PB7)
#define MB_COIL_CLK_MODE_SELECT    (6)   // [RW] 时钟输出模式，0=随PCIE，1=手动 (NC)
#define MB_COIL_PCIE_CLK_OUTPUT    (7)   // [RW] NGFF PCIE时钟输出 (PB10)
#define MB_COIL_GL3510_RESET       (8)   // [RW] GL3510复位 (PB11)
#define MB_COIL_DS1_HIGH_CURRENT   (9)   // [RW] DS1口大电流模式 (PB12)
#define MB_COIL_DS1_POWER          (10)  // [RW] DS1口电源 (PB13)
#define MB_COIL_DS2_POWER          (11)  // [RW] DS2口电源 (PB14)
#define MB_COIL_DS2_HIGH_CURRENT   (12)  // [RW] DS2口大电流模式 (PB15)
#define MB_COIL_DS3_POWER          (13)  // [RW] DS3口电源 (PC11)
#define MB_COIL_GROVE_PORT1_SUPPLY (14)  // [RW] Grove Port1供电 (PC14)
#define MB_COIL_GROVE_PORT2_SUPPLY (15)  // [RW] Grove Port2供电 (PC15)
#define MB_COIL_SAVE_COIL_CONFIG   (16)  // [WO] 保存相关配置 (NC)
#define MB_COIL_SAVE_HOLD_CONFIG   (17)  // [WO] 保存相关配置 (NC)
#define MB_COIL_POWER_OFF          (18)  // [WO] 关闭电源 (NC)

// 离散输入寄存器定义
#define MB_DISCRETE_INPUT_PCIE1_PRESENT  (1)  // [RO] PCIE1 设备存在信号 (PA12)
#define MB_DISCRETE_INPUT_PCIE2_PRESENT  (2)  // [RO] PCIE2 设备存在信号 (PB3)
#define MB_DISCRETE_INPUT_3V3_POWER_GOOD (3)  // [RO] 3.3V PowerGood反馈 (PB6)
#define MB_DISCRETE_INPUT_1V8_POWER_GOOD (4)  // [RO] 1.8V PowerGood反馈 (PF1)
#define MB_DISCRETE_INPUT_TOP_BUTTON     (5)  // [RO] 顶部按钮输入 (PC10)

// 输入寄存器定义
#define MB_INPUT_REG_PCIE1_VOLTAGE      (1)   // [RO] PCIE1 电压
#define MB_INPUT_REG_PCIE1_CURRENT      (2)   // [RO] PCIE1 电流
#define MB_INPUT_REG_PCIE2_VOLTAGE      (3)   // [RO] PCIE2 电压
#define MB_INPUT_REG_PCIE2_CURRENT      (4)   // [RO] PCIE2 电流
#define MB_INPUT_REG_USB1_VOLTAGE       (5)   // [RO] USB1 电压
#define MB_INPUT_REG_USB1_CURRENT       (6)   // [RO] USB1 电流
#define MB_INPUT_REG_USB2_VOLTAGE       (7)   // [RO] USB2 电压
#define MB_INPUT_REG_USB2_CURRENT       (8)   // [RO] USB2 电流
#define MB_INPUT_REG_INTERNAL5V_VOLTAGE (9)   // [RO] 内部5V 电压
#define MB_INPUT_REG_INTERNAL5V_CURRENT (10)  // [RO] 内部5V 电流
#define MB_INPUT_REG_EXTERNAL5V_VOLTAGE (11)  // [RO] 外部5V 电压
#define MB_INPUT_REG_EXTERNAL5V_CURRENT (12)  // [RO] 外部5V 电流
#define MB_INPUT_REG_FAN_SPEED          (13)  // [RO] 风扇实际转速
#define MB_INPUT_REG_FIRMWARE_VERSION   (14)  // [RO] 固件版本

// 保持寄存器定义
#define MB_HOLD_REG_MAIN_POWER_DELAY  (1)     // [RW] 主电源延时关闭时间
#define MB_HOLD_REG_FAN_SPEED_PERCENT (2)     // [RW] 风扇转速百分比
#define MB_HOLD_REG_VDD_CPU_VOLTAGE   (3)     // [RW] VDD_CPU 电压
#define MB_HOLD_REG_UART_BAUD_HIGH    (4)     // [RW] 串口波特率高字节
#define MB_HOLD_REG_UART_BAUD_LOW     (5)     // [RW] 串口波特率低字节
#define MB_HOLD_REG_HOST_IP_HIGH      (6)     // [RW] 主机IP高字节
#define MB_HOLD_REG_HOST_IP_LOW       (7)     // [RW] 主机IP低字节
#define MB_HOLD_REG_I2C_ADDR          (8)     // [RW] I2C地址寄存器（高字节地址/低字节寄存器）
#define MB_HOLD_REG_I2C_READ_CMD      (9)     // [RW] I2C读命令寄存器（操作后清除）
#define MB_HOLD_REG_I2C_WRITE_CMD     (10)    // [RW] I2C写命令寄存器（操作后清除）
#define MB_HOLD_REG_RGB_COUNT         (11)    // [RW] RGB灯数量
#define MB_HOLD_REG_RGB_MODE          (12)    // [RW] RGB灯模式
#define MB_HOLD_REG_RGB1_ARM_START    (13)    // [RW] RGB1灯映射起始地址
#define MB_HOLD_REG_RGB1_ARM_END      (86)    // [RW] RGB1灯映射结束地址
#define MB_HOLD_REG_LCD_MODE          (87)    // [RW] LCD显示模式
#define MB_HOLD_REG_LCD_RAM_START     (88)    // [RW] LCD RAM起始地址
#define MB_HOLD_REG_LCD_RAM_END       (1023)  // [RW] LCD RAM结束地址

void modbus_reg_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _DEMO_H_ */
