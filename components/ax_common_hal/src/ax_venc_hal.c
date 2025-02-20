

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
#include "ax_venc_api.h"
#include "ax_venc_hal.h"

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

// // vnec
// static void set_Chn_mode_par_0(ax_venc_dev_info *self)
// {
//     // config
//     self->config.stRCInfo.eRCType = VENC_RC_VBR;
//     self->config.nGOP = 50;
//     self->config.nBitrate = 4000;
//     self->config.stRCInfo.nMinQp = 10;
//     self->config.stRCInfo.nMaxQp = 51;
//     self->config.stRCInfo.nMinIQp = 10;
//     self->config.stRCInfo.nMaxIQp = 51;
//     self->config.stRCInfo.nIntraQpDelta = -2;
//     self->config.nOffsetCropX = 0;
//     self->config.nOffsetCropY = 0;
//     self->config.nOffsetCropW = 0;
//     self->config.nOffsetCropH = 0;
//     // 输出格式
//     // self->config.ePayloadType = PT_H264;
//     // 输入画面
//     // self->config.nInWidth = 1280;
//     // self->config.nInHeight = 720;

//     // 输入输出帧率
//     // self->config.nSrcFrameRate = 24;
//     // self->config.nDstFrameRate = 24;

//     // 视频信号摆幅选项，支持 Limited Range 和 Full Range。
//     self->stVencChnAttr.stVencAttr.u32VideoRange = 1; /* 0: Narrow Range(NR), Y[16,235], Cb/Cr[16,240]; 1: Full
//     Range(FR), Y/Cb/Cr[0,255] */
//     // 1：link mode,图像自动从上游模块流转到下游模块，不需要用户干预；0：nonlink
//     mode，模块以独立方式工作，不与上下游产生链接关系。用户调用 Send 类 API 向模块发送数据，调用 Get 类 API
//     从模块取出数据。
//     // self->stVencChnAttr.stVencAttr.enLinkMode = AX_LINK_MODE;
//     self->stVencChnAttr.stVencAttr.enLinkMode = AX_NONLINK_MODE;
//     /* GOP Setting */
//     self->stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;

//     // // GDR 功能选项。
//     // stVencChnAttr.stVencAttr.u32GdrDuration = 0;
//     // 编码器输出码流的颗粒度 0，one-slice-one-frame 编码
//     // stVencChnAttr.stVencAttr.u32MbLinesPerSlice = 0;                                 /*get stream mode is slice
//     mode or frame mode?*/
//     // // 编码图像最大宽度，以像素为单位，要求两像素对齐
//     // stVencChnAttr.stVencAttr.u32MaxPicWidth = 0;
//     // // 编码图像最大高度，以像素为单位，要求两像素对齐。
//     // stVencChnAttr.stVencAttr.u32MaxPicHeight = 0;

//     self->config.nStride = self->config.nInWidth;

//     // 编码图像的有效宽度，即每行的有效像素数，应小于等于跨度。
//     self->stVencChnAttr.stVencAttr.u32PicWidthSrc = self->config.nInWidth; /*the picture width*/
//     // 编码图像的有效高度，以像素为单位。
//     self->stVencChnAttr.stVencAttr.u32PicHeightSrc = self->config.nInHeight; /*the picture height*/
//     // 裁剪图像的起始水平 X，Y 坐标。
//     self->stVencChnAttr.stVencAttr.u32CropOffsetX = self->config.nOffsetCropX;
//     self->stVencChnAttr.stVencAttr.u32CropOffsetY = self->config.nOffsetCropY;
//     // 裁剪图像的w,h
//     self->stVencChnAttr.stVencAttr.u32CropWidth = self->config.nOffsetCropW;
//     self->stVencChnAttr.stVencAttr.u32CropHeight = self->config.nOffsetCropH;

//     // ALOGN("VencChn %d:w:%d, h:%d, s:%d, Crop:(%d, %d, %d, %d) rcType:%d, payload:%d", gVencChnMapping[VencChn],
//     stVencChnAttr.stVencAttr.u32PicWidthSrc, stVencChnAttr.stVencAttr.u32PicHeightSrc, config.nStride,
//     stVencChnAttr.stVencAttr.u32CropOffsetX, stVencChnAttr.stVencAttr.u32CropOffsetY,
//     stVencChnAttr.stVencAttr.u32CropWidth, stVencChnAttr.stVencAttr.u32CropHeight, config.stRCInfo.eRCType,
//     config.ePayloadType);
//     // 每个编码通道需要为输出码流准备一个 ring buffer，大小由 u32BufSize 参数决定，以 byte 为单位。推荐计算公式为
//     u32MaxPicWidth × u32MaxPicHeight × 1.5 self->stVencChnAttr.stVencAttr.u32BufSize = self->config.nStride *
//     self->config.nInHeight * 3 / 2; /*stream buffer size*/
//     // 编码协议类型 PT_H264 PT_H265 PT_MJPEG PT_JPEG
//     self->stVencChnAttr.stVencAttr.enType = self->config.ePayloadType;

