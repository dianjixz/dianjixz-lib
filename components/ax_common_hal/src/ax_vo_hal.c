/**********************************************************************************
 *
 * Copyright (c) 2019-2020 Beijing AXera Technology Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Beijing AXera Technology Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Beijing AXera Technology Co., Ltd.
 *
 **********************************************************************************/
#include "ax_vo_hal.h"

// static void _calculateFrameRate(ax_sensor_dev_info *self)
// {
//     self->fcnt++;
//     clock_gettime(CLOCK_MONOTONIC, &self->ts2);
//     if ((self->ts2.tv_sec * 1000 + self->ts2.tv_nsec / 1000000) -
//             (self->ts1.tv_sec * 1000 + self->ts1.tv_nsec / 1000000) >=
//         1000) {
//         self->fps                = self->fcnt;
//         self->ts1                = self->ts2;
//         self->fcnt               = 0;
//         self->sernsor_frame_rate = self->fps;
//     }
// }

AX_S32 SAMPLE_VO_CREATE_POOL(AX_U32 u32BlkCnt, AX_U64 u64BlkSize, AX_U64 u64MetaSize, AX_U32 *pPoolID)
{
    AX_POOL_CONFIG_T stPoolCfg = {0};

    stPoolCfg.MetaSize  = u64MetaSize;
    stPoolCfg.BlkCnt    = u32BlkCnt;
    stPoolCfg.BlkSize   = u64BlkSize;
    stPoolCfg.CacheMode = AX_POOL_CACHE_MODE_NONCACHE;
    strcpy((char *)stPoolCfg.PartitionName, "anonymous");

    *pPoolID = AX_POOL_CreatePool(&stPoolCfg);
    if (*pPoolID == AX_INVALID_POOLID) {
        printf("AX_POOL_CreatePool failed, u32BlkCnt = %d, u64BlkSize = 0x%llx, u64MetaSize = 0x%llx\n", u32BlkCnt,
               u64BlkSize, u64MetaSize);
        return -1;
    }

    printf("u32BlkCnt = %d, u64BlkSize = 0x%llx, pPoolID = %d\n", u32BlkCnt, u64BlkSize, *pPoolID);

    return 0;
}
int SAMPLE_VO_POOL_DESTROY(AX_U32 u32PoolID)
{
    return AX_POOL_DestroyPool(u32PoolID);
}
AX_S32 SAMPLE_VO_FMT2ImgStoreInfo(AX_IMG_FORMAT_E enImgFormat, AX_U32 *bytesPerPixel, AX_U32 *nPlanes)
{
    switch (enImgFormat) {
        case AX_FORMAT_YUV420_SEMIPLANAR:
        case AX_FORMAT_YUV420_SEMIPLANAR_VU:
        case AX_FORMAT_YUV422_SEMIPLANAR:
            *bytesPerPixel = 1;
            *nPlanes       = 2;
            break;
        case AX_FORMAT_RGB888:
        case AX_FORMAT_BGR888:
        case AX_FORMAT_RGBA5658:
        case AX_FORMAT_ARGB8565:
        case AX_FORMAT_BGRA5658:
        case AX_FORMAT_ABGR8565:
            *bytesPerPixel = 3;
            *nPlanes       = 1;
            break;
        case AX_FORMAT_RGB565:
        case AX_FORMAT_BGR565:
        case AX_FORMAT_ARGB1555:
        case AX_FORMAT_RGBA5551:
        case AX_FORMAT_ABGR1555:
        case AX_FORMAT_BGRA5551:
        case AX_FORMAT_ARGB4444:
        case AX_FORMAT_RGBA4444:
        case AX_FORMAT_ABGR4444:
        case AX_FORMAT_BGRA4444:
            *bytesPerPixel = 2;
            *nPlanes       = 1;
            break;
        case AX_FORMAT_RGBA8888:
        case AX_FORMAT_ARGB8888:
        case AX_FORMAT_ABGR8888:
        case AX_FORMAT_BGRA8888:
            *bytesPerPixel = 2;
            *nPlanes       = 1;
            break;
        case AX_FORMAT_YUV400:
            *bytesPerPixel = 1;
            *nPlanes       = 1;
        default:
            printf("not support fromat %d", enImgFormat);
            return -1;
    }

    return 0;
}

