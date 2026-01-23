#pragma once
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
}