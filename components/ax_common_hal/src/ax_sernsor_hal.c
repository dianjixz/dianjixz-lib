/**********************************************************************************
 *
 * Copyright (c) 2019-2020 Beijing AXera Technology Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Beijing AXera Technology Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Beijing AXera Technology Co., Ltd.
 *
 **********************************************************************************/
#include "ax_sernsor_hal.h"
#include "ax_sys_api.h"
/* comm pool */
COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDummySdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 6},                              /*vin raw16 use */
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, AX_COMPRESS_MODE_LOSSY, 4}, /*vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDVPSdr[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 10}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleBTSdr[] = {
    {720, 480, 720, AX_FORMAT_YUV422_INTERLEAVED_UYVY, 25 * 2}, /*vin yuv422 uyvy use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleLVDSSdr[] = {
    {1600, 1200, 1600, AX_FORMAT_YUV420_SEMIPLANAR, 10}, /* vin nv21/nv21 use */
};

/* private pool */
COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummySdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 10, AX_COMPRESS_MODE_LOSSY, 4},
};

COMMON_SYS_POOL_CFG_T gtSysPrivatePoolSingleDVPSdr[] = {
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP, 25 * 2}, /*vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysPrivatePoolSingleBTSdr[] = {
    {720, 480, 720, AX_FORMAT_YUV422_INTERLEAVED_UYVY, 25 * 2}, /*vin yuv422 uyvy use */
};

COMMON_SYS_POOL_CFG_T gtSysPrivatePoolSingleLVDSSdr[] = {
    {1600, 1200, 1600, AX_FORMAT_BAYER_RAW_10BPP, 25 * 2}, /*vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 3, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4},                            /* vin nv21/nv21 use */
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 3},                            /* vin nv21/nv21 use */
    {720, 576, 720, AX_FORMAT_YUV420_SEMIPLANAR, 3},                               /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 12, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw16 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10Bt656Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
    {720, 480, 720, AX_FORMAT_YUV422_SEMIPLANAR, 6, AX_COMPRESS_MODE_NONE, 0},     /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10Bt656Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
    {720, 480, 720, AX_FORMAT_YUV422_SEMIPLANAR, 5, AX_COMPRESS_MODE_NONE, 0},          /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10Bt656Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
    {720, 480, 720, AX_FORMAT_YUV422_SEMIPLANAR, 7, AX_COMPRESS_MODE_NONE, 0},     /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10Bt656Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 13, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
    {720, 480, 720, AX_FORMAT_YUV422_SEMIPLANAR, 5, AX_COMPRESS_MODE_NONE, 0},           /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 6, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 14, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10SdrOnly[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 6, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_NONE, 0},  /* vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 3, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOs450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

/*************************************/
COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc850SlSdr[] = {
    {3840, 2160, 3840, AX_FORMAT_YUV420_SEMIPLANAR, 3, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4},                            /* vin nv21/nv21 use */
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 3},                            /* vin nv21/nv21 use */
    {720, 576, 720, AX_FORMAT_YUV420_SEMIPLANAR, 3},                               /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc850SlSdr[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};
/*************************************/

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 6, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc450aiSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc450aiHdr[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 6, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc450aiHdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_NONE, 0},  /* vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingles5kjn1sq03Sdr[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 3, AX_COMPRESS_MODE_LOSSY, 4}, /* vin nv21/nv21 use */
};

COMMON_SYS_POOL_CFG_T gtPrivatePoolSingles5kjn1sq03Sdr[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, AX_COMPRESS_MODE_LOSSY, 4}, /* vin raw10 use */
};

static AX_VOID __cal_dump_pool(COMMON_SYS_POOL_CFG_T pool[], AX_SNS_HDR_MODE_E eHdrMode, AX_S32 nFrameNum)
{
    if (NULL == pool) {
        return;
    }
    if (nFrameNum > 0) {
        switch (eHdrMode) {
            case AX_SNS_LINEAR_MODE:
                pool[0].nBlkCnt += nFrameNum;
                break;

            case AX_SNS_HDR_2X_MODE:
                pool[0].nBlkCnt += nFrameNum * 2;
                break;

            case AX_SNS_HDR_3X_MODE:
                pool[0].nBlkCnt += nFrameNum * 3;
                break;

            case AX_SNS_HDR_4X_MODE:
                pool[0].nBlkCnt += nFrameNum * 4;
                break;

            default:
                pool[0].nBlkCnt += nFrameNum;
                break;
        }
    }
}

