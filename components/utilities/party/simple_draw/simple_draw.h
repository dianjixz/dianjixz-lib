/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image processing library.
 */
#ifndef __IMLIB_H__
#define __IMLIB_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef M_PI
#define M_PI    3.14159265f
#define M_PI_2  1.57079632f
#define M_PI_4  0.78539816f
#endif

#define IM_LOG2_2(x)    (((x) &                0x2ULL) ? ( 2                        ) :             1) // NO ({ ... }) !
#define IM_LOG2_4(x)    (((x) &                0xCULL) ? ( 2 +  IM_LOG2_2((x) >>  2)) :  IM_LOG2_2(x)) // NO ({ ... }) !
#define IM_LOG2_8(x)    (((x) &               0xF0ULL) ? ( 4 +  IM_LOG2_4((x) >>  4)) :  IM_LOG2_4(x)) // NO ({ ... }) !
#define IM_LOG2_16(x)   (((x) &             0xFF00ULL) ? ( 8 +  IM_LOG2_8((x) >>  8)) :  IM_LOG2_8(x)) // NO ({ ... }) !
#define IM_LOG2_32(x)   (((x) &         0xFFFF0000ULL) ? (16 + IM_LOG2_16((x) >> 16)) : IM_LOG2_16(x)) // NO ({ ... }) !
#define IM_LOG2(x)      (((x) & 0xFFFFFFFF00000000ULL) ? (32 + IM_LOG2_32((x) >> 32)) : IM_LOG2_32(x)) // NO ({ ... }) !

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })
#define IM_LIMIT(a, min_b, max_c) ({__typeof__ (a) _a = (a);__typeof__ (min_b) _min_b = (min_b);__typeof__ (max_c) _max_c = (max_c); _a = _a > max_c ? max_c : _a;_a < min_b ? min_b : _a;})

#define INT8_T_BITS     (sizeof(int8_t) * 8)
#define INT8_T_MASK     (INT8_T_BITS - 1)
#define INT8_T_SHIFT    IM_LOG2(INT8_T_MASK)

#define INT16_T_BITS    (sizeof(int16_t) * 8)
#define INT16_T_MASK    (INT16_T_BITS - 1)
#define INT16_T_SHIFT   IM_LOG2(INT16_T_MASK)

#define INT32_T_BITS    (sizeof(int32_t) * 8)
#define INT32_T_MASK    (INT32_T_BITS - 1)
#define INT32_T_SHIFT   IM_LOG2(INT32_T_MASK)

#define INT64_T_BITS    (sizeof(int64_t) * 8)
#define INT64_T_MASK    (INT64_T_BITS - 1)
#define INT64_T_SHIFT   IM_LOG2(INT64_T_MASK)

#define UINT8_T_BITS    (sizeof(uint8_t) * 8)
#define UINT8_T_MASK    (UINT8_T_BITS - 1)
#define UINT8_T_SHIFT   IM_LOG2(UINT8_T_MASK)

#define UINT16_T_BITS   (sizeof(uint16_t) * 8)
#define UINT16_T_MASK   (UINT16_T_BITS - 1)
#define UINT16_T_SHIFT  IM_LOG2(UINT16_T_MASK)

#define UINT32_T_BITS   (sizeof(uint32_t) * 8)
#define UINT32_T_MASK   (UINT32_T_BITS - 1)
#define UINT32_T_SHIFT  IM_LOG2(UINT32_T_MASK)

#define UINT64_T_BITS   (sizeof(uint64_t) * 8)
#define UINT64_T_MASK   (UINT64_T_BITS - 1)
#define UINT64_T_SHIFT  IM_LOG2(UINT64_T_MASK)

#define IM_DEG2RAD(x)   (((x)*M_PI)/180)
#define IM_RAD2DEG(x)   (((x)*180)/M_PI)


/////////////////
// pixel Stuff //
/////////////////



#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
// ARGB

typedef struct pixel_s {
    char blue;
    char green;
    char red;
} pixel24_t;

#define pixel24232(_u24_t) \
({\
    __typeof__ (_u24_t) ___u24_t = _u24_t;\
    ((*((uint32_t*)((void*)&___u24_t))) & 0xffffff00);\
})
//input_ uint32_t，output pixel24_t
#define pixel32224(_u32_t) \
({\
    __typeof__ (_u32_t) __u32_t = _u32_t;\
    __u32_t = __u32_t >> 8;\
    (*((pixel24_t*)((void*)&__u32_t)));\
})
void nihao()
{
    printf("t am __ORDER_BIG_ENDIAN__ \n");
}
#else
//cpu is little
//input pixel24_t，output uint32_t
// BGRA