//     switch (self->stVencChnAttr.stVencAttr.enType)
//     {
//     case PT_H265:
//     {
//         // 编码码流的 Profile 选项，该参数决定编码时可以启用哪些语法特性。
//         self->stVencChnAttr.stVencAttr.enProfile = VENC_HEVC_MAIN_PROFILE;
//         self->stVencChnAttr.stVencAttr.enLevel = VENC_HEVC_LEVEL_6;
//         // 编码码流的 Tier 选项，仅适用于 H.265 协议。 支持 VENC_HEVC_MAIN_TIER, VENC_HEVC_HIGH_TIER
//         self->stVencChnAttr.stVencAttr.enTier = VENC_HEVC_MAIN_TIER;

//         if (self->config.stRCInfo.eRCType == VENC_RC_CBR)
//         {
//             AX_VENC_H265_CBR_S stH265Cbr;
//             // 码控算法的类型。
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
//             // 码控算法的初始 QP 值，范围 [-1, 51]。
//             self->stVencChnAttr.stRcAttr.s32FirstFrameStartQp = -1;
//             stH265Cbr.u32Gop = self->config.nGOP;
//             stH265Cbr.u32SrcFrameRate = self->config.nSrcFrameRate;  /* input frame rate */
//             stH265Cbr.fr32DstFrameRate = self->config.nDstFrameRate; /* target frame rate */
//             stH265Cbr.u32BitRate = self->config.nBitrate;
//             stH265Cbr.u32MinQp = self->config.stRCInfo.nMinQp;
//             stH265Cbr.u32MaxQp = self->config.stRCInfo.nMaxQp;
//             stH265Cbr.u32MinIQp = self->config.stRCInfo.nMinIQp;
//             stH265Cbr.u32MaxIQp = self->config.stRCInfo.nMaxIQp;
//             stH265Cbr.s32IntraQpDelta = self->config.stRCInfo.nIntraQpDelta;
//             memcpy(&self->stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(AX_VENC_H265_CBR_S));
//         }
//         else if (self->config.stRCInfo.eRCType == VENC_RC_VBR)
//         {
//             AX_VENC_H265_VBR_S stH265Vbr;
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
//             self->stVencChnAttr.stRcAttr.s32FirstFrameStartQp = -1;
//             stH265Vbr.u32Gop = self->config.nGOP;
//             stH265Vbr.u32SrcFrameRate = self->config.nSrcFrameRate;
//             stH265Vbr.fr32DstFrameRate = self->config.nDstFrameRate;
//             stH265Vbr.u32MaxBitRate = self->config.nBitrate;
//             stH265Vbr.u32MinQp = self->config.stRCInfo.nMinQp;
//             stH265Vbr.u32MaxQp = self->config.stRCInfo.nMaxQp;
//             stH265Vbr.u32MinIQp = self->config.stRCInfo.nMinIQp;
//             stH265Vbr.u32MaxIQp = self->config.stRCInfo.nMaxIQp;
//             stH265Vbr.s32IntraQpDelta = self->config.stRCInfo.nIntraQpDelta;
//             memcpy(&self->stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(AX_VENC_H265_VBR_S));
//         }
//         else if (self->config.stRCInfo.eRCType == VENC_RC_FIXQP)
//         {
//             AX_VENC_H265_FIXQP_S stH265FixQp;
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
//             stH265FixQp.u32Gop = self->config.nGOP;
//             stH265FixQp.u32SrcFrameRate = self->config.nSrcFrameRate;
//             stH265FixQp.fr32DstFrameRate = self->config.nDstFrameRate;
//             stH265FixQp.u32IQp = 25;
//             stH265FixQp.u32PQp = 30;
//             stH265FixQp.u32BQp = 32;
//             memcpy(&self->stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(AX_VENC_H265_FIXQP_S));
//         }
//         break;
//     }
//     case PT_H264:
//     {
//         // 编码码流的 Profile 选项，该参数决定编码时可以启用哪些语法特性。
//         self->stVencChnAttr.stVencAttr.enProfile = VENC_H264_MAIN_PROFILE;
//         // 编码码流的 Level 选项，解码器的解码能力（解码速度和 buffer 容量）不低于此值才能正确解码。
//         self->stVencChnAttr.stVencAttr.enLevel = VENC_H264_LEVEL_5_2;