static AX_VOID __set_pipe_hdr_mode(AX_U32 *pHdrSel, AX_SNS_HDR_MODE_E eHdrMode)
{
    if (NULL == pHdrSel) {
        return;
    }

    switch (eHdrMode) {
        case AX_SNS_LINEAR_MODE:
            *pHdrSel = 0x1;
            break;

        case AX_SNS_HDR_2X_MODE:
            *pHdrSel = 0x1 | 0x2;
            break;

        case AX_SNS_HDR_3X_MODE:
            *pHdrSel = 0x1 | 0x2 | 0x4;
            break;

        case AX_SNS_HDR_4X_MODE:
            *pHdrSel = 0x1 | 0x2 | 0x4 | 0x8;
            break;

        default:
            *pHdrSel = 0x1;
            break;
    }
}

static AX_VOID __set_vin_attr(AX_CAMERA_T *pCam, SAMPLE_SNS_TYPE_E eSnsType, AX_SNS_HDR_MODE_E eHdrMode,
                              COMMON_VIN_MODE_E eSysMode, AX_BOOL bAiispEnable)
{
    pCam->eSnsType                              = eSnsType;
    pCam->tSnsAttr.eSnsMode                     = eHdrMode;
    pCam->tDevAttr.eSnsMode                     = eHdrMode;
    pCam->eHdrMode                              = eHdrMode;
    pCam->eSysMode                              = eSysMode;
    pCam->tPipeAttr[pCam->nPipeId].eSnsMode     = eHdrMode;
    pCam->tPipeAttr[pCam->nPipeId].bAiIspEnable = bAiispEnable;

    if (eHdrMode > AX_SNS_LINEAR_MODE) {
        pCam->tDevAttr.eSnsOutputMode = AX_SNS_DOL_HDR;
    }

    if (COMMON_VIN_TPG == eSysMode) {
        pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_TPG;
    }

    if (COMMON_VIN_LOADRAW == eSysMode) {
        pCam->bEnableDev = AX_FALSE;
    } else {
        pCam->bEnableDev = AX_TRUE;
    }
    pCam->bChnEn[0]    = AX_TRUE;
    pCam->bRegisterSns = AX_TRUE;

    return;
}

static AX_U32 __sample_case_single_dummy(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                         SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 i                            = 0;
    AX_CAMERA_T *pCam                   = NULL;
    COMMON_VIN_MODE_E eSysMode          = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode          = pVinParam->eHdrMode;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode = pVinParam->eLoadRawNode;
    pCam                                = &pCamList[0];
    pCommonArgs->nCamCnt                = 1;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam          = &pCamList[i];
        pCam->nPipeId = 0;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                                &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);

        pCam->nDevId                  = 0;
        pCam->nRxDev                  = 0;
        pCam->tSnsClkAttr.nSnsClkIdx  = 0;
        pCam->tDevBindPipe.nNum       = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
        pCam->eLoadRawNode            = eLoadRawNode;
        pCam->eInputMode              = AX_INPUT_MODE_MIPI;
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        for (AX_S32 j = 0; j < AX_VIN_MAX_PIPE_NUM; j++) {
            pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }

    return 0;
}

static AX_U32 __sample_case_single_dvp(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType, SAMPLE_VIN_PARAM_T *pVinParam,
                                       COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt       = 1;
    pCam                       = &pCamList[0];
    pCam->nPipeId              = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_DVP;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (AX_S32 j = 0; j < AX_VIN_MAX_PIPE_NUM; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
    }

    return 0;
}

static AX_U32 __sample_case_single_bt656(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                         SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt       = 1;
    pCam                       = &pCamList[0];
    pCam->nPipeId              = 2;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                                 = 2;
    pCam->nRxDev                                 = 2;
    pCam->tSnsClkAttr.nSnsClkIdx                 = 0;
    pCam->tDevBindPipe.nNum                      = 1;
    pCam->tDevBindPipe.nPipeId[0]                = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId]                = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                               = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode                             = AX_INPUT_MODE_BT656;
    pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode = AX_VIN_PIPE_ISP_BYPASS_MODE;
    pCam->tPipeAttr[pCam->nPipeId].ePixelFmt     = AX_FORMAT_YUV420_SEMIPLANAR;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    pCam->bRegisterSns = AX_FALSE;

    return 0;
}

