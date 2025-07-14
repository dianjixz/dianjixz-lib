/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _IP_H_
#define _IP_H_

#ifdef __cplusplus

extern "C" {
#endif

#include "config.h"

#define IP_ADDR_DEFAULT (0)


void set_ip_high(uint16_t high);

void set_ip_low(uint16_t low);

uint32_t get_ip(void);

#ifdef __cplusplus
}
#endif

#endif /* _IP_H_ */