static void set_Vo_mode_par_0(struct ax_vo_hal_t *self)
{
    self->stVoConfig.u32VDevNr  = 1;
    self->stVoConfig.stVoDev[0] = (SAMPLE_VO_DEV_CONFIG_S){
        .enMode       = AX_VO_MODE_OFFLINE,
        .enVoIntfType = AX_VO_INTF_DSI,
        .enIntfSync   = AX_VO_OUTPUT_USER,
        .u32SyncIndex = SAMPLE_VO_SYNC_USER_CUSTUM,
        .enVoOutfmt   = AX_VO_OUT_FMT_RGB888,
        .stSyncInfos =
            {
                .u16Vact = 1280, /* 垂直有效显示区域 */
                .u16Vbb  = 16,   /* 垂直后肩 */
                .u16Vfb  = 16,   /* 垂直前肩 */
                .u16Hpw  = 4,    /* 水平同步脉冲宽度 */

                .u16Hact = 720,   /* 水平有效显示区域 */
                .u16Hbb  = 40,    /* 水平后肩 */
                .u16Hfb  = 40,    /* 水平前肩 */
                .u16Vpw  = 4,     /* 垂直同步脉冲宽度 */
                // .u32Pclk = 56000, /* 像素时钟，单位 kHz */
                .u32Pclk = 27000, /* 像素时钟，单位 kHz */
                .bIdv    = 0,     /* 数据有效信号的极性（根据需求配置） */
                .bIhs    = 0,     /* 水平同步信号的极性（根据需求配置） */
                .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
            },
    };
    self->stVoConfig.u32LayerNr = 1;
    self->stVoConfig.stVoLayer[0] =
        (SAMPLE_VO_LAYER_CONFIG_S){.bindVoDev     = {[0] = 2, [1] = 2},
                                   .stVoLayerAttr = {.stImageSize     = {.u32Width = 720, .u32Height = 1280},
                                                     .enPixFmt        = AX_FORMAT_YUV420_SEMIPLANAR,
                                                     .u32DispatchMode = 1,
                                                     .f32FrmRate      = 30},
                                   .enVoMode      = VO_MODE_1MUX,
                                   .enChnFrmFmt   = AX_FORMAT_YUV420_SEMIPLANAR,
                                   .u32ChnNr      = 1};
}
static void set_Vo_mode_par_1(struct ax_vo_hal_t *self)
{
    self->stVoConfig.u32VDevNr  = 1;
    self->stVoConfig.stVoDev[0] = (SAMPLE_VO_DEV_CONFIG_S){
        .enMode       = AX_VO_MODE_OFFLINE,
        .enVoIntfType = AX_VO_INTF_DSI,
        .enIntfSync   = AX_VO_OUTPUT_USER,
        .u32SyncIndex = 3,
        .enVoOutfmt   = AX_VO_OUT_FMT_RGB888,
        .stSyncInfos =
            {
                .u16Vact = 1280, /* 垂直有效显示区域 */
                .u16Vbb  = 16,   /* 垂直后肩 */
                .u16Vfb  = 16,   /* 垂直前肩 */
                .u16Hpw  = 4,    /* 水平同步脉冲宽度 */

                .u16Hact = 720,   /* 水平有效显示区域 */
                .u16Hbb  = 40,    /* 水平后肩 */
                .u16Hfb  = 40,    /* 水平前肩 */
                .u16Vpw  = 4,     /* 垂直同步脉冲宽度 */
                .u32Pclk = 56000, /* 像素时钟，单位 kHz */
                .bIdv    = 0,     /* 数据有效信号的极性（根据需求配置） */
                .bIhs    = 0,     /* 水平同步信号的极性（根据需求配置） */
                .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
            },
    };
    self->stVoConfig.u32LayerNr = 1;
    self->stVoConfig.stVoLayer[0] =
        (SAMPLE_VO_LAYER_CONFIG_S){.bindVoDev     = {[0] = 2, [1] = 2},
                                   .stVoLayerAttr = {.stImageSize     = {.u32Width = 720, .u32Height = 1280},
                                                     .enPixFmt        = AX_FORMAT_YUV420_SEMIPLANAR,
                                                     .u32DispatchMode = 1,
                                                     .f32FrmRate      = 30},
                                   .enVoMode      = VO_MODE_1MUX,
                                   .enChnFrmFmt   = AX_FORMAT_YUV420_SEMIPLANAR,
                                   .u32ChnNr      = 1};
}