//         if (self->config.stRCInfo.eRCType == VENC_RC_CBR)
//         {
//             AX_VENC_H264_CBR_S stH264Cbr;
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
//             self->stVencChnAttr.stRcAttr.s32FirstFrameStartQp = -1;
//             stH264Cbr.u32Gop = self->config.nGOP;
//             stH264Cbr.u32SrcFrameRate = self->config.nSrcFrameRate;  /* input frame rate */
//             stH264Cbr.fr32DstFrameRate = self->config.nDstFrameRate; /* target frame rate */
//             stH264Cbr.u32BitRate = self->config.nBitrate;
//             stH264Cbr.u32MinQp = self->config.stRCInfo.nMinQp;
//             stH264Cbr.u32MaxQp = self->config.stRCInfo.nMaxQp;
//             stH264Cbr.u32MinIQp = self->config.stRCInfo.nMinIQp;
//             stH264Cbr.u32MaxIQp = self->config.stRCInfo.nMaxIQp;
//             stH264Cbr.s32IntraQpDelta = self->config.stRCInfo.nIntraQpDelta;
//             // 码控算法选项
//             memcpy(&self->stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(AX_VENC_H264_CBR_S));
//         }
//         else if (self->config.stRCInfo.eRCType == VENC_RC_VBR)
//         {
//             AX_VENC_H264_VBR_S stH264Vbr;
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
//             self->stVencChnAttr.stRcAttr.s32FirstFrameStartQp = -1;
//             stH264Vbr.u32Gop = self->config.nGOP;
//             stH264Vbr.u32SrcFrameRate = self->config.nSrcFrameRate;
//             stH264Vbr.fr32DstFrameRate = self->config.nDstFrameRate;
//             stH264Vbr.u32MaxBitRate = self->config.nBitrate;
//             stH264Vbr.u32MinQp = self->config.stRCInfo.nMinQp;
//             stH264Vbr.u32MaxQp = self->config.stRCInfo.nMaxQp;
//             stH264Vbr.u32MinIQp = self->config.stRCInfo.nMinIQp;
//             stH264Vbr.u32MaxIQp = self->config.stRCInfo.nMaxIQp;
//             stH264Vbr.s32IntraQpDelta = self->config.stRCInfo.nIntraQpDelta;
//             memcpy(&self->stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(AX_VENC_H264_VBR_S));
//         }
//         else if (self->config.stRCInfo.eRCType == VENC_RC_FIXQP)
//         {
//             AX_VENC_H264_FIXQP_S stH264FixQp;
//             self->stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
//             stH264FixQp.u32Gop = self->config.nGOP;
//             stH264FixQp.u32SrcFrameRate = self->config.nSrcFrameRate;
//             stH264FixQp.fr32DstFrameRate = self->config.nDstFrameRate;
//             stH264FixQp.u32IQp = 25;
//             stH264FixQp.u32PQp = 30;
//             stH264FixQp.u32BQp = 32;
//             memcpy(&self->stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(AX_VENC_H264_FIXQP_S));
//         }
//         break;
//     }
//     default:
//         ALOGE("VencChn %d:Payload type unrecognized.", self->Chn);
//         break;
//     }
// }

// static void set_Chn_mode_par_2(ax_venc_dev_info *self)
// {
//     set_Chn_mode_par_0(self);
//     self->stVencChnAttr.stVencAttr.enLinkMode = AX_LINK_MODE;
// }

// // jpeg
// static void set_Chn_mode_par_1(ax_venc_dev_info *self)
// {
//     self->stVencChnAttr.stVencAttr.u32MaxPicHeight = 32768;
//     self->stVencChnAttr.stVencAttr.u32MaxPicWidth = 32768;

//     self->stVencChnAttr.stVencAttr.enLinkMode = AX_NONLINK_MODE;
// }
// // venc
// static void set_Chn_mode_par_after_1(ax_venc_dev_info *self)
// {
//     AX_S32 s32Ret;
//     memset(&self->stJpegParam, 0, sizeof(self->stJpegParam));
//     s32Ret = AX_VENC_GetJpegParam(self->Chn, &self->stJpegParam);
//     if (AX_SUCCESS != s32Ret)
//     {
//         SAMPLE_ERR_LOG("AX_VENC_GetJpegParam:%d failed!\n", self->Chn);
//         return;
//     }
//     self->stJpegParam.u32Qfactor = self->jpeg_u32Qfactor;
//     s32Ret = AX_VENC_SetJpegParam(self->Chn, &self->stJpegParam);
//     if (AX_SUCCESS != s32Ret)
//     {
//         SAMPLE_ERR_LOG("AX_VENC_SetJpegParam:%d failed!\n", self->Chn);
//     }
// }

