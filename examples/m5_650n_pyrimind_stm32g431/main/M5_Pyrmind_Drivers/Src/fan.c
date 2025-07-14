#include "fan.h"

static uint16_t compare = 0;

static __IO uint32_t crr1 = 0, crr2 = 0, crr3 = 0;
static __IO uint8_t measure_flag = 0;
static __IO uint32_t freq        = 0;
static __IO uint32_t duty        = 0;
static __IO uint32_t fan_freq    = 0;
static __IO uint32_t fan_rpm     = 0;
static __IO uint32_t value       = 0;
static __IO uint8_t measure_cnt  = 1;

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void capture(void)
{
    uint32_t diff1 = 0, diff2 = 0;
    uint32_t max_cnt = __HAL_TIM_GET_AUTORELOAD(&htim2);

    if (crr2 >= crr1)
        diff1 = crr2 - crr1;
    else
        diff1 = (max_cnt + 1) + crr2 - crr1;

    if (crr3 >= crr1)
        diff2 = crr3 - crr1;
    else
        diff2 = (max_cnt + 1) + crr3 - crr1;

    if (diff2 == 0) {
        fan_freq = 0;
        fan_rpm  = 0;
        duty     = 0;
        return;
    }

    freq     = TIMER_CLOCK_HZ / diff2;
    duty     = diff1 * 100 / diff2;
    fan_freq = freq;
    fan_rpm  = freq * (60 / FAN_PULSE_PER_REV);
}

void fan_init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  // 开启定时器
    set_fan_pwm(FAN_PWM_MIN);                  // 默认最下转速
}

void set_fan_pwm(uint16_t pwm)
{
    compare = map(pwm, 0, FAN_PWM_MAX, 0, FAN_ARR_MAX);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, compare);
#ifdef FAN_DEBUG
    printf("set fan pwm: %d, compare: %d\n", pwm, compare);
#endif
}

uint16_t get_fan_rpm(void)
{
    uint32_t start = HAL_GetTick();
    crr1 = crr2 = crr3 = 0;
    measure_flag       = 0;

    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    while (measure_flag == 0) {
        if ((HAL_GetTick() - start) > MEASURE_TIMEOUT_MS) {
            HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
#ifdef FAN_DEBUG
            printf("get fan rpm timeout\n");
#endif
            return 0xFFFF;
        }
    }

    capture();
#ifdef FAN_DEBUG
    printf("get fan rpm: %d\n", fan_rpm);
#endif
    return (uint16_t)fan_rpm;
}

bool fan_pwm_check(uint16_t pwm)
{
    if (pwm < FAN_PWM_MIN || pwm > FAN_PWM_MAX) {
        return false;
    }
    return true;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2) {
        uint32_t value = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
        switch (measure_cnt) {
            case 1:
                crr1 = value;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
                measure_cnt = 2;
                break;

            case 2:
                crr2 = value;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
                measure_cnt = 3;
                break;

            case 3:
                crr3 = value;
                HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
                measure_cnt  = 1;
                measure_flag = 1;
                break;

            default:
                HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
                measure_cnt = 1;
                break;
        }
    }
}