typedef struct pixel_s {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel24_t;

#define pixel24232(_u24_t) \
({\
    __typeof__ (_u24_t) ___u24_t = _u24_t;\
    ((*((uint32_t*)&___u24_t)) & 0x00ffffff);\
})
//input_ uint32_t，output pixel24_t
#define pixel32224(_u32_t) \
({\
    __typeof__ (_u32_t) __u32_t = _u32_t;\
    (*((pixel24_t*)&__u32_t));\
})
#endif //__BYTE_ORDER__




/////////////////
// Color Stuff //
/////////////////

typedef struct color_thresholds_list_lnk_data
{
    uint8_t LMin, LMax; // or grayscale
    int8_t AMin, AMax;
    int8_t BMin, BMax;
}
color_thresholds_list_lnk_data_t;

#define COLOR_THRESHOLD_BINARY(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_GRAYSCALE(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_RGB565(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    uint8_t _l = COLOR_RGB565_TO_L(_pixel); \
    int8_t _a = COLOR_RGB565_TO_A(_pixel); \
    int8_t _b = COLOR_RGB565_TO_B(_pixel); \
    ((_threshold->LMin <= _l) && (_l <= _threshold->LMax) && \
    (_threshold->AMin <= _a) && (_a <= _threshold->AMax) && \
    (_threshold->BMin <= _b) && (_b <= _threshold->BMax)) ^ _invert; \
})
#define COLOR_THRESHOLD_RGB888(pixel, threshold, invert)             \
({\
    __typeof__(pixel) _pixel = (pixel);                      \
    __typeof__(threshold) _threshold = (threshold);          \
    __typeof__(invert) _invert = (invert);                   \
    uint8_t _l = COLOR_RGB888_TO_L(_pixel);                  \
    int8_t _a = COLOR_RGB888_TO_A(_pixel);                   \
    int8_t _b = COLOR_RGB888_TO_B(_pixel);                   \
    ((_threshold->LMin <= _l) && (_l <= _threshold->LMax) && \
    (_threshold->AMin <= _a) && (_a <= _threshold->AMax) && \
    (_threshold->BMin <= _b) && (_b <= _threshold->BMax)) ^ _invert; \
})
#define COLOR_THRESHOLD_LAB(pixel, threshold, invert)                               \
({\
    __typeof__(pixel) _pixel = (pixel);                                             \
    __typeof__(threshold) _threshold = (threshold);                                 \
    __typeof__(invert) _invert = (invert);                                          \
    ((_threshold->LMin <= _pixel->L) && (_pixel->L <= _threshold->LMax) &&          \
    (_threshold->AMin <= _pixel->A) && (_pixel->A <= _threshold->AMax) &&           \
    (_threshold->BMin <= _pixel->B) && (_pixel->B <= _threshold->BMax)) ^ _invert;  \
})
#define COLOR_BOUND_BINARY(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_GRAYSCALE(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_RGB565(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(COLOR_RGB565_TO_R5(_pixel0) - COLOR_RGB565_TO_R5(_pixel1)) <= COLOR_RGB565_TO_R5(_threshold)) && \
    (abs(COLOR_RGB565_TO_G6(_pixel0) - COLOR_RGB565_TO_G6(_pixel1)) <= COLOR_RGB565_TO_G6(_threshold)) && \
    (abs(COLOR_RGB565_TO_B5(_pixel0) - COLOR_RGB565_TO_B5(_pixel1)) <= COLOR_RGB565_TO_B5(_threshold)); \
})
#define COLOR_BOUND_RGB888(pixel0, pixel1, threshold)                                                         \
({ \
    __typeof__(pixel0) _pixel0 = (pixel0);                                                                    \
    __typeof__(pixel1) _pixel1 = (pixel1);                                                                    \
    __typeof__(threshold) _threshold = (threshold);                                                           \
    (abs(COLOR_RGB888_TO_R8(_pixel0) - COLOR_RGB888_TO_R8(_pixel1)) <= COLOR_RGB888_TO_R8(_threshold)) &&     \
    (abs(COLOR_RGB888_TO_G8(_pixel0) - COLOR_RGB888_TO_G8(_pixel1)) <= COLOR_RGB888_TO_G8(_threshold)) &&     \
    (abs(COLOR_RGB888_TO_B8(_pixel0) - COLOR_RGB888_TO_B8(_pixel1)) <= COLOR_RGB888_TO_B8(_threshold));       \
})
#define COLOR_BINARY_MIN 0
#define COLOR_BINARY_MAX 1
#define COLOR_GRAYSCALE_BINARY_MIN 0x00
#define COLOR_GRAYSCALE_BINARY_MAX 0xFF
#define COLOR_RGB565_BINARY_MIN 0x0000
#define COLOR_RGB565_BINARY_MAX 0xFFFF
#define COLOR_RGB888_BINARY_MIN 0x000000
#define COLOR_RGB888_BINARY_MAX 0xFFFFFF