// // jpeg
// static void set_Chn_mode_par_3(ax_venc_dev_info *self)
// {
//     set_Chn_mode_par_1(self);
//     self->stVencChnAttr.stVencAttr.enLinkMode = AX_LINK_MODE;
// }
// static void set_Chn_mode_par_after_3(ax_venc_dev_info *self)
// {
//     set_Chn_mode_par_after_1(self);
// }
static void get_eRCType_default(int enType, int eRCType, void *par)
{
    AX_VENC_CHN_ATTR_T *stVencChnAttr = (AX_VENC_CHN_ATTR_T *)par;
    if (enType == PT_H264) {
        stVencChnAttr->stVencAttr.enProfile = AX_VENC_H264_MAIN_PROFILE;
        stVencChnAttr->stVencAttr.enLevel   = AX_VENC_H264_LEVEL_5_2;
        switch (eRCType) {
            case SAMPLE_RC_CBR: {
                AX_VENC_H264_CBR_T stH264Cbr;
                stVencChnAttr->stRcAttr.enRcMode             = AX_VENC_RC_MODE_H264CBR;
                stVencChnAttr->stRcAttr.s32FirstFrameStartQp = -1;
                stH264Cbr.u32Gop                             = 120;
                stH264Cbr.u32BitRate                         = 2048;
                stH264Cbr.u32MinQp                           = 10;
                stH264Cbr.u32MaxQp                           = 51;
                stH264Cbr.u32MinIQp                          = 10;
                stH264Cbr.u32MaxIQp                          = 51;
                stH264Cbr.s32IntraQpDelta                    = -2;
                stH264Cbr.u32MinIprop                        = 10;
                stH264Cbr.u32MaxIprop                        = 40;
                stVencChnAttr->stRcAttr.stH264Cbr            = stH264Cbr;
            } break;
            case SAMPLE_RC_VBR: {
                AX_VENC_H264_VBR_T stH264Vbr;
                stVencChnAttr->stRcAttr.enRcMode             = AX_VENC_RC_MODE_H264VBR;
                stVencChnAttr->stRcAttr.s32FirstFrameStartQp = -1;
                stH264Vbr.u32Gop                             = 120;
                stH264Vbr.u32MaxBitRate                      = 2048;
                stH264Vbr.u32MinQp                           = 10;
                stH264Vbr.u32MaxQp                           = 51;
                stH264Vbr.u32MinIQp                          = 10;
                stH264Vbr.u32MaxIQp                          = 51;
                stH264Vbr.s32IntraQpDelta                    = -2;
                stVencChnAttr->stRcAttr.stH264Vbr            = stH264Vbr;
            } break;
            case SAMPLE_RC_AVBR:
                break;
            case SAMPLE_RC_QPMAP:
                break;
            case SAMPLE_RC_FIXQP: {
                AX_VENC_H264_FIXQP_T stH264FixQp;
                stVencChnAttr->stRcAttr.enRcMode    = AX_VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop                  = 120;
                stH264FixQp.u32IQp                  = 25;
                stH264FixQp.u32PQp                  = 30;
                stH264FixQp.u32BQp                  = 32;
                stVencChnAttr->stRcAttr.stH264FixQp = stH264FixQp;
            } break;
            case SAMPLE_RC_CVBR:
                break;
            default:
                break;
        }
    } else if (enType == PT_H265) {
        stVencChnAttr->stVencAttr.enProfile = AX_VENC_HEVC_MAIN_PROFILE;
        stVencChnAttr->stVencAttr.enLevel   = AX_VENC_HEVC_LEVEL_5_1;
        stVencChnAttr->stVencAttr.enTier    = AX_VENC_HEVC_MAIN_TIER;
        switch (eRCType) {
            case SAMPLE_RC_CBR: {
                AX_VENC_H265_CBR_T stH265Cbr;
                stVencChnAttr->stRcAttr.enRcMode             = AX_VENC_RC_MODE_H265CBR;
                stVencChnAttr->stRcAttr.s32FirstFrameStartQp = -1;
                stH265Cbr.u32Gop                             = 120;
                stH265Cbr.u32BitRate                         = 2048;
                stH265Cbr.u32MinQp                           = 10;
                stH265Cbr.u32MaxQp                           = 51;
                stH265Cbr.u32MinIQp                          = 10;
                stH265Cbr.u32MaxIQp                          = 51;
                stH265Cbr.s32IntraQpDelta                    = -2;
                stH265Cbr.u32MinIprop                        = 30;
                stH265Cbr.u32MaxIprop                        = 40;
                stVencChnAttr->stRcAttr.stH265Cbr            = stH265Cbr;
            } break;
            case SAMPLE_RC_VBR: {
                AX_VENC_H265_VBR_T stH265Vbr;
                stVencChnAttr->stRcAttr.enRcMode             = AX_VENC_RC_MODE_H265VBR;
                stVencChnAttr->stRcAttr.s32FirstFrameStartQp = -1;
                stH265Vbr.u32Gop                             = 120;
                stH265Vbr.u32MaxBitRate                      = 2048;
                stH265Vbr.u32MinQp                           = 10;
                stH265Vbr.u32MaxQp                           = 51;
                stH265Vbr.u32MinIQp                          = 10;
                stH265Vbr.u32MaxIQp                          = 51;
                stH265Vbr.s32IntraQpDelta                    = -2;
                stVencChnAttr->stRcAttr.stH265Vbr            = stH265Vbr;
            } break;
            case SAMPLE_RC_AVBR:
                break;
            case SAMPLE_RC_QPMAP:
                break;
            case SAMPLE_RC_FIXQP: {
                AX_VENC_H265_FIXQP_T stH265FixQp;
                stVencChnAttr->stRcAttr.enRcMode    = AX_VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop                  = 120;
                stH265FixQp.u32IQp                  = 25;
                stH265FixQp.u32PQp                  = 30;
                stH265FixQp.u32BQp                  = 32;
                stVencChnAttr->stRcAttr.stH265FixQp = stH265FixQp;
            } break;
            case SAMPLE_RC_CVBR:
                break;
            default:
                break;
        }
    }
}

