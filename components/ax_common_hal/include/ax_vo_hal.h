#ifndef __VO_HAL_H__
#define __VO_HAL_H__

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "my_hal.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "ax_global_type.h"
#include "common_sys.h"
#include "common_vo.h"
#include "common_vo_pattern.h"

typedef enum {
    AX_VO_NONT       = 0b00000,
    AX_VO_FMT0       = 0b00001,
    AX_VO_CHN_POOL   = 0b00010,
    AX_VO_FMT1       = 0b00100,
    AX_VO_LAYER_POOL = 0b01000,
    AX_VO_OPEN       = 0b10000,
} AX_VO_STATUS;

typedef struct {
    int status;
    int Chn;
    AX_VIDEO_FRAME_T stFrame[2];
    AX_VO_QUERY_STATUS_T stStatus;
    AX_MOD_INFO_T selfMod;
} ax_vo_dev_info;

#define AX_MAX_VO_CHN_        3
#define AX_MAX_VO_CHN_CONFIG_ 10
typedef struct ax_vo_hal_t {
    SAMPLE_VO_CONFIG_S stVoConfig;
    ax_vo_dev_info dev[AX_MAX_VO_CHN_];
    int status;
    int (*InitVo)(struct ax_vo_hal_t *, int, int);
    int (*OpenVo)(struct ax_vo_hal_t *, int);
    int (*CloseVo)(struct ax_vo_hal_t *, int);

    void (*start)(struct ax_vo_hal_t *, int);
    void (*stop)(struct ax_vo_hal_t *, int);
    void *(*get_frame_mem)(struct ax_vo_hal_t *, AX_U32, AX_U32);
    AX_VIDEO_FRAME_T *(*get_frame)(struct ax_vo_hal_t *, AX_U32, AX_U32);
    void (*put_frame)(struct ax_vo_hal_t *, AX_U32, AX_U32);

    int (*getStatus)(struct ax_vo_hal_t *, int);
    void (*set_Vo_mode_par[AX_MAX_VO_CHN_CONFIG_])(struct ax_vo_hal_t *);
} ax_vo_hal;

int ax_create_vo(ax_vo_hal *vo_dev);
void ax_destroy_vo(ax_vo_hal *vo_dev);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ax_vo_hal_cpp {
private:
    bool exit_flage;

public:
    ax_vo_hal _dev;
    ax_vo_hal_cpp(/* args */)
    {
        ax_create_vo(&_dev);
        exit_flage = true;
    }
    int InitVo(int vo_dev, int mode)
    {
        return _dev.InitVo(&_dev, vo_dev, mode);
    }
    int OpenVo(int vo_dev)
    {
        return _dev.OpenVo(&_dev, vo_dev);
    }
    int CloseVo(int vo_dev)
    {
        return _dev.CloseVo(&_dev, vo_dev);
    }
    void Start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }
    void *GetFrameMem(AX_U32 u32ChnID, AX_U32 u32LayerID)
    {
        return _dev.get_frame_mem(&_dev, u32ChnID, u32LayerID);
    }
    void PutFrameMem(AX_U32 u32ChnID, AX_U32 u32LayerID)
    {
        _dev.put_frame(&_dev, u32ChnID, u32LayerID);
    }
    void Stop(int Chn)
    {
        _dev.stop(&_dev, Chn);
    }
    void exit()
    {
        if (exit_flage) {
            ax_destroy_vo(&_dev);
            exit_flage = false;
        }
    }
    ~ax_vo_hal_cpp()
    {
        if (exit_flage) {
            exit();
        }
    }
};

#endif

#endif