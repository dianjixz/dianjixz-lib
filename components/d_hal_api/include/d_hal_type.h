#pragma once

#ifdef CONFIG_D_HAL_NN_AXERA_AX650C

#endif

namespace  d_hal
{
    typedef enum {
        D_HAL_NN_AXERA_AX650C = 0,
        D_HAL_NN_AXERA_AX630C,
        D_HAL_NN_AXERA_AX620Q,
        D_HAL_NN_ONNX,
        D_HAL_NN_NCNN,
    }D_HAL_NN_TYPE_E;
    typedef enum {
        D_HAL_AXERA_AX650C = 0,
    }D_HAL_HARDWARE_TYPE_E;
    typedef enum {
        D_HAL_VIN_V4L2 = 0,
    }D_HAL_VIN_TYPE_E;

    typedef enum {
        D_HAL_VOUT_SDL = 0,
        D_HAL_VOUT_SDL2,
        D_HAL_VOUT_FBDEV,
        D_HAL_VOUT_AXERA_MIPI,
        D_HAL_VOUT_AXERA_HDMI,
    }D_HAL_VOUT_TYPE_E;
    
    struct spinlock_guard {
        std::atomic_flag& flag;
        spinlock_guard(std::atomic_flag& f) : flag(f) {
            while (flag.test_and_set(std::memory_order_acquire));
        }
        ~spinlock_guard() {
            flag.clear(std::memory_order_release);
        }
    };
}