static void venc_set_Chn_mode_par_0(struct ax_venc_hal_t *self, int chn)
{
    AX_VENC_CHN_ATTR_T stVencChnAttr;
    memset(&stVencChnAttr, 0, sizeof(AX_VENC_CHN_ATTR_T));
    stVencChnAttr.stVencAttr.u32MaxPicWidth  = self->dev[chn].stVencChnAttr.stVencAttr.u32MaxPicWidth;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = self->dev[chn].stVencChnAttr.stVencAttr.u32MaxPicHeight;
    stVencChnAttr.stVencAttr.u8InFifoDepth   = 4;
    stVencChnAttr.stVencAttr.u8OutFifoDepth  = 4;
    stVencChnAttr.stVencAttr.u32PicWidthSrc  = self->dev[chn].stVencChnAttr.stVencAttr.u32PicWidthSrc;
    stVencChnAttr.stVencAttr.u32PicHeightSrc = self->dev[chn].stVencChnAttr.stVencAttr.u32PicHeightSrc;

    if (stVencChnAttr.stVencAttr.u32MaxPicWidth == 0)
        stVencChnAttr.stVencAttr.u32MaxPicWidth = stVencChnAttr.stVencAttr.u32PicWidthSrc;
    if (stVencChnAttr.stVencAttr.u32MaxPicHeight == 0)
        stVencChnAttr.stVencAttr.u32MaxPicHeight = stVencChnAttr.stVencAttr.u32PicHeightSrc;

    stVencChnAttr.stRcAttr.stFrameRate.fSrcFrameRate =
        self->dev[chn].stVencChnAttr.stRcAttr.stFrameRate.fSrcFrameRate == 0
            ? 30
            : self->dev[chn].stVencChnAttr.stRcAttr.stFrameRate.fSrcFrameRate;
    stVencChnAttr.stRcAttr.stFrameRate.fDstFrameRate =
        self->dev[chn].stVencChnAttr.stRcAttr.stFrameRate.fDstFrameRate == 0
            ? 30
            : self->dev[chn].stVencChnAttr.stRcAttr.stFrameRate.fDstFrameRate;
    stVencChnAttr.stVencAttr.u32BufSize =
        self->dev[chn].stVencChnAttr.stVencAttr.u32BufSize == 0
            ? (stVencChnAttr.stVencAttr.u32PicWidthSrc * stVencChnAttr.stVencAttr.u32PicHeightSrc * 3 / 2)
            : self->dev[chn].stVencChnAttr.stVencAttr.u32BufSize;
    stVencChnAttr.stVencAttr.enLinkMode = AX_LINK_MODE;
    /* GOP Setting */
    stVencChnAttr.stGopAttr.enGopMode = AX_VENC_GOPMODE_NORMALP;
    stVencChnAttr.stVencAttr.enType   = self->dev[chn].stVencChnAttr.stVencAttr.enType;
    get_eRCType_default(stVencChnAttr.stVencAttr.enType, SAMPLE_RC_FIXQP, &stVencChnAttr);
    self->dev[chn].stVencChnAttr = stVencChnAttr;
}