#define COLOR_GRAYSCALE_MIN 0
#define COLOR_GRAYSCALE_MAX 255

#define COLOR_R5_MIN 0
#define COLOR_R5_MAX 31
#define COLOR_G6_MIN 0
#define COLOR_G6_MAX 63
#define COLOR_B5_MIN 0
#define COLOR_B5_MAX 31

#define COLOR_R8_MIN 0
#define COLOR_R8_MAX 255
#define COLOR_G8_MIN 0
#define COLOR_G8_MAX 255
#define COLOR_B8_MIN 0
#define COLOR_B8_MAX 255

#define COLOR_L_MIN 0
#define COLOR_L_MAX 100
#define COLOR_A_MIN -128
#define COLOR_A_MAX 127
#define COLOR_B_MIN -128
#define COLOR_B_MAX 127

#define COLOR_Y_MIN 0
#define COLOR_Y_MAX 255
#define COLOR_U_MIN -128
#define COLOR_U_MAX 127
#define COLOR_V_MIN -128
#define COLOR_V_MAX 127

// RGB565 Stuff //

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 11) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel >> 8) & 0xF8; \
    __pixel | (__pixel >> 5); \
})

#define COLOR_RGB565_TO_G6(pixel) (((pixel) >> 5) & 0x3F)
#define COLOR_RGB565_TO_G8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel >> 3) & 0xFC; \
    __pixel | (__pixel >> 6); \
})

#define COLOR_RGB565_TO_B5(pixel) ((pixel) & 0x1F)
#define COLOR_RGB565_TO_B8(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    __pixel = (__pixel << 3) & 0xF8; \
    __pixel | (__pixel >> 5); \
})

#define COLOR_RGB888_TO_R8(pixel) pixel32224(pixel).red
#define COLOR_RGB888_TO_G8(pixel) pixel32224(pixel).green
#define COLOR_RGB888_TO_B8(pixel) pixel32224(pixel).blue

#define COLOR_R5_G6_B5_TO_RGB565(r5, g6, b5) (((r5) << 11) | ((g6) << 5) | (b5))
#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8) ((((r8) & 0xF8) << 8) | (((g8) & 0xFC) << 3) | ((b8) >> 3))

#define COLOR_R8_G8_B8_TO_RGB888(r8, g8, b8) pixel24232(((pixel24_t){.red = r8,.green = g8, .blue = b8}))

#define COLOR_RGB888_TO_Y_(r8, g8, b8) ((((r8) * 38) + ((g8) * 75) + ((b8) * 15)) >> 7) // 0.299R + 0.587G + 0.114B

#define COLOR_RGB888_TO_Y(rgb888) COLOR_RGB888_TO_Y_(COLOR_RGB888_TO_R8(rgb888), COLOR_RGB888_TO_G8(rgb888), COLOR_RGB888_TO_B8(rgb888)) // 0.299R + 0.587G + 0.114B

#define COLOR_RGB565_TO_Y(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_Y_(r, g, b); \
})

