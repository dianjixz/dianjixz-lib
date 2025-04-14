//
// Copyright (c) 2023-2025 dianjixz
//

#ifndef __AX_VENC_HAL_H_
#define __AX_VENC_HAL_H_
#include "my_hal.h"
#include "common_venc.h"

typedef hal_buffer_t Venc_Frame;
typedef hal_buffer_t venc_buffer_t;

typedef enum {
    HAL_AX_VENC_CHN_NONT         = 0,
    HAL_AX_VENC_CHN_CREATE       = 0b1,
    HAL_AX_VENC_CHN_START_RECV   = 0b01,
    HAL_AX_VENC_CHN_LINK         = 0b001
} AX_VENC_CHN_STATUS;

enum {
    HAL_AX_VENC_PAR_0,
    HAL_AX_VENC_PAR_1,
    HAL_AX_VENC_PAR_2,
    HAL_AX_VENC_PAR_3,
    HAL_AX_VENC_PAR_4,
    HAL_AX_VENC_PAR_CUSTOM,
    HAL_AX_VENC_PAR_MAX
};

typedef struct _stRCInfo
{
    SAMPLE_VENC_RC_E eRCType;
    AX_U32 nMinQp;
    AX_U32 nMaxQp;
    AX_U32 nMinIQp;
    AX_U32 nMaxIQp;
    AX_S32 nIntraQpDelta;
} RC_INFO_T;

typedef struct _stVideoConfig
{
    AX_PAYLOAD_TYPE_E ePayloadType;
    AX_U32 nGOP;
    AX_U32 nSrcFrameRate;
    AX_U32 nDstFrameRate;
    AX_U32 nStride;
    AX_S32 nInWidth;
    AX_S32 nInHeight;
    AX_S32 nOutWidth;
    AX_S32 nOutHeight;
    AX_IMG_FORMAT_E eImgFormat;
    RC_INFO_T stRCInfo;
    AX_S32 nBitrate;
    AX_LINK_MODE_E enLinkMode;
} VIDEO_CONFIG_T;

typedef struct {
    VIDEO_CONFIG_T stVideoConfig;
    SAMPLE_VENC_RC_E rcMode;
    int status;
    int chn;
    pthread_t farm_pthread_p;
    void (*out_farm)(Venc_Frame *);
    union {
        AX_VENC_JPEG_PARAM_T stJpegParam;
        AX_VENC_CHN_ATTR_T stVencChnAttr;
        AX_VENC_MJPEG_PARAM_T stMjpegParam;
    };
    AX_MOD_INFO_T srcMod;
    AX_MOD_INFO_T ditMod;
} ax_venc_dev_info;

typedef struct ax_venc_hal_t {
    int ChnSize;
    ax_venc_dev_info dev[10];
    AX_VENC_MOD_ATTR_T stModAttr;
    int (*InitChn)(struct ax_venc_hal_t *, int, int, int, int);
    int (*OpenChn)(struct ax_venc_hal_t *, int, int);
    int (*SetChnOut)(struct ax_venc_hal_t *, int, int, int, int, AX_PAYLOAD_TYPE_E, void (*out_farm)(Venc_Frame *));
    int (*on_farm)(struct ax_venc_hal_t *, int, Venc_Frame *);
    int (*set_farm_on)(struct ax_venc_hal_t *, int, Venc_Frame *);
    void (*start)(struct ax_venc_hal_t *, int);
    void (*stop)(struct ax_venc_hal_t *, int);
    void (*CloseChn)(struct ax_venc_hal_t *, int);
    int (*getStatus)(struct ax_venc_hal_t *, int);
    int (*EncodeOneFrameToJpeg)(struct ax_venc_hal_t *, void *, int *, AX_U32, AX_U32, AX_U32, AX_U64, void *, AX_U32);
    void (*set_Chn_mode_par[HAL_AX_VENC_PAR_MAX])(struct ax_venc_hal_t *, int);
    void (*set_Chn_mode_par_after[HAL_AX_VENC_PAR_MAX])(struct ax_venc_hal_t *);

    void (*superior_link)(struct ax_venc_hal_t *self, int CHN, void *Mod);
    AX_MOD_INFO_T (*get_chn_pipe_id)(struct ax_venc_hal_t *self, int dev, int chn);
} ax_venc_hal;

#ifdef __cplusplus
extern "C" {
#endif

int ax_create_venc(ax_venc_hal *venc_dev);
void ax_destroy_venc(ax_venc_hal *venc_dev);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ax_venc_hal_cpp {
private:
    bool exit_flage;

public:
    ax_venc_hal _dev;
    ax_venc_hal_cpp(/* args */)
    {
        exit_flage = true;
        ax_create_venc(&_dev);
    }
    int SetChnOut(int Chn, int w, int h, int fps, AX_PAYLOAD_TYPE_E out_type, void (*out_farm)(Venc_Frame *))
    {
        return _dev.SetChnOut(&_dev, Chn, w, h, fps, out_type, out_farm);
    }
    int OpenChn(int Chn, int mode)
    {
        return _dev.OpenChn(&_dev, Chn, mode);
    }
    int on_farm(int Chn, Venc_Frame *buffer)
    {
        return _dev.on_farm(&_dev, Chn, buffer);
    }

    void start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }
    void exit()
    {
        if (exit_flage) {
            ax_destroy_venc(&_dev);
            exit_flage = false;
        }
    }

    ~ax_venc_hal_cpp()
    {
        if (exit_flage) {
            exit();
        }
    }
};

#endif

#endif /* __AX_VENC_HAL_H_ */