static int hal_OpenChn(ax_venc_hal *self, int Chn, int mode)
{
    AX_S32 ret;
    if (Chn > 9) return -1;
    ax_venc_dev_info *chn_self = &self->dev[Chn];
    self->set_Chn_mode_par[mode](self, Chn);
    ret = AX_VENC_CreateChn(Chn, &chn_self->stVencChnAttr);
    if (AX_SUCCESS != ret) {
        // ALOGE("VencChn %d: AX_VENC_CreateChn failed, s32Ret:0x%x", VencChn, ret);
        return -1;
    }
    self->dev[Chn].status |= AX_VENC_CHN_CREATE;
    return 0;
}

static int hal_SetChnOut(ax_venc_hal *self, int Chn, int w, int h, int fps, AX_PAYLOAD_TYPE_E out_type,
                         void (*out_farm)(Venc_Frame *))
{
    ax_venc_dev_info *chn_self = &self->dev[Chn];
    memset(chn_self, 0, sizeof(ax_venc_dev_info));
    switch (out_type) {
        case PT_H264:
        case PT_H265: {
            chn_self->out_farm                                         = out_farm;
            chn_self->stVencChnAttr.stVencAttr.u32PicWidthSrc          = w;
            chn_self->stVencChnAttr.stVencAttr.u32PicHeightSrc         = h;
            chn_self->stVencChnAttr.stRcAttr.stFrameRate.fSrcFrameRate = fps;
            chn_self->stVencChnAttr.stRcAttr.stFrameRate.fDstFrameRate = fps;
            chn_self->stVencChnAttr.stVencAttr.enType                  = out_type;
        } break;
        case PT_JPEG: {
            chn_self->out_farm = out_farm;
        } break;
        default:
            ALOGE("pipeline_output_e=%d,should not init venc", out_type);
            break;
    }
}

static void *hal_venc_get_farm_pthread(void *p)
{
    AX_S32 s32Ret;
    ax_venc_dev_info *self = (ax_venc_dev_info *)p;
    if (self->status != AX_VENC_CHN_CREATE) return NULL;
    AX_VENC_STREAM_T stStream;

    while (self->status & AX_VENC_CHN_START_RECV) {
        memset(&stStream, 0, sizeof(AX_VENC_STREAM_T));
        s32Ret = AX_VENC_GetStream(self->chn, &stStream, 200);
        if (AX_SUCCESS == s32Ret) {
            venc_buffer_t buffer;
            buffer.n_width       = self->stVencChnAttr.stVencAttr.u32PicWidthSrc;
            buffer.n_height      = self->stVencChnAttr.stVencAttr.u32PicHeightSrc;
            buffer.n_stride      = stStream.stPack.enCodingType;
            buffer.m_output_type = self->stVencChnAttr.stVencAttr.enType;
            buffer.n_size        = stStream.stPack.u32Len;
            buffer.p_vir         = stStream.stPack.pu8Addr;
            buffer.p_phy         = stStream.stPack.ulPhyAddr;
            buffer.n_pts         = stStream.stPack.u64PTS;
            self->out_farm(&buffer);
            AX_VENC_ReleaseStream(self->chn, &stStream);
        }
    }
    ALOGN("VencChn %d: getStream Exit!\n", self->chn);
    return NULL;
}