#define COLOR_Y_TO_RGB888(pixel) ((pixel) * 0x010101)
#define COLOR_Y_TO_RGB565(pixel) \
({ \
    __typeof__ (pixel) __pixel = (pixel); \
    int __rb_pixel = (__pixel >> 3) & 0x1F; \
    (__rb_pixel * 0x0801) + ((__pixel << 3) & 0x7E0); \
})
#define COLOR_RGB888_TO_U_(r8, g8, b8) ((((r8) * -21) - ((g8) * 43) + ((b8) * 64)) >> 7) // -0.168736R - 0.331264G + 0.5B
#define COLOR_RGB888_TO_U(rgb888) COLOR_RGB888_TO_U_(COLOR_RGB888_TO_R8(rgb888), COLOR_RGB888_TO_G8(rgb888), COLOR_RGB888_TO_B8(rgb888)) // -0.168736R - 0.331264G + 0.5B
#define COLOR_RGB565_TO_U(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_U_(r, g, b); \
})
#define COLOR_RGB888_TO_V_(r8, g8, b8) ((((r8) * 64) - ((g8) * 54) - ((b8) * 10)) >> 7) // 0.5R - 0.418688G - 0.081312B
#define COLOR_RGB888_TO_V(rgb888) COLOR_RGB888_TO_V_(COLOR_RGB888_TO_R8(rgb888), COLOR_RGB888_TO_G8(rgb888), COLOR_RGB888_TO_B8(rgb888)) // 0.5R - 0.418688G - 0.081312B
#define COLOR_RGB565_TO_V(rgb565) \
({ \
    __typeof__ (rgb565) __rgb565 = (rgb565); \
    int r = COLOR_RGB565_TO_R8(__rgb565); \
    int g = COLOR_RGB565_TO_G8(__rgb565); \
    int b = COLOR_RGB565_TO_B8(__rgb565); \
    COLOR_RGB888_TO_V_(r, g, b); \
})



#ifdef IMLIB_ENABLE_LAB_LUT
extern const int8_t lab_table[196608/2];
#define COLOR_RGB565_TO_L(pixel) lab_table[((pixel>>1) * 3) + 0]
#define COLOR_RGB565_TO_A(pixel) lab_table[((pixel>>1) * 3) + 1]
#define COLOR_RGB565_TO_B(pixel) lab_table[((pixel>>1) * 3) + 2]
#else
#define COLOR_RGB565_TO_L(pixel) imlib_rgb565_to_l(pixel)
#define COLOR_RGB565_TO_A(pixel) imlib_rgb565_to_a(pixel)
#define COLOR_RGB565_TO_B(pixel) imlib_rgb565_to_b(pixel)
#define COLOR_RGB888_TO_L(pixel) imlib_rgb888_to_l(pixel)
#define COLOR_RGB888_TO_A(pixel) imlib_rgb888_to_a(pixel)
#define COLOR_RGB888_TO_B(pixel) imlib_rgb888_to_b(pixel)
#endif

#define COLOR_LAB_TO_RGB565(l, a, b) imlib_lab_to_rgb(l, a, b)
#define COLOR_YUV_TO_RGB565(y, u, v) imlib_yuv_to_rgb((y) + 128, u, v)

#define COLOR_LAB_TO_RGB888(l, a, b) imlib_lab_to_rgb888(l, a, b)
#define COLOR_YUV_TO_RGB888(y, u, v) imlib_yuv_to_rgb888((y) + 128, u, v)

#define COLOR_BINARY_TO_GRAYSCALE(pixel) ((pixel) * COLOR_GRAYSCALE_MAX)
#define COLOR_BINARY_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) ? 127 : -128), 0, 0)
#define COLOR_BINARY_TO_RGB888(pixel) COLOR_YUV_TO_RGB888(((pixel) ? 127 : -128), 0, 0)
#define COLOR_RGB565_TO_BINARY(pixel) (COLOR_RGB565_TO_Y(pixel) > (((COLOR_Y_MAX - COLOR_Y_MIN) / 2) + COLOR_Y_MIN))
#define COLOR_RGB565_TO_GRAYSCALE(pixel) COLOR_RGB565_TO_Y(pixel)
#define COLOR_RGB565_TO_RGB888(pixel) COLOR_R8_G8_B8_TO_RGB888(COLOR_RGB565_TO_R8(pixel), COLOR_RGB565_TO_G8(pixel), COLOR_RGB565_TO_B8(pixel))
#define COLOR_RGB888_TO_GRAYSCALE(pixel) COLOR_RGB888_TO_Y(pixel)
#define COLOR_RGB888_TO_BINARY(pixel) (COLOR_RGB888_TO_Y(pixel) > (((COLOR_Y_MAX - COLOR_Y_MIN) / 2) + COLOR_Y_MIN))
#define COLOR_RGB888_TO_RGB565(pixel) COLOR_R8_G8_B8_TO_RGB565(COLOR_RGB888_TO_R8(pixel), COLOR_RGB888_TO_G8(pixel), COLOR_RGB888_TO_B8(pixel))

#define COLOR_GRAYSCALE_TO_BINARY(pixel) ((pixel) > (((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / 2) + COLOR_GRAYSCALE_MIN))
#define COLOR_GRAYSCALE_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) - 128), 0, 0)
#define COLOR_GRAYSCALE_TO_RGB888(pixel) COLOR_YUV_TO_RGB888(((pixel) - 128), 0, 0)

