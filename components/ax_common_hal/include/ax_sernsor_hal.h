#ifndef __SENSOR_HAL_H__
#define __SENSOR_HAL_H__

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ax_global_type.h"
#include "common_isp.h"
#include "common_sys.h"
#include "common_cam.h"
#include "common_isp.h"
#include "common_nt.h"
#include "common_vin.h"
#include "my_hal.h"

typedef enum {
    SAMPLE_VIN_NONE                      = -1,
    SAMPLE_VIN_SINGLE_DUMMY              = 0,
    SAMPLE_VIN_SINGLE_OS04A10            = 1,
    SAMPLE_VIN_DOUBLE_OS04A10            = 2,
    SAMPLE_VIN_SINGLE_SC450AI            = 3,
    SAMPLE_VIN_DOUBLE_SC450AI            = 4,
    SAMPLE_VIN_DOUBLE_OS04A10_AND_BT656  = 5,
    SAMPLE_VIN_SINGLE_S5KJN1SQ03         = 6,
    SAMPLE_VIN_SINGLE_OS04A10_DCG_HDR    = 7,
    SAMPLE_VIN_SINGLE_OS04A10_DCG_VS_HDR = 8,
    SYS_CASE_SINGLE_DVP                  = 20,
    SYS_CASE_SINGLE_BT601                = 21,
    SYS_CASE_SINGLE_BT656                = 22,
    SYS_CASE_SINGLE_BT1120               = 23,
    SYS_CASE_SINGLE_LVDS                 = 24,
    SYS_CASE_SINGLE_OS04A10_ONLINE       = 25,
    SAMPLE_VIN_SINGLE_SC850SL            = 26,
    SAMPLE_VIN_BUTT
} SAMPLE_VIN_CASE_E;

typedef struct {
    SAMPLE_VIN_CASE_E eSysCase;
    COMMON_VIN_MODE_E eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode;
    AX_BOOL bAiispEnable;
    AX_S32 nDumpFrameNum;
} SAMPLE_VIN_PARAM_T;

typedef enum {
    HAL_AX_SENSOR_NONT             = 0b00000,
    HAL_AX_SENSOR_NPU_ENABLE       = 0b00001,
    HAL_AX_SENSOR_CAM_ENABLE       = 0b00010,
    HAL_AX_SENSOR_CAM_POOL_ENABLE  = 0b00100,
    HAL_AX_SENSOR_CAM_OPEN         = 0b01000,
    HAL_AX_SENSOR_GET_FRAME_THREAD = 0b10000,
} AX_SENSOR_STATUS;

enum {
    HAL_AX_SENSOR_PAR_0,
    HAL_AX_SENSOR_PAR_1,
    HAL_AX_SENSOR_PAR_2,
    HAL_AX_SENSOR_PAR_3,
    HAL_AX_SENSOR_PAR_4,
    HAL_AX_SENSOR_PAR_CUSTOM,
    HAL_AX_SENSOR_PAR_MAX
};

enum {
    HAL_AX_SENSOR_DEV_0,
    HAL_AX_SENSOR_DEV_1,
    HAL_AX_SENSOR_DEV_2,
    HAL_AX_SENSOR_DEV_MAX
};

enum {
    HAL_AX_SENSOR_CHN_0,
    HAL_AX_SENSOR_CHN_MAX
};

typedef hal_buffer_t Sensor_Frame;

typedef struct {
    int status;
    int nPipeId;
    int Chn;
    pthread_t farm_pthread_p;
    void (*out_farm)(void *);
    AX_IMG_INFO_T ax_img;
    int sernsor_frame_rate;
    int fcnt;
    int fps;
    struct timespec ts1, ts2;
    AX_MOD_INFO_T selfMod;
} ax_sensor_dev_info;

typedef struct ax_sensor_hal_t {
    int ChnSize;
    SAMPLE_VIN_PARAM_T VinParam;
    AX_CAMERA_T gCams[MAX_CAMERAS];
    COMMON_SYS_ARGS_T tCommonArgs;
    COMMON_SYS_ARGS_T tPrivArgs;

    ax_sensor_dev_info dev[HAL_AX_SENSOR_DEV_MAX];

    int status;

    int (*InitSensor)(struct ax_sensor_hal_t *, int, int);
    int (*OpenSensor)(struct ax_sensor_hal_t *);
    int (*CloseSensor)(struct ax_sensor_hal_t *);
    int (*SetSensorOut)(struct ax_sensor_hal_t *, int, void (*out_farm)(void *));
    void (*start)(struct ax_sensor_hal_t *, int);
    void (*stop)(struct ax_sensor_hal_t *, int);
    int (*GetFrameRate)(struct ax_sensor_hal_t *, int);
    int (*getStatus)(struct ax_sensor_hal_t *, int);

    void (*set_Sensor_mode_par[HAL_AX_SENSOR_PAR_MAX])(struct ax_sensor_hal_t *);
    void (*set_Sensor_mode_par_after[HAL_AX_SENSOR_PAR_MAX])(struct ax_sensor_hal_t *);

    void *(*get_link_mod)(struct ax_sensor_hal_t *, int);
    AX_MOD_INFO_T (*get_chn_pipe_id)(struct ax_sensor_hal_t *self, int dev, int chn);
} ax_sensor_hal;

int ax_create_sensor(ax_sensor_hal *sensor_dev);
void ax_destroy_sensor(ax_sensor_hal *sensor_dev);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class ax_sensor_hal_cpp {
private:
    bool exit_flage;

public:
    ax_sensor_hal _dev;
    ax_sensor_hal_cpp(/* args */)
    {
        ax_create_sensor(&_dev);
        exit_flage = true;
    }
    int OpenSensor()
    {
        return _dev.OpenSensor(&_dev);
    }
    int CloseSensor()
    {
        return _dev.CloseSensor(&_dev);
    }
    int SetSensorOut(int Chn, void (*out_farm)(void *))
    {
        return _dev.SetSensorOut(&_dev, Chn, out_farm);
    }
    void start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }
    void exit()
    {
        if (exit_flage) {
            ax_destroy_sensor(&_dev);
            exit_flage = false;
        }
    }
    ~ax_sensor_hal_cpp()
    {
        if (exit_flage) {
            exit();
        }
    }
};

#endif

#endif