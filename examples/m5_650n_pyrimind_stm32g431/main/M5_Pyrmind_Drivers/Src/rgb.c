#include <rgb.h>
#include <stdlib.h>
#include "string.h"
#include "demo.h"

#define gpio_low()  GPIOC->BRR = GPIO_PIN_13
#define gpio_high() GPIOC->BSRR = GPIO_PIN_13
#define delay_150ns() \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP")
#define delay_300ns() \
    delay_150ns();    \
    delay_150ns();    \
    delay_150ns();    \
    delay_150ns();    \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP")
#define delay_600ns() \
    delay_300ns();    \
    delay_300ns();    \
    __asm("NOP");     \
    __asm("NOP")
#define delay_900ns() \
    delay_600ns();    \
    delay_300ns()

#define out_bit_low() \
    gpio_high();      \
    delay_300ns();    \
    gpio_low();       \
    delay_900ns()

#define out_bit_high() \
    gpio_high();       \
    delay_600ns();     \
    gpio_low();        \
    delay_600ns()

#define restart()                           \
    do {                                    \
        for (uint8_t i = 0; i < 255; i++) { \
            delay_900ns();                  \
            delay_300ns();                  \
        }                                   \
    } while (0)

static bool rgb_arm_flag                   = false;
static rgb_mode_t rgb_mode                 = RGB_MODE_GRADIENT;
static uint8_t rgb_buffer[RGB_NUM_MAX * 3] = {0};

extern __IO uint16_t g_mb_holding_regs[MB_HOLDING_REG_COUNT + 1];

static const uint8_t gradient_colors[RGB_GRADIENT_COLOR_COUNT][3] = {
    {255, 0, 0},    // 红
    {255, 165, 0},  // 橙
    {255, 255, 0},  // 黄
    {0, 255, 0},    // 绿
    {0, 0, 255},    // 蓝
    {128, 0, 128}   // 紫
};

// HSV to RGB conversion
static void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint8_t region     = h / 60;
    uint16_t remainder = (h - region * 60) * 255 / 60;

    uint8_t p = (v * (255 - s)) / 255;
    uint8_t q = (v * (255 - (s * remainder) / 255)) / 255;
    uint8_t t = (v * (255 - (s * (255 - remainder)) / 255)) / 255;

    switch (region) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

static void rgb_arm_buffer_update(void)
{
    uint16_t ledCount = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
    if (ledCount > RGB_NUM_MAX) ledCount = RGB_NUM_MAX;

    uint16_t baseIndex = MB_HOLD_REG_RGB1_ARM_START;

    for (uint16_t i = 0; i < ledCount; i++) {
        uint16_t regGroupIndex = (i / 2) * 3;
        uint8_t R, G, B;

        if ((i % 2) == 0) {
            R = (g_mb_holding_regs[baseIndex + regGroupIndex] >> 8) & 0xFF;
            G = (g_mb_holding_regs[baseIndex + regGroupIndex]) & 0xFF;
            B = (g_mb_holding_regs[baseIndex + regGroupIndex + 1] >> 8) & 0xFF;
        } else {
            R = (g_mb_holding_regs[baseIndex + regGroupIndex + 1]) & 0xFF;
            G = (g_mb_holding_regs[baseIndex + regGroupIndex + 2] >> 8) & 0xFF;
            B = (g_mb_holding_regs[baseIndex + regGroupIndex + 2]) & 0xFF;
        }

        rgb_buffer[i * 3 + 0] = G;
        rgb_buffer[i * 3 + 1] = R;
        rgb_buffer[i * 3 + 2] = B;
    }
}

static void rgb_send_buffer(void)
{
    uint16_t count = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
    if (count > RGB_NUM_MAX) count = RGB_NUM_MAX;
    __disable_irq();
    for (uint16_t i = 0; i < count * 3; i++) {
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (rgb_buffer[i] & (1 << (7 - bit))) {
                out_bit_high();
            } else {
                out_bit_low();
            }
        }
    }
    __enable_irq();
    restart();
}

static void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};                     // Structure to hold GPIO initialization parameters
    GPIO_InitStruct.Pin              = GPIO_PIN_13;             // Specify the GPIO pin to configure
    GPIO_InitStruct.Mode             = GPIO_MODE_OUTPUT_PP;     // Set pin mode to output push-pull
    GPIO_InitStruct.Pull             = GPIO_NOPULL;             // No internal pull-up or pull-down
    GPIO_InitStruct.Speed            = GPIO_SPEED_FREQ_MEDIUM;  // Set speed to medium frequency
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);                     // Initialize GPIO with the specified settings
}

void rgb_init(void)
{
    GPIO_init();
    restart();
    rgb_send_buffer();
}

void rgb_power_start_indicate(void)
{
    uint8_t buffer[3] = {0x00, 0xFF, 0x00};
    __disable_irq();
    for (uint16_t i = 0; i < 3; i++) {
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (buffer[i] & (1 << (7 - bit))) {
                out_bit_high();
            } else {
                out_bit_low();
            }
        }
    }
    __enable_irq();
    restart();
}

void rgb_power_complete_indicate(void)
{
    rgb_send_buffer();
}