typedef enum {
    COLOR_PALETTE_RAINBOW,
    COLOR_PALETTE_IRONBOW
} color_palette_t;

// Color palette LUTs
extern const uint16_t rainbow_table[256];
extern const uint16_t ironbow_table[256];

/////////////////
// Image Stuff //
/////////////////

// Pixel format IDs.
typedef enum {
    PIXFORMAT_ID_BINARY     = 1,
    PIXFORMAT_ID_GRAY       = 2,
    PIXFORMAT_ID_RGB565     = 3,
    PIXFORMAT_ID_BAYER      = 4,
    PIXFORMAT_ID_YUV422     = 5,
    PIXFORMAT_ID_JPEG       = 6,
    PIXFORMAT_ID_RGB888     = 7,
    PIXFORMAT_ID_PNG        = 9,
    PIXFORMAT_ID_ARGB8      = 8,
    
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
} pixformat_id_t;

// Pixel sub-format IDs.
typedef enum {
    SUBFORMAT_ID_GRAY8      = 0,
    SUBFORMAT_ID_GRAY16     = 1,
    SUBFORMAT_ID_BGGR       = 0, // !!! Note: Make sure bayer sub-formats don't  !!!
    SUBFORMAT_ID_GBRG       = 1, // !!! overflow the sensor.hw_flags.bayer field !!!
    SUBFORMAT_ID_GRBG       = 2,
    SUBFORMAT_ID_RGGB       = 3,
    SUBFORMAT_ID_YUV422     = 0,
    SUBFORMAT_ID_YVU422     = 1,
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
} subformat_id_t;

// Pixel format Byte Per Pixel.
typedef enum {
    PIXFORMAT_BPP_BINARY    = 0,
    PIXFORMAT_BPP_GRAY8     = 1,
    PIXFORMAT_BPP_GRAY16    = 2,
    PIXFORMAT_BPP_RGB565    = 2,
    PIXFORMAT_BPP_BAYER     = 1,
    PIXFORMAT_BPP_YUV422    = 2,
    PIXFORMAT_BPP_RGB888    = 3,
    PIXFORMAT_BPP_ARGB8     = 4,
    /* Note: Update PIXFORMAT_IS_VALID when adding new formats */
} pixformat_bpp_t;

// Pixel format flags.
#define PIXFORMAT_FLAGS_Y       (1 << 28) // YUV format.
#define PIXFORMAT_FLAGS_M       (1 << 27) // Mutable format.
#define PIXFORMAT_FLAGS_C       (1 << 26) // Colored format.
#define PIXFORMAT_FLAGS_J       (1 << 25) // Compressed format (JPEG/PNG).
#define PIXFORMAT_FLAGS_R       (1 << 24) // RAW/Bayer format.
#define PIXFORMAT_FLAGS_CY      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_Y)
#define PIXFORMAT_FLAGS_CM      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_M)
#define PIXFORMAT_FLAGS_CR      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_R)
#define PIXFORMAT_FLAGS_CJ      (PIXFORMAT_FLAGS_C | PIXFORMAT_FLAGS_J)
#define IMLIB_IMAGE_MAX_SIZE(x) ((x) & 0xFFFFFFFF)