static AX_U32 __sample_case_single_bt1120(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                          SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt       = 1;
    pCam                       = &pCamList[0];
    pCam->nPipeId              = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                                 = 2;
    pCam->nRxDev                                 = 2;
    pCam->tSnsClkAttr.nSnsClkIdx                 = 0;
    pCam->tDevBindPipe.nNum                      = 1;
    pCam->tDevBindPipe.nPipeId[0]                = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId]                = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                               = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode                             = AX_INPUT_MODE_BT1120;
    pCam->tPipeAttr[pCam->nPipeId].ePipeWorkMode = AX_VIN_PIPE_ISP_BYPASS_MODE;
    pCam->tPipeAttr[pCam->nPipeId].ePixelFmt     = AX_FORMAT_YUV420_SEMIPLANAR;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    pCam->bRegisterSns = AX_FALSE;

    return 0;
}

static AX_U32 __sample_case_single_lvds(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    pCommonArgs->nCamCnt       = 1;
    pCam                       = &pCamList[0];
    pCam->nPipeId              = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_LVDS;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (AX_S32 j = 0; j < AX_VIN_MAX_PIPE_NUM; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
    }

    return 0;
}

static AX_U32 __sample_case_single_os04a10(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                           SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam                   = NULL;
    COMMON_VIN_MODE_E eSysMode          = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode          = pVinParam->eHdrMode;
    AX_S32 j                            = 0;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode = pVinParam->eLoadRawNode;
    pCommonArgs->nCamCnt                = 1;
    pCam                                = &pCamList[0];
    pCam->nPipeId                       = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->eLoadRawNode            = eLoadRawNode;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_MIPI;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
    }
    return 0;
}

static AX_U32 __sample_case_single_sc850sl(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                           SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam                   = NULL;
    COMMON_VIN_MODE_E eSysMode          = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode          = pVinParam->eHdrMode;
    AX_S32 j                            = 0;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode = pVinParam->eLoadRawNode;
    pCommonArgs->nCamCnt                = 1;
    pCam                                = &pCamList[0];
    pCam->nPipeId                       = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->eLoadRawNode            = eLoadRawNode;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_MIPI;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        if (pCam->tPipeInfo[j].bAiispEnable) {
            if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc850sl_sdr.bin",
                        sizeof(pCam->tPipeInfo[j].szBinPath));
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc850sl_hdr_2x.bin",
                        sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        } else {
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }
    return 0;
}

static AX_U32 __sample_case_double_os04a10(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                           SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam          = &pCamList[i];
        pCam->nNumber = i;
        pCam->nPipeId = i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                                &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);

        pCam->nDevId = i;
        if (i == 0) {
            pCam->nRxDev   = 0;
            pCam->nI2cAddr = 0x36;
        } else {
            pCam->nRxDev   = 1;
            pCam->nI2cAddr = 0x36;
        }
        pCam->tSnsClkAttr.nSnsClkIdx  = 0;
        pCam->tDevBindPipe.nNum       = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
        if (eHdrMode == AX_SNS_LINEAR_MODE)
            pCam->tSnsAttr.nSettingIndex = 33;
        else
            pCam->tSnsAttr.nSettingIndex = 34;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;
        pCam->eInputMode         = AX_INPUT_MODE_MIPI;
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        if (pCam->nPipeId != 0 && eHdrMode == AX_SNS_HDR_2X_MODE) {
            pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
        }
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }
    return 0;
}

static AX_U32 __sample_case_single_sc450ai(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                           SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam                   = NULL;
    COMMON_VIN_MODE_E eSysMode          = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode          = pVinParam->eHdrMode;
    AX_S32 j                            = 0;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode = pVinParam->eLoadRawNode;
    pCommonArgs->nCamCnt                = 1;

    pCam          = &pCamList[0];
    pCam->nPipeId = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->eLoadRawNode            = eLoadRawNode;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_MIPI;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        if (pCam->tPipeInfo[j].bAiispEnable) {
            if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_sdr_dual3dnr.bin",
                        sizeof(pCam->tPipeInfo[j].szBinPath));
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_hdr_2x_ainr.bin",
                        sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        } else {
            strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
        }
    }
    return 0;
}