static int ax_Init(struct ax_vo_hal_t *self, int vo_dev, int mode)
{
    memset(&self->stVoConfig, 0, sizeof(SAMPLE_VO_CONFIG_S));
    self->set_Vo_mode_par[mode](self);
}
static int ax_Open(struct ax_vo_hal_t *self, int vo_dev)
{
    AX_BOOL bNeedWbc = AX_FALSE;
    AX_S32 i, j, s32Chns, s32Ret = 0;
    AX_U32 u32Row, u32Col;
    AX_U32 u32ChnWidth, u32ChnHeight;
    AX_U32 u32LayerWidth  = 0;
    AX_U32 u32LayerHeight = 0;
    AX_U64 u64BlkSize = 0, u64PixSize = 0, u64BlkNr;
    SAMPLE_VO_DEV_CONFIG_S *pstVoDevConf;
    SAMPLE_VO_LAYER_CONFIG_S *pstVoLayerConf;
    AX_VO_VIDEO_LAYER_ATTR_T *pstVoLayerAttr;
    AX_U32 bytesPerPixel = 0, nPlanes = 0;
    self->status = HAL_AX_VO_NONT;

    for (i = 0; i < self->stVoConfig.u32LayerNr; i++) {
        pstVoDevConf   = &self->stVoConfig.stVoDev[i];
        pstVoLayerConf = &self->stVoConfig.stVoLayer[i];

        pstVoLayerConf->u64KeepChnPrevFrameBitmap0 = ~0x0UL;
        pstVoLayerConf->u64KeepChnPrevFrameBitmap1 = ~0x0UL;
        if (self->stVoConfig.u32BindMode) {
            for (j = 0; j < self->stVoConfig.u32VDevNr; j++) {
                pstVoLayerConf->bindVoDev[j] = self->stVoConfig.stVoDev[j].u32VoDev;
                if (self->stVoConfig.stVoDev[j].bWbcEn) {
                    bNeedWbc = self->stVoConfig.stVoDev[j].bWbcEn;
                }
            }
        } else {
            pstVoLayerConf->bindVoDev[0] = pstVoDevConf->u32VoDev;
            bNeedWbc                     = pstVoDevConf->bWbcEn;
        }

        pstVoLayerAttr = &pstVoLayerConf->stVoLayerAttr;

        u32LayerWidth  = pstVoLayerAttr->stImageSize.u32Width;
        u32LayerHeight = pstVoLayerAttr->stImageSize.u32Height;

        SAMPLE_VO_WIN_INFO(u32LayerWidth, u32LayerHeight, pstVoLayerConf->enVoMode, &u32Row, &u32Col, &u32ChnWidth,
                           &u32ChnHeight);

        s32Chns    = u32Row * u32Col;
        u64PixSize = (AX_U64)ALIGN_UP(u32ChnWidth, 8) * u32ChnHeight;
        s32Ret     = SAMPLE_VO_FMT2ImgStoreInfo(pstVoLayerConf->enChnFrmFmt, &bytesPerPixel, &nPlanes);
        if (s32Ret) {
            printf("SAMPLE_VO_FMT2ImgStoreInfo failed, i:%d, enChnFrmFmt:0x%x\n", i, pstVoLayerConf->enChnFrmFmt);
            // goto exit0;
            return -1;
        }

        if (AX_FORMAT_YUV422_SEMIPLANAR == pstVoLayerConf->enChnFrmFmt)
            u64BlkSize = u64PixSize * 4 / nPlanes;
        else
            u64BlkSize = u64PixSize * 3 / nPlanes;

        s32Ret = SAMPLE_VO_CREATE_POOL(s32Chns * 2, u64BlkSize, 512, &pstVoLayerConf->u32ChnPoolId);
        if (s32Ret) {
            printf("creat ChnPool failed, s32Ret = 0x%x\n", s32Ret);
            return -2;
        }
        self->status |= HAL_AX_VO_CHN_POOL;

        u64PixSize = (AX_U64)ALIGN_UP(u32LayerWidth, 8) * ALIGN_UP(u32LayerHeight, 2);
        s32Ret     = SAMPLE_VO_FMT2ImgStoreInfo(pstVoLayerConf->stVoLayerAttr.enPixFmt, &bytesPerPixel, &nPlanes);
        if (s32Ret) {
            printf("SAMPLE_VO_FMT2ImgStoreInfo failed, i:%d, enPixFmt:0x%x\n", i,
                   pstVoLayerConf->stVoLayerAttr.enPixFmt);
            return -3;
        }
        if (AX_FORMAT_YUV422_SEMIPLANAR == pstVoLayerConf->stVoLayerAttr.enPixFmt)
            u64BlkSize = u64PixSize * 4 / nPlanes;
        else
            u64BlkSize = u64PixSize * 3 / nPlanes;
        u64BlkNr = bNeedWbc ? 8 : 4;
        s32Ret   = SAMPLE_VO_CREATE_POOL(u64BlkNr, u64BlkSize, 512, &pstVoLayerConf->u32LayerPoolId);
        if (s32Ret) {
            printf("creat LayerPool failed, i:%d, s32Ret:0x%x\n", i, s32Ret);
            return -4;
        }
        self->status |= HAL_AX_VO_LAYER_POOL;
        pstVoLayerAttr->u32FifoDepth = pstVoLayerConf->u32FifoDepth;
        pstVoLayerAttr->u32PoolId    = pstVoLayerConf->u32LayerPoolId;

        printf("u32LayerPoolId = %d, u32ChnPoolId = %d\n", pstVoLayerConf->u32LayerPoolId,
               pstVoLayerConf->u32ChnPoolId);
    }

    s32Ret = SAMPLE_COMM_VO_StartVO(&self->stVoConfig);
    if (s32Ret) {
        printf("SAMPLE_COMM_VO_StartVO failed, i:%d, s32Ret:0x%x\n", i, s32Ret);
        return -5;
    }
    self->status |= HAL_AX_VO_OPEN;
    return 0;
}
static int ax_Close(struct ax_vo_hal_t *self, int vo_dev)
{
    SAMPLE_VO_LAYER_CONFIG_S *pstVoLayerConf;
    if (self->status & HAL_AX_VO_OPEN) {
        SAMPLE_COMM_VO_StopVO(&self->stVoConfig);
    }
    printf("VO test Finished success!\n");
    if (self->status & HAL_AX_VO_LAYER_POOL) {
        for (int i = 0; i < self->stVoConfig.u32LayerNr; i++) {
            pstVoLayerConf = &self->stVoConfig.stVoLayer[i];
            SAMPLE_VO_POOL_DESTROY(pstVoLayerConf->u32ChnPoolId);
            SAMPLE_VO_POOL_DESTROY(pstVoLayerConf->u32LayerPoolId);
        }
    }

    self->status = HAL_AX_VO_NONT;
    return 0;
}

