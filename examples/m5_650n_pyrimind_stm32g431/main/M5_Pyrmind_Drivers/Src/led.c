#include "led.h"

static bool led_arm_flag = false;

extern __IO uint16_t g_mb_holding_regs[MB_HOLDING_REG_COUNT + 1];

bool led_mode_check(uint16_t mode){
    return mode <= LED_MODE_IP;
}

void set_led_arm_flag(bool flag)
{
    led_arm_flag = flag;
}

bool get_led_arm_flag(void)
{
    return led_arm_flag;
}