static AX_U32 __sample_case_double_sc450ai(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                           SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam          = &pCamList[i];
        pCam->nNumber = i;
        pCam->nPipeId = i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                                &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);

        pCam->nDevId = i;
        if (i == 0) {
            pCam->nRxDev   = 0;
            pCam->nI2cAddr = 0x30;
        } else {
            pCam->nRxDev   = 1;
            pCam->nI2cAddr = 0x30;
        }
        pCam->tSnsClkAttr.nSnsClkIdx  = 0;
        pCam->tDevBindPipe.nNum       = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
        if (eHdrMode == AX_SNS_LINEAR_MODE)
            pCam->tSnsAttr.nSettingIndex = 33;
        else
            pCam->tSnsAttr.nSettingIndex = 35;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;
        pCam->eInputMode         = AX_INPUT_MODE_MIPI;
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        if (pCam->nPipeId != 0 && eHdrMode == AX_SNS_HDR_2X_MODE) {
            pCam->tPipeAttr[pCam->nPipeId].tCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
        }
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            if (pCam->tPipeInfo[j].bAiispEnable) {
                if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_sdr_dual3dnr.bin",
                            sizeof(pCam->tPipeInfo[j].szBinPath));
                } else {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc450ai_hdr_2x_ainr.bin",
                            sizeof(pCam->tPipeInfo[j].szBinPath));
                }
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        }
    }
    return 0;
}

static AX_U32 __sample_case_double_os04a10_and_bt656(AX_CAMERA_T *pCamList, SAMPLE_VIN_PARAM_T *pVinParam,
                                                     COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam          = NULL;
    COMMON_VIN_MODE_E eSysMode = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode = pVinParam->eHdrMode;
    SAMPLE_SNS_TYPE_E eSnsType;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 3;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        if (i < 2)
            eSnsType = OMNIVISION_OS04A10;
        else {
            eSnsType = SAMPLE_SNS_BT656;
            eHdrMode = AX_SNS_LINEAR_MODE;
        }
        pCam          = &pCamList[i];
        pCam->nNumber = i;
        pCam->nPipeId = i;
        COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                                &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        if (i == 0) {
            pCam->nI2cAddr = 0x36;
        } else if (i == 1) {
            pCam->nI2cAddr = 0x36;
        }
        pCam->tSnsClkAttr.nSnsClkIdx  = 0;
        pCam->tDevBindPipe.nNum       = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
        pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
        if (eHdrMode == AX_SNS_LINEAR_MODE)
            pCam->tSnsAttr.nSettingIndex = 33;
        else
            pCam->tSnsAttr.nSettingIndex = 34;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;
        pCam->eInputMode         = AX_INPUT_MODE_MIPI;
        if (i == 2) {
            pCam->eInputMode   = AX_INPUT_MODE_BT656;
            pCam->bRegisterSns = AX_FALSE;
        }
        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
            if (pCam->tPipeInfo[j].bAiispEnable && i < 2) {
                if (eHdrMode <= AX_SNS_LINEAR_MODE) {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_sdr_dual3dnr.bin",
                            sizeof(pCam->tPipeInfo[j].szBinPath));
                } else {
                    strncpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_hdr_2x_ainr.bin",
                            sizeof(pCam->tPipeInfo[j].szBinPath));
                }
            } else {
                strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
            }
        }
    }
    return 0;
}

static AX_U32 __sample_case_single_s5kjn1sq03(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
                                              SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_CAMERA_T *pCam                   = NULL;
    COMMON_VIN_MODE_E eSysMode          = pVinParam->eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode          = pVinParam->eHdrMode;
    AX_S32 j                            = 0;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode = pVinParam->eLoadRawNode;
    pCommonArgs->nCamCnt                = 1;
    pCam                                = &pCamList[0];
    pCam->nPipeId                       = 0;
    COMMON_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr,
                            &pCam->tPipeAttr[pCam->nPipeId], pCam->tChnAttr);
    pCam->nDevId                  = 0;
    pCam->nRxDev                  = 0;
    pCam->tSnsClkAttr.nSnsClkIdx  = 0;
    pCam->tDevBindPipe.nNum       = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
    pCam->eLoadRawNode            = eLoadRawNode;
    pCam->ptSnsHdl[pCam->nPipeId] = COMMON_ISP_GetSnsObj(eSnsType);
    pCam->eBusType                = COMMON_ISP_GetSnsBusType(eSnsType);
    pCam->eInputMode              = AX_INPUT_MODE_MIPI;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, pVinParam->bAiispEnable);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode    = SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = pVinParam->bAiispEnable;
        strncpy(pCam->tPipeInfo[j].szBinPath, "null.bin", sizeof(pCam->tPipeInfo[j].szBinPath));
    }
    return 0;
}