static AX_VIDEO_FRAME_T *get_frame(struct ax_vo_hal_t *self, AX_U32 u32ChnID, AX_U32 u32LayerID)
{
    AX_S32 s32Ret        = 0;
    AX_U32 bytesPerPixel = 0, nPlanes = 0;
    AX_VOID *u64VirAddr;
    AX_U64 u64PhysAddr;
    ax_vo_dev_info *dev_info = &self->dev[u32ChnID];
    memset(&dev_info->stFrame[u32LayerID], 0, sizeof(AX_VIDEO_FRAME_T));
    dev_info->stFrame[u32LayerID].stCompressInfo = self->stVoConfig.stVoLayer[u32LayerID].chnCompressInfo;
    dev_info->stFrame[u32LayerID].enImgFormat    = self->stVoConfig.stVoLayer[u32LayerID].stVoLayerAttr.enPixFmt;
    dev_info->stFrame[u32LayerID].u32Width = self->stVoConfig.stVoLayer[u32LayerID].stVoLayerAttr.stImageSize.u32Width;
    dev_info->stFrame[u32LayerID].u32Height =
        self->stVoConfig.stVoLayer[u32LayerID].stVoLayerAttr.stImageSize.u32Height;
    SAMPLE_VO_FMT2ImgStoreInfo(dev_info->stFrame[u32LayerID].enImgFormat, &bytesPerPixel, &nPlanes);
    dev_info->stFrame[u32LayerID].u32PicStride[0] = ALIGN_UP(dev_info->stFrame[u32LayerID].u32Width, 8) * bytesPerPixel;

    if (nPlanes == 2) {
        if (AX_FORMAT_YUV422_SEMIPLANAR == dev_info->stFrame[u32LayerID].enImgFormat)
            dev_info->stFrame[u32LayerID].u32FrameSize =
                dev_info->stFrame[u32LayerID].u32PicStride[0] * dev_info->stFrame[u32LayerID].u32Height * 4 / nPlanes;
        else
            dev_info->stFrame[u32LayerID].u32FrameSize =
                dev_info->stFrame[u32LayerID].u32PicStride[0] * dev_info->stFrame[u32LayerID].u32Height * 3 / nPlanes;
    } else
        dev_info->stFrame[u32LayerID].u32FrameSize =
            dev_info->stFrame[u32LayerID].u32PicStride[0] * dev_info->stFrame[u32LayerID].u32Height;

    dev_info->stFrame[u32LayerID].u32BlkId[0] = AX_POOL_GetBlock(self->stVoConfig.stVoLayer[u32LayerID].u32ChnPoolId,
                                                                 dev_info->stFrame[u32LayerID].u32FrameSize, NULL);
    if (AX_INVALID_BLOCKID == dev_info->stFrame[u32LayerID].u32BlkId[0]) {
        printf("layer%d-chn%d AX_POOL_GetBlock failed\n", u32LayerID, u32ChnID);
        usleep(10000);
        return NULL;
    }
    dev_info->stFrame[u32LayerID].u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(dev_info->stFrame[u32LayerID].u32BlkId[0]);
    dev_info->stFrame[u32LayerID].u64VirAddr[0] = 0;
    if (!dev_info->stFrame[u32LayerID].u64PhyAddr[0]) {
        printf("AX_POOL_Handle2PhysAddr failed, BlkId = 0x%x\n", dev_info->stFrame[u32LayerID].u32BlkId[0]);
        AX_POOL_ReleaseBlock(dev_info->stFrame[u32LayerID].u32BlkId[0]);
        return NULL;
    }
    dev_info->stFrame[u32LayerID].u64VirAddr[0] =
        (AX_U64)AX_SYS_Mmap(dev_info->stFrame[u32LayerID].u64PhyAddr[0], dev_info->stFrame[u32LayerID].u32FrameSize);
    if (!dev_info->stFrame[u32LayerID].u64VirAddr[0]) {
        printf("AX_SYS_Mmap failed, u64PhysAddr = 0x%llx, u32FrameSize = 0x%x\n",
               dev_info->stFrame[u32LayerID].u64PhyAddr[0], dev_info->stFrame[u32LayerID].u32FrameSize);
        AX_POOL_ReleaseBlock(dev_info->stFrame[u32LayerID].u32BlkId[0]);
        return NULL;
    }
    return &dev_info->stFrame[u32LayerID];
}
static void *get_frame_mem(struct ax_vo_hal_t *self, AX_U32 u32ChnID, AX_U32 u32LayerID)
{
    AX_VIDEO_FRAME_T *fram = get_frame(self, u32ChnID, u32LayerID);
    return (void *)fram->u64VirAddr[0];
}

