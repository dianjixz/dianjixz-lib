//
// Copyright (c) 2023-2025 dianjixz
//

#ifndef __AX_IVPS_HAL_H_
#define __AX_IVPS_HAL_H_

#include "ax_ivps_api.h"
#include "common_venc.h"
#include "my_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef hal_buffer_t Venc_Frame;
typedef hal_buffer_t ivps_buffer_t;

typedef enum {
    HAL_AX_IVPS_GRP_NONT   = 0,
    HAL_AX_IVPS_GRP_CREATE = 0b1,
    HAL_AX_IVPS_GRP_SET    = 0b01,
    HAL_AX_IVPS_GRP_ENABLE = 0b001,
    HAL_AX_IVPS_GRP_START  = 0b0001,
    HAL_AX_IVPS_GRP_LINK   = 0b00001,
    HAL_AX_IVPS_GRP_OSD    = 0b000001
} AX_IVPS_GRP_STATUS;

enum {
    HAL_AX_IVPS_PAR_0,
    HAL_AX_IVPS_PAR_1,
    HAL_AX_IVPS_PAR_2,
    HAL_AX_IVPS_PAR_3,
    HAL_AX_IVPS_PAR_4,
    HAL_AX_IVPS_PAR_CUSTOM,
    HAL_AX_IVPS_PAR_MAX
};

enum {
    HAL_AX_IVPS_DEV_0,
    HAL_AX_IVPS_DEV_1,
    HAL_AX_IVPS_DEV_2,
    HAL_AX_IVPS_DEV_3,
    HAL_AX_IVPS_DEV_4,
    HAL_AX_IVPS_DEV_5,
    HAL_AX_IVPS_DEV_6,
    HAL_AX_IVPS_DEV_7,
    HAL_AX_IVPS_DEV_8,
    HAL_AX_IVPS_DEV_9,
    HAL_AX_IVPS_DEV_10,
    HAL_AX_IVPS_DEV_11,
    HAL_AX_IVPS_DEV_12,
    HAL_AX_IVPS_DEV_13,
    HAL_AX_IVPS_DEV_14,
    HAL_AX_IVPS_DEV_15,
    HAL_AX_IVPS_DEV_16,
    HAL_AX_IVPS_DEV_17,
    HAL_AX_IVPS_DEV_18,
    HAL_AX_IVPS_DEV_19,
    HAL_AX_IVPS_DEV_MAX
};
enum {
    HAL_AX_IVPS_CHN_0,
    HAL_AX_IVPS_CHN_1,
    HAL_AX_IVPS_CHN_2,
    HAL_AX_IVPS_CHN_3,
    HAL_AX_IVPS_CHN_4,
    HAL_AX_IVPS_CHN_5,
    HAL_AX_IVPS_CHN_6,
    HAL_AX_IVPS_CHN_7,
    HAL_AX_IVPS_CHN_8,
    HAL_AX_IVPS_CHN_9,
    HAL_AX_IVPS_CHN_10,
    HAL_AX_IVPS_CHN_11,
    HAL_AX_IVPS_CHN_MAX
};

#define AUTO_GRP_ID -1

struct ax_ivps_pthread_par {
    int GRP;
    int CHN;
    int exit;
    void *ctx;
    void (*out_farm)(AX_VIDEO_FRAME_T *, void *ctx);
    pthread_t farm_pthread_p;
};

typedef struct {
    int status;
    int GRP_id;
    AX_IVPS_GRP_ATTR_T stGrpAttr;
    AX_IVPS_PIPELINE_ATTR_T stPipelineAttr;
    struct ax_ivps_pthread_par chn_par[5];
    int n_osd_rgn_chn[HAL_AX_IVPS_CHN_MAX];
    int n_osd_rgn_chn_hand[HAL_AX_IVPS_CHN_MAX];
    AX_MOD_INFO_T srcMod;
    AX_MOD_INFO_T ditMod;
} ax_ivps_dev_info;

typedef struct ax_ivps_hal_t {
    ax_ivps_dev_info dev[HAL_AX_IVPS_DEV_MAX];
    int (*InitGRP)(struct ax_ivps_hal_t *self, int GRP, int mode);
    int (*OpenGRP)(struct ax_ivps_hal_t *self, int GRP);
    int (*on_farm)(struct ax_ivps_hal_t *self, int GRP, void *farm);
    int (*on_osd)(struct ax_ivps_hal_t *self, int GRP, int hRegion, void *osd);
    int (*set_chn_farm_on)(struct ax_ivps_hal_t *self, int GRP, int CHN, void (*out_farm)(AX_VIDEO_FRAME_T *fram, void *ctx), void *ctx);
    void (*start)(struct ax_ivps_hal_t *self, int GRP, int CHN);
    void (*stop)(struct ax_ivps_hal_t *self, int GRP, int CHN);
    int (*CloseGRP)(struct ax_ivps_hal_t *self, int GRP);
    void (*set_GRP_mode_par[HAL_AX_IVPS_PAR_MAX])(struct ax_ivps_hal_t *self, int GRP);
    void (*superior_link)(struct ax_ivps_hal_t *self, int GRP, void *Mod);
    AX_MOD_INFO_T (*get_chn_pipe_id)(struct ax_ivps_hal_t *self, int dev, int chn);
} ax_ivps_hal;

// AX_IVPS_Init
int ax_create_ivps(ax_ivps_hal *ivps_dev);
// AX_IVPS_DEinit
void ax_destroy_ivps(ax_ivps_hal *ivps_dev);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ax_ivps_hal_cpp {
private:
    bool exit_flage;

public:
    ax_ivps_hal _dev;
    ax_ivps_hal_cpp()
    {
        ax_create_ivps(&_dev);
        exit_flage = true;
    }
    int InitGRP(int GRP, int mode)
    {
        return _dev.InitGRP(&_dev, GRP, mode);
    }
    int OpenGRP(int GRP)
    {
        return _dev.OpenGRP(&_dev, GRP);
    }
    void start(int GRP, int CHN)
    {
        _dev.start(&_dev, GRP, CHN);
    }

    void exit()
    {
        if (exit_flage) {
            ax_destroy_ivps(&_dev);
            exit_flage = false;
        }
    }
    ~ax_ivps_hal_cpp()
    {
        if (exit_flage) {
            exit();
        }
    }
};

#endif

#endif /* __AX_IVPS_HAL_H_ */