// Each pixel format encodes flags, pixel format id and bpp as follows:
// 31......29  28  27  26  25  24  23..........16  15...........8  7.............0
// <RESERVED>  YF  MF  CF  JF  RF  <PIXFORMAT_ID>  <SUBFORMAT_ID>  <BYTES_PER_PIX>
// NOTE: Bit 31-30 must Not be used for pixformat_t to be used as mp_int_t.
typedef enum {
    PIXFORMAT_INVALID    = (0x00000000U),
    PIXFORMAT_BINARY     = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_BINARY << 16) | (0                   << 8) | PIXFORMAT_BPP_BINARY ),
    PIXFORMAT_GRAYSCALE  = (PIXFORMAT_FLAGS_M  | (PIXFORMAT_ID_GRAY   << 16) | (SUBFORMAT_ID_GRAY8  << 8) | PIXFORMAT_BPP_GRAY8  ),
    PIXFORMAT_RGB565     = (PIXFORMAT_FLAGS_CM | (PIXFORMAT_ID_RGB565 << 16) | (0                   << 8) | PIXFORMAT_BPP_RGB565 ),
    PIXFORMAT_ARGB8      = (PIXFORMAT_FLAGS_CM | (PIXFORMAT_ID_ARGB8  << 16) | (0                   << 8) | PIXFORMAT_BPP_ARGB8  ),
    PIXFORMAT_BAYER      = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_BGGR   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_BGGR = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_BGGR   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_GBRG = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_GBRG   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_GRBG = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_GRBG   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_BAYER_RGGB = (PIXFORMAT_FLAGS_CR | (PIXFORMAT_ID_BAYER  << 16) | (SUBFORMAT_ID_RGGB   << 8) | PIXFORMAT_BPP_BAYER  ),
    PIXFORMAT_YUV        = (PIXFORMAT_FLAGS_CY | (PIXFORMAT_ID_YUV422 << 16) | (SUBFORMAT_ID_YUV422 << 8) | PIXFORMAT_BPP_YUV422 ),
    PIXFORMAT_YUV422     = (PIXFORMAT_FLAGS_CY | (PIXFORMAT_ID_YUV422 << 16) | (SUBFORMAT_ID_YUV422 << 8) | PIXFORMAT_BPP_YUV422 ),
    PIXFORMAT_YVU422     = (PIXFORMAT_FLAGS_CY | (PIXFORMAT_ID_YUV422 << 16) | (SUBFORMAT_ID_YVU422 << 8) | PIXFORMAT_BPP_YUV422 ),
    PIXFORMAT_JPEG       = (PIXFORMAT_FLAGS_CJ | (PIXFORMAT_ID_JPEG   << 16) | (0                   << 8) | 0                    ),
    PIXFORMAT_PNG        = (PIXFORMAT_FLAGS_CJ | (PIXFORMAT_ID_PNG    << 16) | (0                   << 8) | 0                    ),
    PIXFORMAT_RGB888     = (PIXFORMAT_FLAGS_CM | (PIXFORMAT_ID_RGB888 << 16) | (0                   << 8) | PIXFORMAT_BPP_RGB888 ),
    PIXFORMAT_LAST       = (0xFFFFFFFFU),
} pixformat_t;

#define PIXFORMAT_MUTABLE_ANY           \
        PIXFORMAT_BINARY:               \
        case PIXFORMAT_GRAYSCALE:       \
        case PIXFORMAT_RGB565:          \
        case PIXFORMAT_ARGB8:           \
        case PIXFORMAT_RGB888           \

#define PIXFORMAT_BAYER_ANY             \
        PIXFORMAT_BAYER_BGGR:           \
        case PIXFORMAT_BAYER_GBRG:      \
        case PIXFORMAT_BAYER_GRBG:      \
        case PIXFORMAT_BAYER_RGGB       \

#define PIXFORMAT_YUV_ANY               \
        PIXFORMAT_YUV422:               \
        case PIXFORMAT_YVU422           \

#define PIXFORMAT_COMPRESSED_ANY        \
        PIXFORMAT_JPEG:                 \
        case PIXFORMAT_PNG              \

#define IMLIB_PIXFORMAT_IS_VALID(x)     \
    ((x == PIXFORMAT_BINARY)            \
     || (x == PIXFORMAT_GRAYSCALE)      \
     || (x == PIXFORMAT_RGB565)         \
     || (x == PIXFORMAT_ARGB8)          \
     || (x == PIXFORMAT_BAYER_BGGR)     \
     || (x == PIXFORMAT_BAYER_GBRG)     \
     || (x == PIXFORMAT_BAYER_GRBG)     \
     || (x == PIXFORMAT_BAYER_RGGB)     \
     || (x == PIXFORMAT_YUV422)         \
     || (x == PIXFORMAT_YVU422)         \
     || (x == PIXFORMAT_JPEG)           \
     || (x == PIXFORMAT_PNG)            \
     || (x == PIXFORMAT_RGB888))        \

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define PIXFORMAT_STRUCT            \
struct {                            \
  union {                           \
    struct {                        \
        uint32_t bpp            :8; \
        uint32_t subfmt_id      :8; \
        uint32_t pixfmt_id      :8; \
        uint32_t is_bayer       :1; \
        uint32_t is_compressed  :1; \
        uint32_t is_color       :1; \
        uint32_t is_mutable     :1; \
        uint32_t is_yuv         :1; \
        uint32_t /*reserved*/   :3; \
    };                              \
    uint32_t pixfmt;                \
  };                                \
  uint32_t size; /* for compressed images */ \
  bool is_data_alloc;  /*data need to free when call destory*/\
}
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define PIXFORMAT_STRUCT            \
struct {                            \
  union {                           \
    struct {                        \
        uint32_t /*reserved*/   :3; \
        uint32_t is_yuv         :1; \
        uint32_t is_mutable     :1; \
        uint32_t is_color       :1; \
        uint32_t is_compressed  :1; \
        uint32_t is_bayer       :1; \
        uint32_t pixfmt_id      :8; \
        uint32_t subfmt_id      :8; \
        uint32_t bpp            :8; \
    };                              \
    uint32_t pixfmt;                \
  };                                \
  uint32_t size; /* for compressed images */ \
  bool is_data_alloc;  /*data need to free when call destory*/\
}
#else
#error "Byte order is not defined."
#endif