static void put_frame_mem(struct ax_vo_hal_t *self, AX_U32 u32ChnID, AX_U32 u32LayerID)
{
    AX_S32 s32Ret                             = 0;
    ax_vo_dev_info *dev_info                  = &self->dev[u32ChnID];
    dev_info->stFrame[u32LayerID].u32BlkId[1] = AX_INVALID_BLOCKID;
    s32Ret = AX_VO_SendFrame(self->stVoConfig.stVoLayer[u32LayerID].u32VoLayer, u32ChnID,
                             &dev_info->stFrame[u32LayerID], -1);
    if (s32Ret) printf("layer%d-chn%d AX_VO_SendFrame failed, s32Ret = 0x%x\n", u32LayerID, u32ChnID, s32Ret);

    AX_SYS_Munmap((AX_VOID *)dev_info->stFrame[u32LayerID].u64VirAddr[0], dev_info->stFrame[u32LayerID].u32FrameSize);
    /* Query Channel Status */
    s32Ret = AX_VO_QueryChnStatus(self->stVoConfig.stVoLayer[u32LayerID].u32VoLayer, u32ChnID, &dev_info->stStatus);
    if (s32Ret) printf("layer%d-chn%d AX_VO_QueryChnStatus failed, s32Ret = 0x%x\n", u32LayerID, u32ChnID, s32Ret);
    usleep(5000);
    AX_POOL_ReleaseBlock(dev_info->stFrame[u32LayerID].u32BlkId[0]);
}

