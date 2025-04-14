/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include "lt9611.h"
#include <stdlib.h>


// hdmi_lt8618sx_t lt8618;
int main(int argc, char *argv[])
{
	HDMI_I2C_INIT();
	// LT9611_Reset();
	// LT9611_Init();
	LT9611_pattern();
	HDMI_I2C_DEINIT();
	return 0;
}