typedef struct image {
    uint32_t w;
    uint32_t h;
    PIXFORMAT_STRUCT;
    union {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;

image_t* imlib_image_create(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels, bool is_data_alloc);
void imlib_image_destroy(image_t **obj);
void imlib_image_init(image_t *ptr, int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels);
void image_copy(image_t *dst, image_t *src);
size_t image_size(image_t *ptr);
bool image_get_mask_pixel(image_t *ptr, int x, int y);

#define IMAGE_BINARY_LINE_LEN(image) (((image)->w + UINT32_T_MASK) >> UINT32_T_SHIFT)
#define IMAGE_BINARY_LINE_LEN_BYTES(image) (IMAGE_BINARY_LINE_LEN(image) * sizeof(uint32_t))

#define IMAGE_GRAYSCALE_LINE_LEN(image) ((image)->w)
#define IMAGE_GRAYSCALE_LINE_LEN_BYTES(image) (IMAGE_GRAYSCALE_LINE_LEN(image) * sizeof(uint8_t))

#define IMAGE_RGB565_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB565_LINE_LEN_BYTES(image) (IMAGE_RGB565_LINE_LEN(image) * sizeof(uint16_t))

#define IMAGE_RGB888_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB888_LINE_LEN_BYTES(image) (IMAGE_RGB888_LINE_LEN(image) * sizeof(pixel24_t))        

#define IMAGE_GET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    size_t _i = (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT); \
    size_t _j = _x & UINT32_T_MASK; \
    ((uint32_t *) _image->data)[_i] = (((uint32_t *) _image->data)[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_RGB565_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB888_PIXEL_(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((pixel24_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_GET_RGB888_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    pixel24232(((pixel24_t *) _image->data)[(_image->w * _y) + _x]); \
})

#define IMAGE_PUT_RGB888_PIXEL_(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((pixel24_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_PUT_RGB888_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((pixel24_t *) _image->data)[(_image->w * _y) + _x] = pixel32224(_v); \
})

#define IMAGE_GET_YUV_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_YUV_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_BAYER_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_BAYER_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

// Fast Stuff //

#define IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y); \
})

#define IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    (_row_ptr[_x >> UINT32_T_SHIFT] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    size_t _i = _x >> UINT32_T_SHIFT; \
    size_t _j = _x & UINT32_T_MASK; \
    _row_ptr[_i] = (_row_ptr[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((pixel24_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_GET_RGB888_PIXEL_FAST_(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_GET_RGB888_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    pixel24232(_row_ptr[_x]); \
})

#define IMAGE_PUT_RGB888_PIXEL_FAST_(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_PUT_RGB888_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = pixel32224(_v); \
})

#define IMAGE_COMPUTE_BAYER_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_COMPUTE_YUV_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})

// Old Image Macros - Will be refactor and removed. But, only after making sure through testing new macros work.



// Grayscale maxes
#define IM_MAX_GS (255)

#define IM_IS_BINARY(img)   ((img)->pixfmt == PIXFORMAT_BINARY)
#define IM_IS_GS(img)       ((img)->pixfmt == PIXFORMAT_GRAYSCALE)
#define IM_IS_RGB565(img)   ((img)->pixfmt == PIXFORMAT_RGB565)
#define IM_IS_BAYER(img)    ((img)->is_bayer)
#define IM_IS_JPEG(img)     ((img)->pixfmt == PIXFORMAT_JPEG)
#define IM_IS_RGB888(img)   ((img)->pixfmt == PIXFORMAT_RGB888)

#define IM_X_INSIDE(img, x) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       (0<=_x)&&(_x<_img->w); })

#define IM_Y_INSIDE(img, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (y) _y = (y); \
       (0<=_y)&&(_y<_img->h); })