static int hal_on_farm(ax_venc_hal *self, int Chn, Venc_Frame *buffer)
{
    AX_S32 s32Ret;
    AX_VIDEO_FRAME_INFO_T stFrame;
    static int sendFrameNum = 0;
    if (!(self->dev[Chn].status & AX_VENC_CHN_CREATE)) {
        ALOGW("VencChn %d not open", Chn);
        return -1;
    }
    memset(&stFrame, 0, sizeof(AX_VIDEO_FRAME_INFO_T));
    stFrame.stVFrame.u64PhyAddr[0]   = buffer->p_phy;
    stFrame.stVFrame.u64VirAddr[0]   = buffer->p_vir;
    stFrame.stVFrame.enImgFormat     = buffer->d_type;
    stFrame.stVFrame.u32Width        = buffer->n_width;
    stFrame.stVFrame.u32Height       = buffer->n_height;
    stFrame.stVFrame.u32PicStride[0] = buffer->n_stride;
    // stFrame.stVFrame.u32BlkId[0] = BlkId;
    stFrame.stVFrame.u32BlkId[0] = 0;
    stFrame.stVFrame.u32BlkId[1] = 0;  // must set 0 if not used
    stFrame.stVFrame.u32BlkId[2] = 0;  // must set 0 if not used
    /* the input frame sequence number */
    stFrame.stVFrame.u64SeqNum = sendFrameNum++;
    if (stFrame.stVFrame.enImgFormat == AX_FORMAT_YUV420_PLANAR) {
        stFrame.stVFrame.u32PicStride[1] = stFrame.stVFrame.u32PicStride[0] / 2;
        stFrame.stVFrame.u32PicStride[2] = stFrame.stVFrame.u32PicStride[0] / 2;
    } else if (stFrame.stVFrame.enImgFormat == AX_FORMAT_YUV420_SEMIPLANAR ||
               stFrame.stVFrame.enImgFormat == AX_FORMAT_YUV420_SEMIPLANAR_VU) {
        stFrame.stVFrame.u32PicStride[1] = stFrame.stVFrame.u32PicStride[0];
        stFrame.stVFrame.u32PicStride[2] = 0;
    } else if (stFrame.stVFrame.enImgFormat == AX_FORMAT_YUV422_INTERLEAVED_YUYV ||
               stFrame.stVFrame.enImgFormat == AX_FORMAT_YUV422_INTERLEAVED_YUYV) {
        stFrame.stVFrame.u32PicStride[1] = 0;
        stFrame.stVFrame.u32PicStride[2] = 0;
    }
    s32Ret = AX_VENC_SendFrame(Chn, &stFrame, 200);
    if (AX_SUCCESS != s32Ret) {
        ALOGE("AX_VENC_SendFrame %d failed, ret=%x", Chn, s32Ret);
    }
    return 0;
}

static int hal_start(struct ax_venc_hal_t *self, int Chn)
{
    AX_S32 s32Ret;
    AX_VENC_RECV_PIC_PARAM_T stRecvParam;
    s32Ret = AX_VENC_StartRecvFrame(Chn, &stRecvParam);
    if (AX_SUCCESS != s32Ret) {
        ALOGE("AX_VENC_StartRecvFrame failed, s32Ret:0x%x", s32Ret);
        return -1;
    }
    if (self->dev[Chn].out_farm) {
        self->dev[Chn].status |= AX_VENC_CHN_START_RECV;
        self->dev[Chn].chn = Chn;
        pthread_create(&self->dev[Chn].farm_pthread_p, NULL, hal_venc_get_farm_pthread, &self->dev[Chn]);
    }
    return 0;
}

static void hal_stop(ax_venc_hal *self, int Chn)
{
    if (self->dev[Chn].status & AX_VENC_CHN_START_RECV) {
        self->dev[Chn].status &= ~((int)AX_VENC_CHN_START_RECV);
        if (self->dev[Chn].out_farm) {
            pthread_join(self->dev[Chn].farm_pthread_p, NULL);
        }
        AX_VENC_StopRecvFrame(Chn);
    }
}

static void hal_CloseChn(ax_venc_hal *self, int Chn)
{
    AX_S32 s32Ret;
    if (self->dev[Chn].status & AX_VENC_CHN_START_RECV) {
        self->stop(self, Chn);
    }
    if (self->dev[Chn].status & AX_VENC_CHN_LINK) {
        AX_SYS_UnLink(&self->dev[Chn].srcMod, &self->dev[Chn].ditMod);
    }
    self->dev[Chn].status &= ~((int)AX_VENC_CHN_LINK);
    if (self->dev[Chn].status & AX_VENC_CHN_CREATE) {
        s32Ret = AX_VENC_DestroyChn(Chn);
        if (0 != s32Ret) {
            ALOGE("VencChn %d:AX_VENC_DestroyChn failed,s32Ret:0x%x.\n", Chn, s32Ret);
        }
    }
    self->dev[Chn].status = AX_VENC_CHN_NONT;
}

static int hal_getStatus(ax_venc_hal *self, int Chn)
{
    return self->dev[Chn].status;
}

// static int hal_EncodeOneFrameToJpeg(ax_venc_hal *self, void *dst_pVirAddr, int *dst_size, AX_U32 nStride, AX_U32
// nWidth,
//                                     AX_U32 nHeight, AX_U64 nPhyAddr, void *pVirAddr, AX_U32 nLen)
// {
//     AX_JPEG_ENCODE_ONCE_PARAMS stJpegEncodeOnceParam;
//     memset(&stJpegEncodeOnceParam, 0, sizeof(stJpegEncodeOnceParam));

//     stJpegEncodeOnceParam.stJpegParam.u32Qfactor = 90;
//     stJpegEncodeOnceParam.u64PhyAddr[0]          = nPhyAddr;
//     stJpegEncodeOnceParam.u64PhyAddr[1]          = nPhyAddr + nStride * nHeight;
//     stJpegEncodeOnceParam.u64PhyAddr[2]          = 0;
//     stJpegEncodeOnceParam.u32PicStride[0]        = nStride;
//     stJpegEncodeOnceParam.u32PicStride[1]        = nStride;
//     stJpegEncodeOnceParam.u32PicStride[2]        = nStride;