static void ax_start(struct ax_vo_hal_t *self, int Chn)
{
    // if(self->status == AX_SENSOR_ISP_RUN)
    // {
    //     self->dev[Chn].status = 1;
    //     self->dev[Chn].Chn = Chn;
    //     pthread_create(&self->dev[Chn].farm_pthread_p, NULL, get_img_thread, &self->dev[Chn]);
    // }
}
static void ax_stop(struct ax_vo_hal_t *self, int Chn)
{
    // self->dev[Chn].status = 0;
    // // pthread_cancel(self->dev[Chn].farm_pthread_p);
    // pthread_join(self->dev[Chn].farm_pthread_p, NULL);
}

static AX_MOD_INFO_T get_chn_pipe_id(struct ax_vo_hal_t *self, int dev, int chn)
{
    AX_MOD_INFO_T mod_info;
    mod_info.enModId  = AX_ID_VO;
    mod_info.s32GrpId = dev;
    mod_info.s32ChnId = chn;
    return mod_info;
}

static int private_flage = 0;
int ax_create_vo(ax_vo_hal *vo_dev)
{
    AX_S32 axRet;
    if (private_flage) return -1;
    AX_VO_Init();
    memset(vo_dev, 0, sizeof(ax_vo_hal));
    private_flage              = 1;
    vo_dev->InitVo             = ax_Init;
    vo_dev->OpenVo             = ax_Open;
    vo_dev->CloseVo            = ax_Close;
    vo_dev->start              = ax_start;
    vo_dev->stop               = ax_stop;
    vo_dev->get_frame_mem      = get_frame_mem;
    vo_dev->put_frame          = put_frame_mem;
    vo_dev->get_frame          = get_frame;
    vo_dev->get_chn_pipe_id        = get_chn_pipe_id;
    vo_dev->set_Vo_mode_par[HAL_AX_VO_PAR_0] = set_Vo_mode_par_0;
    vo_dev->set_Vo_mode_par[HAL_AX_VO_PAR_1] = set_Vo_mode_par_1;

    return 0;
}
void ax_destroy_vo(ax_vo_hal *vo_dev)
{
    if (private_flage == 0) return;
    // for (int i = 0; i < sizeof(vo_dev->dev) / sizeof(vo_dev->dev[0]); i++) {
    //     if (vo_dev->dev[i].status) {
    //         vo_dev->stop(vo_dev, i);
    //     }
    // }
    if (vo_dev->status != HAL_AX_VO_NONT) {
        vo_dev->CloseVo(vo_dev, 0);
    }
    private_flage = 0;
    AX_VO_Deinit();
}