void rgb_show(void)
{
    rgb_arm_buffer_update();
    rgb_send_buffer();
}

bool rgb_num_check(uint16_t num)
{
    return (num >= RGB_NUM_MIN) && (num <= RGB_NUM_MAX);
}

bool rgb_mode_check(uint16_t mode)
{
    return mode <= RGB_MODE_RAINFLOW;
}

void set_rgb_arm_flag(bool flag)
{
    rgb_arm_flag = flag;
}

bool get_rgb_arm_flag(void)
{
    return rgb_arm_flag;
}

void set_rgb_mode(rgb_mode_t mode)
{
    rgb_mode = mode;
}

rgb_mode_t get_rgb_mode(void)
{
    return rgb_mode;
}

void rgb_gradient_step(void)
{
    static uint32_t last_tick = 0;
    static uint16_t step = 0;

    if (HAL_GetTick() - last_tick < RGB_UPDATE_INTERVAL_MS) return;
    last_tick = HAL_GetTick();

    uint16_t totalSteps = RGB_GRADIENT_STEP_MAX;

    uint16_t n = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
    if (n > RGB_NUM_MAX) n = RGB_NUM_MAX;

    for (uint16_t i = 0; i < n; i++) {
        uint16_t hue = (step + i * (totalSteps / n)) % totalSteps;
        uint8_t r, g, b;
        hsv_to_rgb(hue, 255, RGB_GRADIENT_BRIGHTNESS, &r, &g, &b);

        rgb_buffer[i * 3 + 0] = g;
        rgb_buffer[i * 3 + 1] = r;
        rgb_buffer[i * 3 + 2] = b;
    }
    rgb_send_buffer();

    if (++step >= totalSteps) step = 0;
}

void rgb_blink_step(void)
{
    static uint32_t last_tick = 0;
    static uint16_t phase = 0;
    static uint8_t colorIndex = 0;

    if (HAL_GetTick() - last_tick < RGB_UPDATE_INTERVAL_MS) return;
    last_tick = HAL_GetTick();

    // 0~359度，正弦波
    phase = (phase + 2) % 360;
    // sin(phase)范围(-1~1)->(0~1)
    float sine = (sinf(phase * 3.14159f / 180.0f) + 1.0f) * 0.5f;

    // 缓慢切换颜色
    if (phase == 0) {
        colorIndex = (colorIndex + 1) % RGB_GRADIENT_COLOR_COUNT;
    }

    uint8_t brightness = (uint8_t)(sine * RGB_BLINK_BRIGHTNESS_MAX);
    if (brightness < RGB_BLINK_BRIGHTNESS_MIN) brightness = RGB_BLINK_BRIGHTNESS_MIN;

    const uint8_t *baseColor = gradient_colors[colorIndex];
    uint8_t r = (baseColor[0] * brightness) / 255;
    uint8_t g = (baseColor[1] * brightness) / 255;
    uint8_t b = (baseColor[2] * brightness) / 255;

    uint16_t n = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
    if (n > RGB_NUM_MAX) n = RGB_NUM_MAX;

    for (uint16_t i = 0; i < n; i++) {
        rgb_buffer[i * 3 + 0] = g;
        rgb_buffer[i * 3 + 1] = r;
        rgb_buffer[i * 3 + 2] = b;
    }
    rgb_send_buffer();
}

void rgb_rainflow_step(void)
{
    static uint32_t last_tick = 0;
    static uint16_t offset = 0;

    if (HAL_GetTick() - last_tick < RGB_UPDATE_INTERVAL_MS) return;
    last_tick = HAL_GetTick();

    uint16_t n = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
    if (n > RGB_NUM_MAX) n = RGB_NUM_MAX;
    if (++offset >= n) offset = 0;

    for (uint16_t i = 0; i < n; i++) {
        int16_t distance = i - offset;
        if (distance < 0) distance += n;

        uint8_t brightness = (distance < RGB_RAINFLOW_TAIL_LENGTH)
                                 ? (255 - distance * RGB_RAINFLOW_TAIL_STEP)
                                 : 0;

        // 让色相也随位置变化
        uint16_t hue = (360 * i / n + offset * 10) % 360;
        uint8_t r, g, b;
        hsv_to_rgb(hue, 255, brightness, &r, &g, &b);

        rgb_buffer[i * 3 + 0] = g;
        rgb_buffer[i * 3 + 1] = r;
        rgb_buffer[i * 3 + 2] = b;
    }
    rgb_send_buffer();
}

void rgb_update(void)
{
     switch (rgb_mode) {
         case RGB_MODE_ARM_MAP:
             if (rgb_arm_flag) {
                 set_rgb_arm_flag(false);
                 rgb_show();
             }
             break;
         case RGB_MODE_GRADIENT:
            rgb_gradient_step();
             break;

         case RGB_MODE_BLINK:
             rgb_blink_step();
             break;

         case RGB_MODE_RAINFLOW:
             rgb_rainflow_step();
             break;
         default:
             break;
     }
}

void rgb_test(void)
{
    for (uint8_t i = 0; i < 56; i++) {
        g_mb_holding_regs[MB_HOLD_REG_RGB1_ARM_START + i] = 0x2F00;
    }
    set_rgb_arm_flag(true);
}