//     stJpegEncodeOnceParam.u32Width        = nWidth;
//     stJpegEncodeOnceParam.u32Height       = nHeight;
//     stJpegEncodeOnceParam.enImgFormat     = AX_YUV420_SEMIPLANAR;
//     stJpegEncodeOnceParam.s16OffsetLeft   = 0;
//     stJpegEncodeOnceParam.s16OffsetRight  = 0;
//     stJpegEncodeOnceParam.s16OffsetTop    = 0;
//     stJpegEncodeOnceParam.s16OffsetBottom = 0;

//     AX_U64 ulPhyAddr;
//     AX_VOID *pu8Addr;
//     AX_U32 u32Len = nWidth * nHeight * 3 / 2;
//     AX_S32 nRet   = AX_SYS_MemAlloc(&ulPhyAddr, (AX_VOID **)&pu8Addr, u32Len, 256, (AX_S8 *)"SKEL_TEST");

//     if (!pu8Addr) {
//         ALOGE("malloc fail nRet=0x%x", nRet);
//         goto JENC_EXIT;
//     }

//     stJpegEncodeOnceParam.ulPhyAddr = ulPhyAddr;
//     stJpegEncodeOnceParam.pu8Addr   = (AX_U8 *)pu8Addr;
//     stJpegEncodeOnceParam.u32Len    = u32Len;

//     nRet = AX_VENC_JpegEncodeOneFrame(&stJpegEncodeOnceParam);
//     if (0 != nRet) {
//         ALOGE("AX_VENC_JpegEncodeOneFrame, ret = %x.\n", nRet);
//         goto JENC_EXIT;
//     }

// JENC_EXIT:
//     if ((0 == nRet) && dst_pVirAddr) {
//         *dst_size = stJpegEncodeOnceParam.u32Len;
//         memcpy(dst_pVirAddr, stJpegEncodeOnceParam.pu8Addr, *dst_size);
//     }

//     if (ulPhyAddr != 0) {
//         AX_SYS_MemFree(ulPhyAddr, pu8Addr);
//     }

//     return nRet;
// }

static void hal_superior_link(struct ax_venc_hal_t *self, int CHN, void *Mod)
{
    self->dev[CHN].srcMod          = *((AX_MOD_INFO_T *)Mod);
    self->dev[CHN].ditMod.s32GrpId = 0;
    self->dev[CHN].ditMod.enModId  = AX_ID_VENC;
    self->dev[CHN].ditMod.s32ChnId = CHN;
    AX_SYS_Link(&self->dev[CHN].srcMod, &self->dev[CHN].ditMod);
    // self->dev[CHN].status &= AX_IVPS_GRP_LINK;
}

static int private_flage = 0;
int ax_create_venc(ax_venc_hal *venc_dev)
{
    AX_S32 s32Ret;
    if (private_flage) return -1;

    memset(venc_dev, 0, sizeof(ax_venc_hal));
    private_flage                                      = 1;
    venc_dev->stModAttr.enVencType                     = AX_VENC_MULTI_ENCODER;
    venc_dev->stModAttr.stModThdAttr.u32TotalThreadNum = 1;
    venc_dev->stModAttr.stModThdAttr.bExplicitSched    = AX_FALSE;
    s32Ret                                             = AX_VENC_Init(&venc_dev->stModAttr);
    if (AX_SUCCESS != s32Ret) {
        ALOGE("VencChn : AX_VENC_Init failed, s32Ret:0x%x", s32Ret);
        return -1;
    }
    private_flage       = 2;
    venc_dev->OpenChn   = hal_OpenChn;
    venc_dev->SetChnOut = hal_SetChnOut;
    venc_dev->on_farm   = hal_on_farm;
    venc_dev->start     = hal_start;
    venc_dev->stop      = hal_stop;
    venc_dev->getStatus = hal_getStatus;
    // venc_dev->EncodeOneFrameToJpeg = hal_EncodeOneFrameToJpeg;
    venc_dev->superior_link       = hal_superior_link;
    venc_dev->CloseChn            = hal_CloseChn;
    venc_dev->set_Chn_mode_par[0] = venc_set_Chn_mode_par_0;
    return 0;
}
void ax_destroy_venc(ax_venc_hal *venc_dev)
{
    if (private_flage == 2) {
        ALOGI("AX_VENC_Deinit()");
        AX_VENC_Deinit();
        private_flage = 0;
    }
    ALOGI("venc_exit over");
}
