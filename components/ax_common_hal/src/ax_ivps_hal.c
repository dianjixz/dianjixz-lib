

// /**********************************************************************************
//  *
//  * Copyright (c) 2019-2020 Beijing AXera Technology Co., Ltd. All Rights Reserved.
//  *
//  * This source file is the property of Beijing AXera Technology Co., Ltd. and
//  * may not be copied or distributed in any isomorphic form without the prior
//  * written consent of Beijing AXera Technology Co., Ltd.
//  *
//  **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "ax_sys_api.h"
#include "ax_ivps_api.h"
// #include "sample_log.h"
#include "ax_ivps_hal.h"

#define SAMPLE_NAME "SampleVenc"
#define VENC_LOG(str, arg...)                                                  \
    do {                                                                       \
        printf("%s: %s:%d " str "\n", SAMPLE_NAME, __func__, __LINE__, ##arg); \
    } while (0)

#define SAMPLE_LOG(str, arg...)                                                \
    do {                                                                       \
        printf("%s: %s:%d " str "\n", SAMPLE_NAME, __func__, __LINE__, ##arg); \
    } while (0)

#define SAMPLE_ERR_LOG(str, arg...)                                                       \
    do {                                                                                  \
        printf("%s: %s:%d Error! " str "\n", "sample_venc.c", __func__, __LINE__, ##arg); \
    } while (0)

#ifndef ALIGN_UP
#define ALIGN_UP(x, align) ((((x) + ((align) - 1)) / (align)) * (align))
#endif

static void hal_set_GRP_mode_par_1(struct ax_ivps_hal_t *self, int GRP)
{
    self->dev[GRP].stGrpAttr.ePipeline      = AX_IVPS_PIPELINE_DEFAULT;
    self->dev[GRP].stGrpAttr.nInFifoDepth   = 2;
    AX_IVPS_PIPELINE_ATTR_T *stPipelineAttr = &self->dev[GRP].stPipelineAttr;
    stPipelineAttr->nOutChnNum              = 3;
    // vin 输入
    stPipelineAttr->tFilter[0][0].bEngage       = AX_TRUE;
    stPipelineAttr->tFilter[0][0].nDstPicWidth  = 2688;
    stPipelineAttr->tFilter[0][0].nDstPicHeight = 1520;
    stPipelineAttr->tFilter[0][0].nDstPicStride = ALIGN_UP(stPipelineAttr->tFilter[0][0].nDstPicWidth, 16);
    stPipelineAttr->tFilter[0][0].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    stPipelineAttr->tFilter[0][0].eEngine       = AX_IVPS_ENGINE_VPP;
    // // 全尺寸 chn0
    stPipelineAttr->tFilter[1][0].bEngage       = AX_TRUE;
    stPipelineAttr->tFilter[1][0].nDstPicWidth  = 2688;
    stPipelineAttr->tFilter[1][0].nDstPicHeight = 1520;
    stPipelineAttr->tFilter[1][0].nDstPicStride = ALIGN_UP(stPipelineAttr->tFilter[1][0].nDstPicWidth, 16);
    stPipelineAttr->tFilter[1][0].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    stPipelineAttr->tFilter[1][0].eEngine       = AX_IVPS_ENGINE_VPP;
    // ai 推理 chn1
    stPipelineAttr->tFilter[2][0].bEngage                 = AX_TRUE;
    stPipelineAttr->tFilter[2][0].nDstPicWidth            = 640;
    stPipelineAttr->tFilter[2][0].nDstPicHeight           = 640;
    stPipelineAttr->tFilter[2][0].nDstPicStride           = ALIGN_UP(stPipelineAttr->tFilter[2][0].nDstPicWidth, 16);
    stPipelineAttr->tFilter[2][0].eDstPicFormat           = AX_FORMAT_RGB888;
    stPipelineAttr->tFilter[2][0].eEngine                 = AX_IVPS_ENGINE_TDP;
    stPipelineAttr->tFilter[2][0].tTdpCfg.eRotation       = AX_IVPS_ROTATION_0;
    stPipelineAttr->tFilter[2][0].tAspectRatio.eMode      = AX_IVPS_ASPECT_RATIO_AUTO;
    stPipelineAttr->tFilter[2][0].tAspectRatio.eAligns[0] = AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER;
    stPipelineAttr->tFilter[2][0].tAspectRatio.eAligns[1] = AX_IVPS_ASPECT_RATIO_VERTICAL_CENTER;
    stPipelineAttr->nOutFifoDepth[1]                      = 1;
    // VO 显示 chn2
    stPipelineAttr->tFilter[3][0].bEngage       = AX_TRUE;
    stPipelineAttr->tFilter[3][0].nDstPicWidth  = 1280;
    stPipelineAttr->tFilter[3][0].nDstPicHeight = 720;
    stPipelineAttr->tFilter[3][0].nDstPicStride = ALIGN_UP(stPipelineAttr->tFilter[3][0].nDstPicWidth, 16);
    stPipelineAttr->tFilter[3][0].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    stPipelineAttr->tFilter[3][0].eEngine       = AX_IVPS_ENGINE_TDP;
    stPipelineAttr->nOutFifoDepth[2]            = 1;
}
static int hal_InitGRP(struct ax_ivps_hal_t *self, int GRP, int mode)
{
    if (GRP == AUTO_GRP_ID) {
        for (size_t i = 0; i < 20; i++) {
            if (self->dev->status == 0) {
                GRP = i;
                break;
            }
        }
    }
    self->set_GRP_mode_par[mode](self, GRP);
    return GRP;
}
static int hal_OpenGRP(struct ax_ivps_hal_t *self, int GRP)
{
    int s32Ret;
    // 创建组
    s32Ret = AX_IVPS_CreateGrp(GRP, &self->dev[GRP].stGrpAttr);
    if (0 != s32Ret) {
        printf("AX_IVPS_CreateGrp failed,nGrp %d,s32Ret:0x%x\n", GRP, s32Ret);
        return -2;
    }
    self->dev[GRP].status |= AX_IVPS_GRP_CREATE;
    // 设置组过滤器
    s32Ret = AX_IVPS_SetPipelineAttr(GRP, &self->dev[GRP].stPipelineAttr);
    if (0 != s32Ret) {
        printf("AX_IVPS_SetPipelineAttr failed,nGrp %d,s32Ret:0x%x\n", GRP, s32Ret);
        return -3;
    }
    self->dev[GRP].status |= AX_IVPS_GRP_SET;
    for (size_t i = 0; i < 5; i++) {
        if (self->dev[GRP].stPipelineAttr.tFilter[i][0].bEngage) {
            AX_IVPS_EnableChn(GRP, i);
        }
    }
    self->dev[GRP].status |= AX_IVPS_GRP_ENABLE;
    s32Ret = AX_IVPS_StartGrp(GRP);
    if (0 != s32Ret) {
        printf("AX_IVPS_StartGrp failed,nGrp %d,s32Ret:0x%x\n", GRP, s32Ret);
        return -4;
    }
    self->dev[GRP].status |= AX_IVPS_GRP_START;
    return self->dev[GRP].status;
}

static int hal_on_farm(struct ax_ivps_hal_t *self, int GRP, void *farm)
{
    AX_IVPS_SendFrame(GRP, (AX_VIDEO_FRAME_T *)farm, -1);
}
const int tmp_tran[] = {0x00, 0x01, 0x10, 0x11, 0x20, 0x21, 0x30, 0x31, 0x40, 0x41, 0x50, 0x51};
static int hal_on_osd(struct ax_ivps_hal_t *self, int GRP, int hRegion, void *osd)
{
    if (self->dev[GRP].n_osd_rgn_chn[hRegion]) {
        return AX_IVPS_RGN_Update(self->dev[GRP].n_osd_rgn_chn_hand[hRegion], osd);
    } else {
        IVPS_RGN_HANDLE hChnRgn = AX_IVPS_RGN_Create();
        if (AX_IVPS_INVALID_REGION_HANDLE != hChnRgn) {
            int nRet = AX_IVPS_RGN_AttachToFilter(hChnRgn, GRP, tmp_tran[hRegion]);
            if (0 != nRet) {
                printf("AX_IVPS_RGN_AttachToFilter(Grp: %d, Filter: 0x%x) failed, ret=0x%x", GRP, tmp_tran[hRegion],
                       nRet);
                return nRet;
            }
            self->dev[GRP].n_osd_rgn_chn_hand[hRegion] = hChnRgn;
            self->dev[GRP].n_osd_rgn_chn[hRegion]      = 1;
            return AX_IVPS_RGN_Update(self->dev[GRP].n_osd_rgn_chn_hand[hRegion], osd);
        }
        return 0;
    }
}
static int hal_set_chn_farm_on(struct ax_ivps_hal_t *self, int GRP, int CHN, void (*out_farm)(AX_VIDEO_FRAME_T *fram, void *ctx), void *ctx)
{
    self->dev[GRP].chn_par[CHN].out_farm = out_farm;
    self->dev[GRP].chn_par[CHN].ctx = ctx;
}
static void *hal_ivps_get_farm_pthread(void *p)
{
    struct ax_ivps_pthread_par *par = (struct ax_ivps_pthread_par *)p;
    while (!par->exit) {
        AX_VIDEO_FRAME_T tVideoFrame;
        // printf("AX_IVPS_GetChnFrame CHN:%d\n", par->CHN);
        AX_S32 ret = AX_IVPS_GetChnFrame(par->GRP, par->CHN, &tVideoFrame, 200);
        if (0 != ret) {
            if (AX_ERR_IVPS_BUF_EMPTY == ret) {
                usleep(1000);
                continue;
            }
            usleep(1000);
            continue;
        }
        // printf("AX_POOL_GetBlockVirAddr CHN:%d\n", par->CHN);
        tVideoFrame.u64VirAddr[0] = (AX_U64)AX_POOL_GetBlockVirAddr(tVideoFrame.u32BlkId[0]);
        tVideoFrame.u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(tVideoFrame.u32BlkId[0]);
        par->out_farm(&tVideoFrame, par->ctx);
        // printf("AX_IVPS_ReleaseChnFrame CHN:%d\n", par->CHN);
        AX_IVPS_ReleaseChnFrame(par->GRP, par->CHN, &tVideoFrame);
    }
    return NULL;
}
static void hal_start(struct ax_ivps_hal_t *self, int GRP, int CHN)
{
    if (self->dev[GRP].chn_par[CHN].out_farm) {
        struct ax_ivps_pthread_par *par = &self->dev[GRP].chn_par[CHN];
        par->GRP                        = GRP;
        par->CHN                        = CHN;
        par->exit                       = 0;
        pthread_create(&par->farm_pthread_p, NULL, hal_ivps_get_farm_pthread, par);
    }
}
static void hal_stop(struct ax_ivps_hal_t *self, int GRP, int CHN)
{
    if (self->dev[GRP].chn_par[CHN].out_farm) {
        self->dev[GRP].chn_par[CHN].exit = 1;
        pthread_join(self->dev[GRP].chn_par[CHN].farm_pthread_p, NULL);
    }
}
static int hal_CloseGRP(struct ax_ivps_hal_t *self, int GRP)
{
    for (size_t i = 0; i < 5; i++) {
        if (self->dev[GRP].chn_par[i].out_farm) {
            self->stop(self, GRP, i);
        }
    }
    if (self->dev[GRP].status & AX_IVPS_GRP_LINK) {
        AX_SYS_UnLink(&self->dev[GRP].srcMod, &self->dev[GRP].ditMod);
        self->dev[GRP].status &= ~((int)AX_IVPS_GRP_LINK);
    }

    for (int i = 0; i < 12; i++) {
        if (self->dev[GRP].n_osd_rgn_chn[i]) {
            AX_IVPS_RGN_DetachFromFilter(self->dev[GRP].n_osd_rgn_chn_hand[i], GRP, (IVPS_FILTER)tmp_tran[i]);
        }
    }

    if (self->dev[GRP].status & AX_IVPS_GRP_OSD) {
        self->dev[GRP].status &= ~((int)AX_IVPS_GRP_OSD);
    }

    if (self->dev[GRP].status & AX_IVPS_GRP_START) {
        AX_IVPS_StopGrp(GRP);
        self->dev[GRP].status &= ~((int)AX_IVPS_GRP_START);
    }

    if (self->dev[GRP].status & AX_IVPS_GRP_ENABLE) {
        for (size_t i = 0; i < 5; i++) {
            if (self->dev[GRP].stPipelineAttr.tFilter[i][0].bEngage) {
                AX_IVPS_DisableChn(GRP, i);
            }
        }
        self->dev[GRP].status &= ~((int)AX_IVPS_GRP_ENABLE);
    }

    if (self->dev[GRP].status & AX_IVPS_GRP_CREATE) {
        AX_IVPS_DestoryGrp(GRP);
    }
    self->dev[GRP].status = AX_IVPS_GRP_NONT;
}
static void hal_superior_link(struct ax_ivps_hal_t *self, int GRP, void *Mod)
{
    self->dev[GRP].srcMod          = *((AX_MOD_INFO_T *)Mod);
    self->dev[GRP].ditMod.s32GrpId = GRP;
    self->dev[GRP].ditMod.enModId  = AX_ID_IVPS;
    self->dev[GRP].ditMod.s32ChnId = 0;
    AX_SYS_Link(&self->dev[GRP].srcMod, &self->dev[GRP].ditMod);
    self->dev[GRP].status &= AX_IVPS_GRP_LINK;
}

static int private_flage = 0;
int ax_create_ivps(ax_ivps_hal *ivps_dev)
{
    AX_S32 s32Ret;
    if (private_flage) return -1;
    memset(ivps_dev, 0, sizeof(ax_ivps_hal));
    private_flage = 1;
    s32Ret        = AX_IVPS_Init();
    if (s32Ret) {
        return -1;
    }
    private_flage                 = 2;
    ivps_dev->InitGRP             = hal_InitGRP;
    ivps_dev->OpenGRP             = hal_OpenGRP;
    ivps_dev->on_farm             = hal_on_farm;
    ivps_dev->on_osd              = hal_on_osd;
    ivps_dev->set_chn_farm_on     = hal_set_chn_farm_on;
    ivps_dev->start               = hal_start;
    ivps_dev->stop                = hal_stop;
    ivps_dev->CloseGRP            = hal_CloseGRP;
    ivps_dev->set_GRP_mode_par[0] = hal_set_GRP_mode_par_1;
    ivps_dev->superior_link       = hal_superior_link;
    return 0;
}
void ax_destroy_ivps(ax_ivps_hal *ivps_dev)
{
    if (private_flage == 2) {
        AX_IVPS_Deinit();
        private_flage = 0;
    }
    // ALOGI("ivps_exit over");
}