static AX_U32 __sample_case_config(struct ax_sensor_hal_t *self, SAMPLE_VIN_PARAM_T *pVinParam,
                                   COMMON_SYS_ARGS_T *pCommonArgs, COMMON_SYS_ARGS_T *pPrivArgs)
{
    AX_CAMERA_T *pCamList      = &self->gCams[0];
    SAMPLE_SNS_TYPE_E eSnsType = OMNIVISION_OS04A10;

    COMM_ISP_PRT("eSysCase %d, eSysMode %d, eHdrMode %d, bAiispEnable %d\n", pVinParam->eSysCase, pVinParam->eSysMode,
                 pVinParam->eHdrMode, pVinParam->bAiispEnable);

    switch (pVinParam->eSysCase) {
        case SYS_CASE_SINGLE_DVP:
            eSnsType = SAMPLE_SNS_DVP;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleDVPSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleDVPSdr) / sizeof(gtSysCommPoolSingleDVPSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleDVPSdr;

            /* private pool config */
            __cal_dump_pool(gtSysPrivatePoolSingleDVPSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtSysPrivatePoolSingleDVPSdr) / sizeof(gtSysPrivatePoolSingleDVPSdr[0]);
            pPrivArgs->pPoolCfg    = gtSysPrivatePoolSingleDVPSdr;

            /* cams config */
            __sample_case_single_dvp(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SYS_CASE_SINGLE_BT601:
            eSnsType = SAMPLE_SNS_BT601;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleBTSdr) / sizeof(gtSysCommPoolSingleBTSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleBTSdr;

            /* private pool config */
            __cal_dump_pool(gtSysPrivatePoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtSysPrivatePoolSingleBTSdr) / sizeof(gtSysPrivatePoolSingleBTSdr[0]);
            pPrivArgs->pPoolCfg    = gtSysPrivatePoolSingleBTSdr;

            /* cams config */
            __sample_case_single_bt656(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SYS_CASE_SINGLE_BT656:
            eSnsType = SAMPLE_SNS_BT656;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleBTSdr) / sizeof(gtSysCommPoolSingleBTSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleBTSdr;

            /* private pool config */
            __cal_dump_pool(gtSysPrivatePoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtSysPrivatePoolSingleBTSdr) / sizeof(gtSysPrivatePoolSingleBTSdr[0]);
            pPrivArgs->pPoolCfg    = gtSysPrivatePoolSingleBTSdr;

            /* cams config */
            __sample_case_single_bt656(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SYS_CASE_SINGLE_BT1120:
            eSnsType = SAMPLE_SNS_BT1120;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleBTSdr) / sizeof(gtSysCommPoolSingleBTSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleBTSdr;

            /* private pool config */
            __cal_dump_pool(gtSysPrivatePoolSingleBTSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtSysPrivatePoolSingleBTSdr) / sizeof(gtSysPrivatePoolSingleBTSdr[0]);
            pPrivArgs->pPoolCfg    = gtSysPrivatePoolSingleBTSdr;

            /* cams config */
            __sample_case_single_bt1120(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SYS_CASE_SINGLE_LVDS:
            eSnsType = SAMPLE_SNS_LVDS;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleLVDSSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleLVDSSdr) / sizeof(gtSysCommPoolSingleLVDSSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleLVDSSdr;

            /* private pool config */
            __cal_dump_pool(gtSysPrivatePoolSingleLVDSSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtSysPrivatePoolSingleLVDSSdr) / sizeof(gtSysPrivatePoolSingleLVDSSdr[0]);
            pPrivArgs->pPoolCfg    = gtSysPrivatePoolSingleLVDSSdr;

            /* cams config */
            __sample_case_single_lvds(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_SINGLE_OS04A10:
            eSnsType = OMNIVISION_OS04A10;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Sdr) / sizeof(gtSysCommPoolSingleOs04a10Sdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleOs04a10Sdr;

            /* private pool config */
            __cal_dump_pool(gtPrivatePoolSingleOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOs04a10Sdr) / sizeof(gtPrivatePoolSingleOs04a10Sdr[0]);
            pPrivArgs->pPoolCfg    = gtPrivatePoolSingleOs04a10Sdr;

            /* cams config */
            __sample_case_single_os04a10(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_DOUBLE_OS04A10:
            eSnsType = OMNIVISION_OS04A10;
            /* comm pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtSysCommPoolDoubleOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleOs04a10Sdr) / sizeof(gtSysCommPoolDoubleOs04a10Sdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10Sdr;
            } else {
                __cal_dump_pool(gtSysCommPoolDoubleOs04a10Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleOs04a10Hdr) / sizeof(gtSysCommPoolDoubleOs04a10Hdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10Hdr;
            }

            /* private pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtPrivatePoolDoubleOs04a10Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleOs04a10Sdr) / sizeof(gtPrivatePoolDoubleOs04a10Sdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10Sdr;
            } else if (AX_SNS_LINEAR_ONLY_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtPrivatePoolDoubleOs04a10SdrOnly, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleOs04a10SdrOnly) / sizeof(gtPrivatePoolDoubleOs04a10SdrOnly[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10SdrOnly;
            } else {
                __cal_dump_pool(gtPrivatePoolDoubleOs04a10Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleOs04a10Hdr) / sizeof(gtPrivatePoolDoubleOs04a10Hdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10Hdr;
            }

            /* cams config */
            __sample_case_double_os04a10(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_DOUBLE_SC450AI:
            eSnsType = SMARTSENS_SC450AI;
            /* comm pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtSysCommPoolDoubleSc450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleSc450aiSdr) / sizeof(gtSysCommPoolDoubleSc450aiSdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc450aiSdr;
            } else {
                __cal_dump_pool(gtSysCommPoolDoubleSc450aiHdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleSc450aiHdr) / sizeof(gtSysCommPoolDoubleSc450aiHdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc450aiHdr;
            }

            /* private pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtPrivatePoolDoubleSc450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleSc450aiSdr) / sizeof(gtPrivatePoolDoubleSc450aiSdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc450aiSdr;
            } else {
                __cal_dump_pool(gtPrivatePoolDoubleSc450aiHdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleSc450aiHdr) / sizeof(gtPrivatePoolDoubleSc450aiHdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc450aiHdr;
            }

            /* cams config */
            __sample_case_double_sc450ai(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_SINGLE_SC450AI:
            eSnsType = SMARTSENS_SC450AI;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingleOs450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs450aiSdr) / sizeof(gtSysCommPoolSingleOs450aiSdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleOs450aiSdr;

            /* private pool config */
            __cal_dump_pool(gtPrivatePoolSingleOs450aiSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOs450aiSdr) / sizeof(gtPrivatePoolSingleOs450aiSdr[0]);
            pPrivArgs->pPoolCfg    = gtPrivatePoolSingleOs450aiSdr;

            /* cams config */
            __sample_case_single_sc450ai(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
            case SAMPLE_VIN_SINGLE_SC850SL:
                eSnsType = SMARTSENS_SC850SL;
                /* comm pool config */
                __cal_dump_pool(gtSysCommPoolSingleSc850SlSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc850SlSdr) / sizeof(gtSysCommPoolSingleSc850SlSdr[0]);
                pCommonArgs->pPoolCfg    = gtSysCommPoolSingleSc850SlSdr;

                /* private pool config */
                __cal_dump_pool(gtPrivatePoolSingleSc850SlSdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc850SlSdr) / sizeof(gtPrivatePoolSingleSc850SlSdr[0]);
                pPrivArgs->pPoolCfg    = gtPrivatePoolSingleSc850SlSdr;

                /* cams config */
                __sample_case_single_sc850sl(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_DOUBLE_OS04A10_AND_BT656:
            /* comm pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtSysCommPoolDoubleOs04a10Bt656Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleOs04a10Bt656Sdr) / sizeof(gtSysCommPoolDoubleOs04a10Bt656Sdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10Bt656Sdr;
            } else {
                __cal_dump_pool(gtSysCommPoolDoubleOs04a10Bt656Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pCommonArgs->nPoolCfgCnt =
                    sizeof(gtSysCommPoolDoubleOs04a10Bt656Hdr) / sizeof(gtSysCommPoolDoubleOs04a10Bt656Hdr[0]);
                pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10Bt656Hdr;
            }

            /* private pool config */
            if (AX_SNS_LINEAR_MODE == pVinParam->eHdrMode) {
                __cal_dump_pool(gtPrivatePoolDoubleOs04a10Bt656Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleOs04a10Bt656Sdr) / sizeof(gtPrivatePoolDoubleOs04a10Bt656Sdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10Bt656Sdr;
            } else {
                __cal_dump_pool(gtPrivatePoolDoubleOs04a10Bt656Hdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
                pPrivArgs->nPoolCfgCnt =
                    sizeof(gtPrivatePoolDoubleOs04a10Bt656Hdr) / sizeof(gtPrivatePoolDoubleOs04a10Bt656Hdr[0]);
                pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10Bt656Hdr;
            }

            /* cams config */
            __sample_case_double_os04a10_and_bt656(pCamList, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_SINGLE_S5KJN1SQ03:
            eSnsType = SAMSUNG_S5KJN1SQ03;
            /* comm pool config */
            __cal_dump_pool(gtSysCommPoolSingles5kjn1sq03Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pCommonArgs->nPoolCfgCnt =
                sizeof(gtSysCommPoolSingles5kjn1sq03Sdr) / sizeof(gtSysCommPoolSingles5kjn1sq03Sdr[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingles5kjn1sq03Sdr;

            /* private pool config */
            __cal_dump_pool(gtPrivatePoolSingles5kjn1sq03Sdr, pVinParam->eHdrMode, pVinParam->nDumpFrameNum);
            pPrivArgs->nPoolCfgCnt =
                sizeof(gtPrivatePoolSingles5kjn1sq03Sdr) / sizeof(gtPrivatePoolSingles5kjn1sq03Sdr[0]);
            pPrivArgs->pPoolCfg = gtPrivatePoolSingles5kjn1sq03Sdr;

            /* cams config */
            __sample_case_single_s5kjn1sq03(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
        case SAMPLE_VIN_SINGLE_DUMMY:
        default:
            eSnsType = SAMPLE_SNS_DUMMY;
            /* pool config */
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleDummySdr) / sizeof(gtSysCommPoolSingleDummySdr[0]);
            pCommonArgs->pPoolCfg    = gtSysCommPoolSingleDummySdr;

            /* private pool config */
            pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummySdr) / sizeof(gtPrivatePoolSingleDummySdr[0]);
            pPrivArgs->pPoolCfg    = gtPrivatePoolSingleDummySdr;

            /* cams config */
            __sample_case_single_dummy(pCamList, eSnsType, pVinParam, pCommonArgs);
            break;
    }

    return 0;
}

static void sernsor_calculateFrameRate(ax_sensor_dev_info *self)
{
    self->fcnt++;
    clock_gettime(CLOCK_MONOTONIC, &self->ts2);
    if ((self->ts2.tv_sec * 1000 + self->ts2.tv_nsec / 1000000) -
            (self->ts1.tv_sec * 1000 + self->ts1.tv_nsec / 1000000) >=
        1000) {
        self->fps                = self->fcnt;
        self->ts1                = self->ts2;
        self->fcnt               = 0;
        self->sernsor_frame_rate = self->fps;
    }
}

static void ax_sernsor_cam_init(struct ax_sensor_hal_t *self)
{
}

static void set_Sensor_mode_par_0(struct ax_sensor_hal_t *self)
{
    self->VinParam.eSysMode = COMMON_VIN_SENSOR;
    self->VinParam.eHdrMode = AX_SNS_LINEAR_MODE;
    self->VinParam.bAiispEnable = AX_TRUE;
    __sample_case_config(self, &self->VinParam, &self->tCommonArgs, &self->tPrivArgs);
}
static void set_Sensor_mode_par_1(struct ax_sensor_hal_t *self)
{
    set_Sensor_mode_par_0(self);
    AX_SYS_SetVINIVPSMode(0, 1, AX_GDC_ONLINE_VPP);
}

static int ax_InitSensor(struct ax_sensor_hal_t *self, int sensor_id, int mode)
{
    self->VinParam.eSysCase = (SAMPLE_VIN_CASE_E)sensor_id;
    self->set_Sensor_mode_par[mode](self);
}
static int ax_OpenSensor(struct ax_sensor_hal_t *self)
{
    AX_S32 axRet;
    // 用户进行初始化操作
    // /* Step3: NPU Init */
    // // AX_ENGINE_Deinit
    // axRet = COMMON_NPU_Init();
    // if (axRet) {
    //     COMM_ISP_PRT("COMMON_NPU_Init fail, ret:0x%x", axRet);
    //     return -1;
    // }
    // self->status |= AX_SENSOR_NPU_ENABLE;
    /* Step4: Cam Init */
    axRet = COMMON_CAM_Init();
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_Init fail, ret:0x%x", axRet);
        return -2;
    }
    self->status |= AX_SENSOR_CAM_ENABLE;
    axRet = COMMON_CAM_PrivPoolInit(&self->tPrivArgs);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_PrivPoolInit fail, ret:0x%x", axRet);
        return -3;
    }
    self->status |= AX_SENSOR_CAM_POOL_ENABLE;
    /* Step5: Cam Open */
    axRet = COMMON_CAM_Open(&self->gCams[0], self->tCommonArgs.nCamCnt);
    if (axRet) {
        COMM_ISP_PRT("COMMON_CAM_Open fail, ret:0x%x", axRet);
        return -4;
    }
    self->status |= AX_SENSOR_CAM_OPEN;
    return 0;
}
static int ax_CloseSensor(struct ax_sensor_hal_t *self)
{
    if (self->status & AX_SENSOR_CAM_OPEN) {
        COMMON_CAM_Close(&self->gCams[0], self->tCommonArgs.nCamCnt);
    }
    self->status &= ~((int)AX_SENSOR_CAM_OPEN);
    if (self->status & AX_SENSOR_CAM_ENABLE) {
        COMMON_CAM_Deinit();
    }
    self->status &= ~((int)AX_SENSOR_CAM_ENABLE);
    self->status = AX_SENSOR_NONT;
    return 0;
}

static void *get_img_thread(void *par)
{
    ax_sensor_dev_info *self = (ax_sensor_dev_info *)par;

    // while (self->status)
    // {
    //     AX_S32 axRet = AX_VIN_GetYuvFrame(self->nPipeId, (AX_YUV_SOURCE_ID_E)self->Chn, &self->ax_img, 500);
    //     if (axRet == 0)
    //     {
    //         sernsor_calculateFrameRate(self);
    //         self->ax_img.tFrameInfo.stVFrame.u64VirAddr[0] =
    //         (AX_U32)AX_POOL_GetBlockVirAddr(self->ax_img.tFrameInfo.stVFrame.u32BlkId[0]);
    //         self->ax_img.tFrameInfo.stVFrame.u64PhyAddr[0] =
    //         AX_POOL_Handle2PhysAddr(self->ax_img.tFrameInfo.stVFrame.u32BlkId[0]); self->out_farm(&self->ax_img);
    //         AX_VIN_ReleaseYuvFrame(self->nPipeId, self->Chn, &self->ax_img);
    //     }
    //     else
    //     {
    //         ALOGD("get ax img error! code:0x%x", axRet);
    //         usleep(10 * 1000);
    //     }
    // }
    return NULL;
}

static int ax_SetSensorOut(struct ax_sensor_hal_t *self, int Chn, void (*out_farm)(Sensor_Frame *))
{
    self->dev[Chn].out_farm = out_farm;
}

static void ax_start(struct ax_sensor_hal_t *self, int Chn)
{
    if (self->status & AX_SENSOR_CAM_OPEN) {
        if (self->dev[Chn].out_farm != NULL) {
            self->dev[Chn].status = DEVICE_RUN;
            self->dev[Chn].Chn    = Chn;
            pthread_create(&self->dev[Chn].farm_pthread_p, NULL, get_img_thread, &self->dev[Chn]);
        }
    }
}
static void ax_stop(struct ax_sensor_hal_t *self, int Chn)
{
    if (self->dev[Chn].out_farm != NULL) {
        self->dev[Chn].status = DEVICE_NONE;
        pthread_join(self->dev[Chn].farm_pthread_p, NULL);
    }
}

static int ax_GetFrameRate(struct ax_sensor_hal_t *self, int chn)
{
    return self->dev[chn].sernsor_frame_rate;
}
static void *get_link_mod(struct ax_sensor_hal_t *self, int chn)
{
    return &self->dev[chn].selfMod;
}

static int private_flage = 0;
int ax_create_sensor(ax_sensor_hal *sensor_dev)
{
    AX_S32 axRet;
    if (private_flage) return -1;
    memset(sensor_dev, 0, sizeof(ax_sensor_hal));
    private_flage                      = 1;
    sensor_dev->InitSensor             = ax_InitSensor;
    sensor_dev->OpenSensor             = ax_OpenSensor;
    sensor_dev->CloseSensor            = ax_CloseSensor;
    sensor_dev->SetSensorOut           = ax_SetSensorOut;
    sensor_dev->start                  = ax_start;
    sensor_dev->stop                   = ax_stop;
    sensor_dev->GetFrameRate           = ax_GetFrameRate;
    sensor_dev->get_link_mod           = get_link_mod;
    sensor_dev->set_Sensor_mode_par[0] = set_Sensor_mode_par_0;
    sensor_dev->set_Sensor_mode_par[1] = set_Sensor_mode_par_1;
    for (size_t i = 0; i < AX_MAX_SENSOR_CHN_; i++) {
        sensor_dev->dev[i].selfMod.enModId  = AX_ID_VIN;
        sensor_dev->dev[i].selfMod.s32GrpId = 0;
        sensor_dev->dev[i].selfMod.s32ChnId = i;
    }
    return 0;
}
void ax_destroy_sensor(ax_sensor_hal *sensor_dev)
{
    if (private_flage == 0) return;
    for (int i = 0; i < sizeof(sensor_dev->dev) / sizeof(sensor_dev->dev[0]); i++) {
        if (sensor_dev->dev[i].status) {
            sensor_dev->stop(sensor_dev, i);
        }
    }
    if (sensor_dev->status != AX_SENSOR_NONT) {
        sensor_dev->CloseSensor(sensor_dev);
    }
    private_flage = 0;
}
