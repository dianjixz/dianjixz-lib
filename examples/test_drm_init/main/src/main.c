/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

 #include <stdlib.h>
 #include <string.h>
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/mman.h>
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <errno.h>
 #include <unistd.h>
 #include <pthread.h>
 #include <signal.h>
 
 #include "ax_sys_api.h"
 #include "ax_base_type.h"
 #include "ax_vo_api.h"
 
 #include "common_vo.h"
 #include "common_vo_pattern.h"
 #include "sample_vo_common.h"
 // #include "ax_vo_ini.h"
 
 static SAMPLE_VO_LAYER_CONFIG_S g_stLayerConf = {
     .bindVoDev = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
     .stVoLayerAttr =
         {
             .stImageSize     = {1920, 1080},
             .enPixFmt        = AX_FORMAT_YUV420_SEMIPLANAR,
             .enSyncMode      = AX_VO_LAYER_SYNC_NORMAL,
             .u32PrimaryChnId = 0,
             .u32FifoDepth    = 0,
             .u32BkClr        = 0,
             .enWBMode        = AX_VO_LAYER_WB_POOL,
             .u32InplaceChnId = 0,
             .u32PoolId       = 0,
             .u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO,
             .enPartMode      = AX_VO_PART_MODE_SINGLE,
         },
     .enVoMode                   = VO_MODE_1MUX,
     .u64KeepChnPrevFrameBitmap0 = ~0x0UL,
     .u64KeepChnPrevFrameBitmap1 = ~0x0UL,
     .enChnFrmFmt                = AX_FORMAT_YUV420_SEMIPLANAR,
     .u32ChnNr                   = 16,
 };
 
 static SAMPLE_VO_CONFIG_S g_stVoConf = {
     .stVoLayer =
         {
             {
                 .bindVoDev   = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
                 .enChnFrmFmt = AX_FORMAT_YUV420_SEMIPLANAR,
             },
             {
                 .bindVoDev   = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
                 .enChnFrmFmt = AX_FORMAT_YUV420_SEMIPLANAR,
             },
         },
 };
 
 // AX_VO_INI_K_SECTION_TYPE         0 sectionType = layer_display                            1
 // AX_VO_INI_K_VO_DEVICE            1 VoDev = 0                                              0
 // AX_VO_INI_K_D_INTERFACE          32 disp_interface_type = VO_INTF_DSI
 // AX_VO_INI_K_D_SYNC_TYPE          33 disp_sync_type = VO_OUTPUT_USER
 // AX_VO_INI_K_D_SYNC_USER_INDEX    34 disp_sync_user_index = 0
 // AX_VO_INI_K_L_IMG_WIDTH          9 layer_img_width  = 480
 // AX_VO_INI_K_L_IMG_HEIGHT         10 layer_img_height = 360
 // AX_VO_INI_K_L_IMG_FMT            11 layer_img_fmt = AX_FORMAT_YUV420_SEMIPLANAR
 // AX_VO_INI_K_L_SYNC_MODE          12 layer_sync_mode = VO_LAYER_SYNC_NORMAL
 // AX_VO_INI_K_L_PRIMERY_CHNID      13 layer_primary_chnId = 0
 // AX_VO_INI_K_L_FRAME_RATE         14 layer_frame_rate = 60
 // AX_VO_INI_K_L_CHNS               16 layer_chns = 1
 // AX_VO_INI_K_C_VO_MODE            2 chn_vo_mode = VO_MODE_1MUX
 // AX_VO_INI_K_L_DISPATCH_MODE      20 layer_dispatch_mode = VO_LAYER_OUT_TO_FIFO
 
 // OFFLINE  DSI-DV-RG24      V0           342        20003        50.00(50 )  720 -732 -796 -864  576 -581 -586 -625
 // 27000   N-N-N     P



    // /************************************************************************/
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vact                   = 1280;  /* 垂直有效显示区域 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vbb                    = 16;    /* 垂直后肩 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vfb                    = 16;    /* 垂直前肩 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vpw                    = 4;     /* 垂直同步脉冲宽度 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hact                   = 720;   /* 水平有效显示区域 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hbb                    = 40;    /* 水平后肩 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hfb                    = 40;    /* 水平前肩 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hpw                    = 4;     /* 水平同步脉冲宽度 */
    // self->stVoConfig.stVoDev[0].stSyncInfos.u32Pclk                   = 27000; /* 像素时钟，单位 kHz */
    // self->stVoConfig.stVoDev[0].stSyncInfos.bIdv                      = AX_FALSE;     /* 数据有效信号的极性（根据需求配置） */
    // self->stVoConfig.stVoDev[0].stSyncInfos.bIhs                      = AX_FALSE;     /* 水平同步信号的极性（根据需求配置） */
    // self->stVoConfig.stVoDev[0].stSyncInfos.bIvs                      = AX_FALSE;     /* 垂直同步信号的极性（根据需求配置） */
    // /************************************************************************/







 AX_S32 main(AX_S32 argc, AX_CHAR *argv[])
 {
     AX_S32 c;
     AX_S32 casenum = 0, layerCase = -1;
     AX_S32 s32Ret           = 0;
     AX_U32 u32DevId         = SAMPLE_VO_DEV0;
     AX_U64 u64SampleTestBit = 0;
     // AX_VO_DISPLAY_MODE_T stDisplayMode;
     AX_CHAR section[32] = {
         0,
     };
 
     SAMPLE_PRT("VO Sample. Build at %s %s\n", __DATE__, __TIME__);
 
     // if (argc < 2) {
     //     SAMPLE_VO_Usage(argv[0]);
     //     return 0;
     // }
 
     signal(SIGPIPE, SIG_IGN);
     signal(SIGINT, SAMPLE_VO_SigInt);
     signal(SIGTSTP, SAMPLE_VO_SigStop);
     signal(SIGQUIT, SAMPLE_VO_SigStop);
 
     casenum = 26;
     u64SampleTestBit |= SAMPLE_VO_LAYER_DISPLAY_TEST;
 
     s32Ret = AX_SYS_Init();
     if (s32Ret) {
         SAMPLE_PRT("AX_SYS_Init failed, s32Ret = 0x%x\n", s32Ret);
         return -1;
     }
 
     s32Ret = AX_POOL_Exit();
     if (s32Ret) {
         SAMPLE_PRT("AX_POOL_Exit failed, s32Ret = 0x%x\n", s32Ret);
         goto exit0;
     }
 
     if (u64SampleTestBit & SAMPLE_VO_LAYER_DISPLAY_TEST) {
         // sprintf(section, "[case%d]", casenum);
         // s32Ret = SAMPLE_VO_PARSE_INI(section, AX_VO_INI_S_LAYER_DISPLAY, &g_stVoConf, "./vo.ini");
 
         memset(&g_stVoConf, 0, sizeof(g_stVoConf));
         // .enMode = AX_VO_MODE_OFFLINE,
         // .enVoIntfType = AX_VO_INTF_DSI,
         // .enIntfSync = AX_VO_OUTPUT_USER
         g_stVoConf = (SAMPLE_VO_CONFIG_S){
             .u32BindMode    = 0,
             .u32FifoDepth   = 0,
             .u32VDevNr      = 1,
             .stVoDev        = {[0] = {.s32InitFlag   = 0,
                                       .u32VoDev      = 0,
                                       .enMode        = AX_VO_MODE_OFFLINE,
                                       .enVoIntfType  = AX_VO_INTF_DSI,
                                       .enVoOutfmt    = AX_VO_OUT_FMT_RGB888,
                                       .setCsc        = AX_FALSE,
                                       .vo_csc        = {.enCscMatrix  = AX_VO_CSC_MATRIX_IDENTITY,
                                                         .u32Luma      = 0,
                                                         .u32Contrast  = 0,
                                                         .u32Hue       = 0,
                                                         .u32Satuature = 0},
                                       .bWbcEn        = AX_FALSE,
                                       .u32WbcFrmaeNr = 0,
                                       .stWbcAttr     = {.enSourceType = AX_VO_WBC_SOURCE_DEV,
                                                         .enMode       = AX_VO_WBC_MODE_NORMAL,
                                                         .u32FifoDepth = 0,
                                                         .f32FrameRate = 0},
                                       .enIntfSync    = AX_VO_OUTPUT_720P60,  // AX_VO_OUTPUT_720x1280_60 AX_VO_OUTPUT_USER
                                       .u32SyncIndex  = SAMPLE_VO_SYNC_USER_CUSTUM,
                                       .stSyncInfos =
                                           {
                                               .u16Vact = 1280, /* 垂直有效显示区域 */
                                               .u16Vbb  = 16,   /* 垂直后肩 */
                                               .u16Vfb  = 16,   /* 垂直前肩 */
                                               .u16Vpw  = 4,   /* 垂直同步脉冲宽度 */
 
                                               .u16Hact = 720, /* 水平有效显示区域 */
                                               .u16Hbb  = 40,  /* 水平后肩 */
                                               .u16Hfb  = 40,  /* 水平前肩 */
                                               .u16Hpw  = 4,    /* 水平同步脉冲宽度 */
                                               .u32Pclk = 27000, /* 像素时钟，单位 kHz */
                                            //    .u32Pclk = 54000, /* 像素时钟，单位 kHz */
                                            //    .u32Pclk = 74250, /* 像素时钟，单位 kHz */
                                               .bIdv    = 0,     /* 数据有效信号的极性（根据需求配置） */
                                               .bIhs    = 0,     /* 水平同步信号的极性（根据需求配置） */
                                               .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
                                           },
                                        // {
                                        //     .u16Vact = 720, /* 垂直有效显示区域 */
                                        //     .u16Vbb  = 20,   /* 垂直后肩 */
                                        //     .u16Vfb  = 5,   /* 垂直前肩 */
                                        //     .u16Vpw  = 5,   /* 垂直同步脉冲宽度 */

                                        //     .u16Hact = 1280, /* 水平有效显示区域 */
                                        //     .u16Hbb  = 220,  /* 水平后肩 */
                                        //     .u16Hfb  = 1760,  /* 水平前肩 */
                                        //     .u16Hpw  = 40,    /* 水平同步脉冲宽度 */
                                        //     // .u32Pclk = 27000, /* 像素时钟，单位 kHz */
                                        //  //    .u32Pclk = 74250, /* 像素时钟，单位 kHz */
                                        //     .u32Pclk = 74250, /* 像素时钟，单位 kHz */
                                        //     .bIdv    = 1,     /* 数据有效信号的极性（根据需求配置） */
                                        //     .bIhs    = 1,     /* 水平同步信号的极性（根据需求配置） */
                                        //     .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
                                        // },
                                     }
                                 },
             .u32LayerNr     = 0,
             .stVoLayer      = {[0] = {.s32InitFlag      = 0,
                                       .u32VoLayer       = 0,
                                       .u32LayerPoolId   = 0,
                                       .bindVoDev        = {[0] = 2, [1] = 2},
                                       .stVoLayerAttr    = {.stImageSize    = {.u32Width = 1280, .u32Height = 720},
                                                            .stCompressInfo = {.enCompressMode   = AX_COMPRESS_MODE_NONE,
                                                                               .u32CompressLevel = 0},
                                                            .enPixFmt =
                                                                AX_FORMAT_YUV420_SEMIPLANAR,  // AX_FORMAT_YUV420_SEMIPLANAR
                                                                                              // AX_FORMAT_BGR888
                                                            .enSyncMode      = AX_VO_LAYER_SYNC_NORMAL,
                                                            .u32PrimaryChnId = 0,
                                                            .u32FifoDepth    = 0,
                                                            .u32BkClr        = 0,
                                                            .u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO,
                                                            .enWBMode        = AX_VO_LAYER_WB_POOL,
                                                            .u32InplaceChnId = 0,
                                                            .u32PoolId       = 0,
                                                            .enPartMode      = AX_VO_PART_MODE_SINGLE,
                                                            .enBlendMode     = AX_VO_BLEND_MODE_DEFAULT,
                                                            .enEngineMode    = AX_VO_ENGINE_MODE_AUTO,
                                                            .u32EngineId     = 0,
                                                            .u32Toleration   = 0,
                                                            .f32FrmRate      = 30},
                                       .s32BatchProcFlag = 0,
                                       .enVoMode         = VO_MODE_1MUX,
                                       .enChnFrmFmt =
                                           AX_FORMAT_YUV420_SEMIPLANAR,  // AX_FORMAT_YUV420_SEMIPLANAR AX_FORMAT_RGB888
                                       .u32ChnFrameRate = 0,
                                       .u32ChnFrameNr   = 0,
                                       .u32ChnFrameOut  = 0,
                                       .u32FifoDepth    = 0,
                                       .u32ChnPoolId    = 0,
                                       .chnCompressInfo = {.enCompressMode = AX_COMPRESS_MODE_NONE, .u32CompressLevel = 0},
                                       .u64KeepChnPrevFrameBitmap0 = 0,
                                       .u64KeepChnPrevFrameBitmap1 = 0,
                                       .u32ChnNr                   = 1,
                                       .stSrcMod                   = {.enModId = AX_ID_MIN, .s32GrpId = 0, .s32ChnId = 0},
                                       .stDstMod                   = {.enModId = AX_ID_MIN, .s32GrpId = 0, .s32ChnId = 0}}},
             .stGraphicLayer = {[0] = {.s32InitFlag = 0,
                                       .u32FbNum    = 0,
                                       .stFbConf    = {[0] = {.u32Index       = 0,
                                                              .u32ResoW       = 0,
                                                              .u32ResoH       = 0,
                                                              .u32Fmt         = 0,
                                                              .u32ColorKeyEn  = 0,
                                                              .u32ColorKeyInv = 0,
                                                              .u32ColorKey    = 0}},
                                       .bindVoDev   = 0}},
             .stCursorLayer  = {.s32InitFlag      = 0,
                                .u32CursorLayerEn = 0,
                                .bindVoDev        = 0,
                                .u32FBIndex       = 0,
                                .u32X             = 0,
                                .u32Y             = 0,
                                .u32Width         = 0,
                                .u32Height        = 0}};
 
         // g_stVoConf.stVoDev[0].u32VoDev = 0;
         // g_stVoConf.stVoDev[1].u32VoDev = 0;
         // g_stVoConf.stVoDev[0].enVoIntfType = AX_VO_INTF_DSI;
         // g_stVoConf.stVoDev[1].enVoIntfType = AX_VO_INTF_DSI;
 
         // g_stVoConf.stVoDev[0].enIntfSync = AX_VO_OUTPUT_USER;
         // g_stVoConf.stVoDev[1].enIntfSync = AX_VO_OUTPUT_USER;
         // g_stVoConf.stVoDev[0].u32SyncIndex = 0;
         // g_stVoConf.stVoDev[1].u32SyncIndex = 0;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.stImageSize.u32Width = 720;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.stImageSize.u32Width = 720;
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.stImageSize.u32Height = 1280;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.stImageSize.u32Height = 1280;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.enSyncMode = AX_VO_LAYER_SYNC_NORMAL;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.enSyncMode = AX_VO_LAYER_SYNC_NORMAL;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.u32PrimaryChnId = 0;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.u32PrimaryChnId = 0;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.f32FrmRate = 30;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.f32FrmRate = 30;
 
         // g_stVoConf.stVoLayer[0].u32ChnNr = 1;
         // g_stVoConf.stVoLayer[1].u32ChnNr = 1;
 
         // g_stVoConf.stVoLayer[0].enVoMode = VO_MODE_1MUX;
         // g_stVoConf.stVoLayer[1].enVoMode = VO_MODE_1MUX;
 
         // g_stVoConf.stVoLayer[0].stVoLayerAttr.u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO;
         // g_stVoConf.stVoLayer[1].stVoLayerAttr.u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO;
 


        //  SAMPLE_VO_SigStop(0);
         SAMPLE_VO_LAYER_DISPLAY(&g_stVoConf);
     }

     



     while (!SAMPLE_VO_CheckSig()) {
         sleep(1);
     }
 
 exit0:
 
     AX_SYS_Deinit();
 
     SAMPLE_PRT("done, s32Ret = 0x%x\n", s32Ret);
 
     return s32Ret;
 }
 