#define IM_GET_GS_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RGB565_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RGB888_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       pixel24232(((pixel24_t*)_img->pixels)[(_y*_img->w)+_x]); })
#define IM_GET_RGB888_PIXEL_(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((pixel24_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_SET_GS_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_SET_RGB565_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_SET_RGB888_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((pixel24_t*)_img->pixels)[(_y*_img->w)+_x]=pixel32224(_p); })

#define IM_EQUAL(img0, img1) \
    ({ __typeof__ (img0) _img0 = (img0); \
       __typeof__ (img1) _img1 = (img1); \
       (_img0->w==_img1->w)&&(_img0->h==_img1->h)&&(_img0->pixfmt=_img1->pixfmt); })

#define IM_TO_GS_PIXEL(img, x, y)    \
    (img->bpp == 1 ? img->pixels[((y)*img->w)+(x)] : COLOR_RGB565_TO_Y(((uint16_t*)img->pixels)[((y)*img->w)+(x)]) )







// Library Hardware Init
// void imlib_init_all();
// void imlib_deinit_all();

//imlib base operation
// void imlib_pixfmt_to(image_t *dst, image_t *src, rectangle_t *roi_i);
// void imlib_image_resize(image_t *dst, image_t *src, int hist);

/* Color space functions */
int8_t imlib_rgb565_to_l(uint16_t pixel);
int8_t imlib_rgb565_to_a(uint16_t pixel);
int8_t imlib_rgb565_to_b(uint16_t pixel);
int8_t imlib_rgb888_to_l(uint32_t pixel);
int8_t imlib_rgb888_to_a(uint32_t pixel);
int8_t imlib_rgb888_to_b(uint32_t pixel);
uint16_t imlib_lab_to_rgb(uint8_t l, int8_t a, int8_t b);
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v);
uint32_t imlib_lab_to_rgb888(uint8_t l, int8_t a, int8_t b);
uint32_t imlib_yuv_to_rgb888(uint8_t y, int8_t u, int8_t v);




// Helper Functions
void imlib_zero(image_t *img, image_t *mask, bool invert);
// void imlib_draw_row_setup(imlib_draw_row_data_t *data);
// void imlib_draw_row_teardown(imlib_draw_row_data_t *data);


// void *imlib_draw_row_get_row_buffer(imlib_draw_row_data_t *data);
// void imlib_draw_row_put_row_buffer(imlib_draw_row_data_t *data, void *row_buffer);
// void imlib_draw_row(int x_start, int x_end, int y_row, imlib_draw_row_data_t *data);
// bool imlib_draw_image_rectangle(image_t *dst_img, image_t *src_img, int dst_x_start, int dst_y_start, float x_scale, float y_scale, rectangle_t *roi,
//                                 int alpha, const uint8_t *alpha_palette, image_hint_t hint,
//                                 int *x0, int *x1, int *y0, int *y1);
// void imlib_flood_fill_int(image_t *out, image_t *img, int x, int y,
//                           int seed_threshold, int floating_threshold,
//                           flood_fill_call_back_t cb, void *data);
// Drawing Functions
int imlib_get_pixel(image_t *img, int x, int y);
int imlib_get_pixel_fast(image_t *img, const void *row_ptr, int x);
void imlib_set_pixel(image_t *img, int x, int y, int p);
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness);
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill);
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill);
void imlib_draw_ellipse(image_t *img, int cx, int cy, int rx, int ry, int rotation, int c, int thickness, bool fill);
// void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
//                        int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_hflip);
// void imlib_draw_image_fast(image_t *img, image_t *other, int x_off, int y_off, float x_scale, float y_scale, float alpha, image_t *mask);
// void imlib_draw_image(image_t *dst_img, image_t *src_img, int dst_x_start, int dst_y_start, float x_scale, float y_scale, rectangle_t *roi,
//                       int rgb_channel, int alpha, const uint16_t *color_palette, const uint8_t *alpha_palette, image_hint_t hint,
//                       imlib_draw_row_callback_t callback, void *dst_row_override);
void imlib_flood_fill(image_t *img, int x, int y,
                      float seed_threshold, float floating_threshold,
                      int c, bool invert, bool clear_background, image_t *mask);
void imlib_draw_cross(image_t *img, int x, int y, int c, int size, int thickness);






#ifdef __cplusplus
}
#endif
#endif //__IMLIB_H__
