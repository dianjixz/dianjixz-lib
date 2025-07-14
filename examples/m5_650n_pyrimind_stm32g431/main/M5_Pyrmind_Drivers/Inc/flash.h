/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/

#ifndef _FLASH_H_
#define _FLASH_H_

#ifdef __cplusplus

extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>
#include "stm32g4xx_hal_flash_ex.h"
#include "config.h"
#include "demo.h"

// STM32G4 Flash page size definition
#define STM32G4xx_PAGE_SIZE (0x800) // Page size: 2048 bytes (2 KB)

// Flash base address
#define STM32G4xx_FLASH_PAGE0_STARTADDR (0x8000000) // Start address of flash page 0 (0x08000000)

// Start address of flash page 60
#define STM32G4xx_FLASH_PAGE60_STARTADDR (STM32G4xx_FLASH_PAGE0_STARTADDR + 60 * STM32G4xx_PAGE_SIZE) // Start address of flash page 60 (0x08007000)
// Start address of flash page 61
#define STM32G4xx_FLASH_PAGE61_STARTADDR (STM32G4xx_FLASH_PAGE0_STARTADDR + 61 * STM32G4xx_PAGE_SIZE) // Start address of flash page 61 (0x08007800)

void flash_init(void);

bool set_page60_buffer(void);
bool get_page60_buffer(void);

bool set_page61_buffer(void);
bool get_page61_buffer(void);

void set_page60_update_flag(bool flag);
void set_page61_update_flag(bool flag);

void flash_update(void);

#ifdef __cplusplus
}
#endif

#endif /* _FLASH_H_ */
