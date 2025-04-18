// /**************************************************************************************************
//  *
//  * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
//  *
//  * This source file is the property of Axera Semiconductor Co., Ltd. and
//  * may not be copied or distributed in any isomorphic form without the prior
//  * written consent of Axera Semiconductor Co., Ltd.
//  *
//  **************************************************************************************************/

//  #include <stdlib.h>
//  #include <string.h>
//  #include <stdio.h>
//  #include <sys/types.h>
//  #include <sys/stat.h>
//  #include <sys/mman.h>
//  #include <sys/ioctl.h>
//  #include <fcntl.h>
//  #include <errno.h>
//  #include <unistd.h>
//  #include <pthread.h>
//  #include <signal.h>

//  #include "ax_sys_api.h"
//  #include "ax_base_type.h"
//  #include "ax_vo_api.h"

//  #include "common_vo.h"
//  #include "common_vo_pattern.h"
//  #include "sample_vo_common.h"
//  // #include "ax_vo_ini.h"

//  static SAMPLE_VO_LAYER_CONFIG_S g_stLayerConf = {
//      .bindVoDev = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
//      .stVoLayerAttr =
//          {
//              .stImageSize     = {1920, 1080},
//              .enPixFmt        = AX_FORMAT_YUV420_SEMIPLANAR,
//              .enSyncMode      = AX_VO_LAYER_SYNC_NORMAL,
//              .u32PrimaryChnId = 0,
//              .u32FifoDepth    = 0,
//              .u32BkClr        = 0,
//              .enWBMode        = AX_VO_LAYER_WB_POOL,
//              .u32InplaceChnId = 0,
//              .u32PoolId       = 0,
//              .u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO,
//              .enPartMode      = AX_VO_PART_MODE_SINGLE,
//          },
//      .enVoMode                   = VO_MODE_1MUX,
//      .u64KeepChnPrevFrameBitmap0 = ~0x0UL,
//      .u64KeepChnPrevFrameBitmap1 = ~0x0UL,
//      .enChnFrmFmt                = AX_FORMAT_YUV420_SEMIPLANAR,
//      .u32ChnNr                   = 16,
//  };

//  static SAMPLE_VO_CONFIG_S g_stVoConf = {
//      .stVoLayer =
//          {
//              {
//                  .bindVoDev   = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
//                  .enChnFrmFmt = AX_FORMAT_YUV420_SEMIPLANAR,
//              },
//              {
//                  .bindVoDev   = {SAMPLE_VO_DEV_MAX, SAMPLE_VO_DEV_MAX},
//                  .enChnFrmFmt = AX_FORMAT_YUV420_SEMIPLANAR,
//              },
//          },
//  };

//  // AX_VO_INI_K_SECTION_TYPE         0 sectionType = layer_display                            1
//  // AX_VO_INI_K_VO_DEVICE            1 VoDev = 0                                              0
//  // AX_VO_INI_K_D_INTERFACE          32 disp_interface_type = VO_INTF_DSI
//  // AX_VO_INI_K_D_SYNC_TYPE          33 disp_sync_type = VO_OUTPUT_USER
//  // AX_VO_INI_K_D_SYNC_USER_INDEX    34 disp_sync_user_index = 0
//  // AX_VO_INI_K_L_IMG_WIDTH          9 layer_img_width  = 480
//  // AX_VO_INI_K_L_IMG_HEIGHT         10 layer_img_height = 360
//  // AX_VO_INI_K_L_IMG_FMT            11 layer_img_fmt = AX_FORMAT_YUV420_SEMIPLANAR
//  // AX_VO_INI_K_L_SYNC_MODE          12 layer_sync_mode = VO_LAYER_SYNC_NORMAL
//  // AX_VO_INI_K_L_PRIMERY_CHNID      13 layer_primary_chnId = 0
//  // AX_VO_INI_K_L_FRAME_RATE         14 layer_frame_rate = 60
//  // AX_VO_INI_K_L_CHNS               16 layer_chns = 1
//  // AX_VO_INI_K_C_VO_MODE            2 chn_vo_mode = VO_MODE_1MUX
//  // AX_VO_INI_K_L_DISPATCH_MODE      20 layer_dispatch_mode = VO_LAYER_OUT_TO_FIFO

//  // OFFLINE  DSI-DV-RG24      V0           342        20003        50.00(50 )  720 -732 -796 -864  576 -581 -586 -625
//  // 27000   N-N-N     P

//     // /************************************************************************/
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vact                   = 1280;  /* 垂直有效显示区域 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vbb                    = 16;    /* 垂直后肩 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vfb                    = 16;    /* 垂直前肩 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Vpw                    = 4;     /* 垂直同步脉冲宽度 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hact                   = 720;   /* 水平有效显示区域 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hbb                    = 40;    /* 水平后肩 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hfb                    = 40;    /* 水平前肩 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u16Hpw                    = 4;     /* 水平同步脉冲宽度 */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.u32Pclk                   = 27000; /* 像素时钟，单位 kHz */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.bIdv                      = AX_FALSE;     /*
//     数据有效信号的极性（根据需求配置） */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.bIhs                      = AX_FALSE;     /*
//     水平同步信号的极性（根据需求配置） */
//     // self->stVoConfig.stVoDev[0].stSyncInfos.bIvs                      = AX_FALSE;     /*
//     垂直同步信号的极性（根据需求配置） */
//     // /************************************************************************/

//  AX_S32 main(AX_S32 argc, AX_CHAR *argv[])
//  {
//      AX_S32 c;
//      AX_S32 casenum = 0, layerCase = -1;
//      AX_S32 s32Ret           = 0;
//      AX_U32 u32DevId         = SAMPLE_VO_DEV0;
//      AX_U64 u64SampleTestBit = 0;
//      // AX_VO_DISPLAY_MODE_T stDisplayMode;
//      AX_CHAR section[32] = {
//          0,
//      };

//      SAMPLE_PRT("VO Sample. Build at %s %s\n", __DATE__, __TIME__);

//      // if (argc < 2) {
//      //     SAMPLE_VO_Usage(argv[0]);
//      //     return 0;
//      // }

//      signal(SIGPIPE, SIG_IGN);
//      signal(SIGINT, SAMPLE_VO_SigInt);
//      signal(SIGTSTP, SAMPLE_VO_SigStop);
//      signal(SIGQUIT, SAMPLE_VO_SigStop);

//      casenum = 26;
//      u64SampleTestBit |= SAMPLE_VO_LAYER_DISPLAY_TEST;

//      s32Ret = AX_SYS_Init();
//      if (s32Ret) {
//          SAMPLE_PRT("AX_SYS_Init failed, s32Ret = 0x%x\n", s32Ret);
//          return -1;
//      }

//      s32Ret = AX_POOL_Exit();
//      if (s32Ret) {
//          SAMPLE_PRT("AX_POOL_Exit failed, s32Ret = 0x%x\n", s32Ret);
//          goto exit0;
//      }

//      if (u64SampleTestBit & SAMPLE_VO_LAYER_DISPLAY_TEST) {
//          // sprintf(section, "[case%d]", casenum);
//          // s32Ret = SAMPLE_VO_PARSE_INI(section, AX_VO_INI_S_LAYER_DISPLAY, &g_stVoConf, "./vo.ini");

//          memset(&g_stVoConf, 0, sizeof(g_stVoConf));
//          // .enMode = AX_VO_MODE_OFFLINE,
//          // .enVoIntfType = AX_VO_INTF_DSI,
//          // .enIntfSync = AX_VO_OUTPUT_USER
//          g_stVoConf = (SAMPLE_VO_CONFIG_S){
//              .u32BindMode    = 0,
//              .u32FifoDepth   = 0,
//              .u32VDevNr      = 1,
//              .stVoDev        = {[0] = {.s32InitFlag   = 0,
//                                        .u32VoDev      = 0,
//                                        .enMode        = AX_VO_MODE_OFFLINE,
//                                        .enVoIntfType  = AX_VO_INTF_DSI,
//                                        .enVoOutfmt    = AX_VO_OUT_FMT_RGB888,
//                                        .setCsc        = AX_FALSE,
//                                        .vo_csc        = {.enCscMatrix  = AX_VO_CSC_MATRIX_IDENTITY,
//                                                          .u32Luma      = 0,
//                                                          .u32Contrast  = 0,
//                                                          .u32Hue       = 0,
//                                                          .u32Satuature = 0},
//                                        .bWbcEn        = AX_FALSE,
//                                        .u32WbcFrmaeNr = 0,
//                                        .stWbcAttr     = {.enSourceType = AX_VO_WBC_SOURCE_DEV,
//                                                          .enMode       = AX_VO_WBC_MODE_NORMAL,
//                                                          .u32FifoDepth = 0,
//                                                          .f32FrameRate = 0},
//                                        .enIntfSync    = AX_VO_OUTPUT_USER,  // AX_VO_OUTPUT_720x1280_60
//                                        AX_VO_OUTPUT_USER .u32SyncIndex  = SAMPLE_VO_SYNC_USER_CUSTUM, .stSyncInfos =
//                                         //    {
//                                         //        .u16Vact = 1280, /* 垂直有效显示区域 */
//                                         //        .u16Vbb  = 16,   /* 垂直后肩 */
//                                         //        .u16Vfb  = 16,   /* 垂直前肩 */
//                                         //        .u16Vpw  = 4,   /* 垂直同步脉冲宽度 */

//                                         //        .u16Hact = 720, /* 水平有效显示区域 */
//                                         //        .u16Hbb  = 40,  /* 水平后肩 */
//                                         //        .u16Hfb  = 40,  /* 水平前肩 */
//                                         //        .u16Hpw  = 4,    /* 水平同步脉冲宽度 */
//                                         //        .u32Pclk = 27000, /* 像素时钟，单位 kHz */
//                                         //     //    .u32Pclk = 54000, /* 像素时钟，单位 kHz */
//                                         //     //    .u32Pclk = 74250, /* 像素时钟，单位 kHz */
//                                         //        .bIdv    = 0,     /* 数据有效信号的极性（根据需求配置） */
//                                         //        .bIhs    = 0,     /* 水平同步信号的极性（根据需求配置） */
//                                         //        .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
//                                         //    },
//                                         {
//                                             .u16Vact = 720, /* 垂直有效显示区域 */
//                                             .u16Vbb  = 22,   /* 垂直后肩 */
//                                             .u16Vfb  = 6,   /* 垂直前肩 */
//                                             .u16Vpw  = 6,   /* 垂直同步脉冲宽度 */

//                                             .u16Hact = 1280, /* 水平有效显示区域 */
//                                             .u16Hbb  = 220,  /* 水平后肩 */
//                                             .u16Hfb  = 1760,  /* 水平前肩 */
//                                             .u16Hpw  = 40,    /* 水平同步脉冲宽度 */
//                                             // .u32Pclk = 27000, /* 像素时钟，单位 kHz */
//                                             .u32Pclk = 37125, /* 像素时钟，单位 kHz */
//                                             // .u32Pclk = 74250, /* 像素时钟，单位 kHz */
//                                             .bIdv    = 1,     /* 数据有效信号的极性（根据需求配置） */
//                                             .bIhs    = 1,     /* 水平同步信号的极性（根据需求配置） */
//                                             .bIvs    = 0      /* 垂直同步信号的极性（根据需求配置） */
//                                         },
//                                      }
//                                  },
//              .u32LayerNr     = 0,
//              .stVoLayer      = {[0] = {.s32InitFlag      = 0,
//                                        .u32VoLayer       = 0,
//                                        .u32LayerPoolId   = 0,
//                                        .bindVoDev        = {[0] = 2, [1] = 2},
//                                        .stVoLayerAttr    = {.stImageSize    = {.u32Width = 1280, .u32Height = 720},
//                                                             .stCompressInfo = {.enCompressMode   =
//                                                             AX_COMPRESS_MODE_NONE,
//                                                                                .u32CompressLevel = 0},
//                                                             .enPixFmt =
//                                                                 AX_FORMAT_YUV420_SEMIPLANAR,  //
//                                                                 AX_FORMAT_YUV420_SEMIPLANAR
//                                                                                               // AX_FORMAT_BGR888
//                                                             .enSyncMode      = AX_VO_LAYER_SYNC_NORMAL,
//                                                             .u32PrimaryChnId = 0,
//                                                             .u32FifoDepth    = 0,
//                                                             .u32BkClr        = 0,
//                                                             .u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO,
//                                                             .enWBMode        = AX_VO_LAYER_WB_POOL,
//                                                             .u32InplaceChnId = 0,
//                                                             .u32PoolId       = 0,
//                                                             .enPartMode      = AX_VO_PART_MODE_SINGLE,
//                                                             .enBlendMode     = AX_VO_BLEND_MODE_DEFAULT,
//                                                             .enEngineMode    = AX_VO_ENGINE_MODE_AUTO,
//                                                             .u32EngineId     = 0,
//                                                             .u32Toleration   = 0,
//                                                             .f32FrmRate      = 30},
//                                        .s32BatchProcFlag = 0,
//                                        .enVoMode         = VO_MODE_1MUX,
//                                        .enChnFrmFmt =
//                                            AX_FORMAT_YUV420_SEMIPLANAR,  // AX_FORMAT_YUV420_SEMIPLANAR
//                                            AX_FORMAT_RGB888
//                                        .u32ChnFrameRate = 0,
//                                        .u32ChnFrameNr   = 0,
//                                        .u32ChnFrameOut  = 0,
//                                        .u32FifoDepth    = 0,
//                                        .u32ChnPoolId    = 0,
//                                        .chnCompressInfo = {.enCompressMode = AX_COMPRESS_MODE_NONE, .u32CompressLevel
//                                        = 0}, .u64KeepChnPrevFrameBitmap0 = 0, .u64KeepChnPrevFrameBitmap1 = 0,
//                                        .u32ChnNr                   = 1,
//                                        .stSrcMod                   = {.enModId = AX_ID_MIN, .s32GrpId = 0, .s32ChnId
//                                        = 0}, .stDstMod                   = {.enModId = AX_ID_MIN, .s32GrpId = 0,
//                                        .s32ChnId = 0}}},
//              .stGraphicLayer = {[0] = {.s32InitFlag = 0,
//                                        .u32FbNum    = 0,
//                                        .stFbConf    = {[0] = {.u32Index       = 0,
//                                                               .u32ResoW       = 0,
//                                                               .u32ResoH       = 0,
//                                                               .u32Fmt         = 0,
//                                                               .u32ColorKeyEn  = 0,
//                                                               .u32ColorKeyInv = 0,
//                                                               .u32ColorKey    = 0}},
//                                        .bindVoDev   = 0}},
//              .stCursorLayer  = {.s32InitFlag      = 0,
//                                 .u32CursorLayerEn = 0,
//                                 .bindVoDev        = 0,
//                                 .u32FBIndex       = 0,
//                                 .u32X             = 0,
//                                 .u32Y             = 0,
//                                 .u32Width         = 0,
//                                 .u32Height        = 0}};

//          // g_stVoConf.stVoDev[0].u32VoDev = 0;
//          // g_stVoConf.stVoDev[1].u32VoDev = 0;
//          // g_stVoConf.stVoDev[0].enVoIntfType = AX_VO_INTF_DSI;
//          // g_stVoConf.stVoDev[1].enVoIntfType = AX_VO_INTF_DSI;

//          // g_stVoConf.stVoDev[0].enIntfSync = AX_VO_OUTPUT_USER;
//          // g_stVoConf.stVoDev[1].enIntfSync = AX_VO_OUTPUT_USER;
//          // g_stVoConf.stVoDev[0].u32SyncIndex = 0;
//          // g_stVoConf.stVoDev[1].u32SyncIndex = 0;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.stImageSize.u32Width = 720;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.stImageSize.u32Width = 720;
//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.stImageSize.u32Height = 1280;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.stImageSize.u32Height = 1280;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.enPixFmt = AX_FORMAT_YUV420_SEMIPLANAR;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.enSyncMode = AX_VO_LAYER_SYNC_NORMAL;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.enSyncMode = AX_VO_LAYER_SYNC_NORMAL;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.u32PrimaryChnId = 0;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.u32PrimaryChnId = 0;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.f32FrmRate = 30;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.f32FrmRate = 30;

//          // g_stVoConf.stVoLayer[0].u32ChnNr = 1;
//          // g_stVoConf.stVoLayer[1].u32ChnNr = 1;

//          // g_stVoConf.stVoLayer[0].enVoMode = VO_MODE_1MUX;
//          // g_stVoConf.stVoLayer[1].enVoMode = VO_MODE_1MUX;

//          // g_stVoConf.stVoLayer[0].stVoLayerAttr.u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO;
//          // g_stVoConf.stVoLayer[1].stVoLayerAttr.u32DispatchMode = AX_VO_LAYER_OUT_TO_FIFO;

//          SAMPLE_VO_LAYER_DISPLAY(&g_stVoConf);
//      }

//      while (!SAMPLE_VO_CheckSig()) {
//          sleep(1);
//      }

//  exit0:

//      AX_SYS_Deinit();

//      SAMPLE_PRT("done, s32Ret = 0x%x\n", s32Ret);

//      return s32Ret;
//  }



#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <mutex>
#include "ax_vo_hal.h"
#include "ax_sernsor_hal.h"
#include "ax_ivps_hal.h"
// #include "sample_engine.h"
#include "ax_venc_hal.h"
#include <atomic>


#include <opencv2/opencv.hpp>
// #include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
// #include <opencv2/videoio.hpp>
// #include <opencv2/video.hpp>

// #include "run_time.h"

#define CALC_FPS(tips)                                                                                           \
    {                                                                                                            \
        static int fcnt = 0;                                                                                     \
        fcnt++;                                                                                                  \
        static struct timespec ts1, ts2;                                                                         \
        clock_gettime(CLOCK_MONOTONIC, &ts2);                                                                    \
        if ((ts2.tv_sec * 1000 + ts2.tv_nsec / 1000000) - (ts1.tv_sec * 1000 + ts1.tv_nsec / 1000000) >= 1000) { \
            printf("%s => H26X FPS:%d\n", tips, fcnt);                                                           \
            ts1  = ts2;                                                                                          \
            fcnt = 0;                                                                                            \
        }                                                                                                        \
    }

extern int gLoopExit;
AX_VOID SAMPLE_VO_SigStop(AX_S32 s32SigNo)
{
    gLoopExit = 0;
}

AX_VIDEO_FRAME_T osd[3];
// int osd_switch = 0;
// std::mutex osd_switch_mtx;
std::atomic_int osd_switch;

ax_ivps_hal ivps;
std::mutex osd_mtx;
// static void ai_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
// {
//     // printf("ai_out_farm ---- \n");
//     AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;

//     // {
//     //     static int count = 0;
//     //     if(count < 120) {
//     //         cv::Mat camera_rgb_img(tVideoFrame->u32Height, tVideoFrame->u32Width, CV_8UC(3), (unsigned char
//     //         *)tVideoFrame->u64VirAddr[0]); std::string image_name = "camera_rgb_img_" + std::to_string(count) +
//     //         ".jpg"; cv::imwrite(image_name, camera_rgb_img); count++;
//     //     }
//     // }

//     SAMPLE_ENGINE_Results stResults;
//     // stResults.obj_count = 0;
//     // printf("tVideoFrame\n");
//     AX_VIDEO_FRAME_T *thisaa = &osd[osd_switch];
//     cv::Mat aamat(720, 1280, CV_8UC(4), (unsigned char *)thisaa->u64VirAddr[0]);
//     memset(aamat.data, 0, 720 * 1280 * 4);
//     int ret = SAMPLE_ENGINE_Inference(tVideoFrame, &stResults, aamat);
//     if (ret != 0) {
//         printf("inference failed, ret=%d.\n", ret);
//     }
//     if (stResults.objects.size() > 0) printf("Detected %d object(s)\n", stResults.objects.size());
//     osd_mtx.lock();
//     osd_switch = osd_switch ? 0 : 1;
//     osd_mtx.unlock();
//     // printf("tVideoFrame over\n");
// }
ax_vo_hal display;
ax_sensor_hal sensor;
// static void vo_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
// {
//     AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;
//     AX_VIDEO_FRAME_T *nihao       = (AX_VIDEO_FRAME_T *)display.get_frame(&display, 0, 0);
//     osd_mtx.lock();
//     int ret =
//         AX_IVPS_AlphaBlendingTdp(tVideoFrame, (osd_switch ? &osd[0] : &osd[1]), (AX_IVPS_POINT_T){0, 0}, 255, &osd[2]);
//     if (ret != 0) {
//         printf("AX_IVPS_AlphaBlendingTdp:%x\n", ret);
//     }
//     osd_mtx.unlock();

//     AX_IVPS_FlipAndRotationTdp(&osd[2], AX_IVPS_CHN_MIRROR, AX_IVPS_ROTATION_90, nihao);

//     // {
//     //     static int count = 0;
//     //     if(count < 120) {
//     //         cv::Mat vo_yuv_img(nihao->u32Height + nihao->u32Height / 2, nihao->u32Width, CV_8UC(1), (unsigned char
//     //         *)nihao->u64VirAddr[0]); cv::Mat vo_rgb_img; cv::cvtColor(vo_yuv_img, vo_rgb_img,
//     //         cv::COLOR_YUV2BGR_I420); std::string image_name = "vo_rgb_img_" + std::to_string(count) + ".jpg";
//     //         cv::imwrite(image_name, vo_rgb_img);
//     //         count++;
//     //     }
//     // }

//     display.put_frame(&display, 0, 0);
//     // CALC_FPS("vo");
// }

static void set_Vo_mode_par_init(struct ax_vo_hal_t *self)
{
    self->stVoConfig.u32VDevNr               = 1;
    self->stVoConfig.stVoDev[0].enMode       = AX_VO_MODE_OFFLINE;
    self->stVoConfig.stVoDev[0].enVoIntfType = AX_VO_INTF_DSI;
    self->stVoConfig.stVoDev[0].enIntfSync   = AX_VO_OUTPUT_USER;
    self->stVoConfig.stVoDev[0].u32SyncIndex = SAMPLE_VO_SYNC_USER_CUSTUM;
    self->stVoConfig.stVoDev[0].enVoOutfmt   = AX_VO_OUT_FMT_RGB888;
    /************************************************************************/
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vact = 1280;     /* 垂直有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vbb  = 16;       /* 垂直后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vfb  = 16;       /* 垂直前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vpw  = 4;        /* 垂直同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hact = 720;      /* 水平有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hbb  = 40;       /* 水平后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hfb  = 40;       /* 水平前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hpw  = 4;        /* 水平同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u32Pclk = 27000;    /* 像素时钟，单位 kHz */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIdv    = AX_FALSE; /* 数据有效信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIhs    = AX_FALSE; /* 水平同步信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIvs    = AX_FALSE; /* 垂直同步信号的极性（根据需求配置） */
    /************************************************************************/
    self->stVoConfig.u32LayerNr                                       = 1;
    self->stVoConfig.stVoLayer[0].bindVoDev[0]                        = 2;
    self->stVoConfig.stVoLayer[0].bindVoDev[1]                        = 2;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.stImageSize.u32Width  = 720;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.stImageSize.u32Height = 1280;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.enPixFmt              = AX_FORMAT_YUV420_SEMIPLANAR;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.u32DispatchMode       = 1;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.f32FrmRate            = 30;
    self->stVoConfig.stVoLayer[0].enVoMode                            = VO_MODE_1MUX;
    self->stVoConfig.stVoLayer[0].enChnFrmFmt                         = AX_FORMAT_YUV420_SEMIPLANAR;
    self->stVoConfig.stVoLayer[0].u32ChnNr                            = 1;
    printf("set config is init------------------------------------\n");
}

static void set_Vo_mode_par_custum(struct ax_vo_hal_t *self)
{
    self->stVoConfig.u32VDevNr               = 1;
    self->stVoConfig.stVoDev[0].enMode       = AX_VO_MODE_OFFLINE;
    self->stVoConfig.stVoDev[0].enVoIntfType = AX_VO_INTF_DSI;
    self->stVoConfig.stVoDev[0].enIntfSync   = AX_VO_OUTPUT_USER;
    self->stVoConfig.stVoDev[0].u32SyncIndex = SAMPLE_VO_SYNC_USER_CUSTUM;
    self->stVoConfig.stVoDev[0].enVoOutfmt   = AX_VO_OUT_FMT_RGB888;
    /************************************************************************/
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vact = 1280;     /* 垂直有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vbb  = 16;       /* 垂直后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vfb  = 16;       /* 垂直前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vpw  = 4;        /* 垂直同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hact = 720;      /* 水平有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hbb  = 40;       /* 水平后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hfb  = 40;       /* 水平前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hpw  = 4;        /* 水平同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u32Pclk = 54000;    /* 像素时钟，单位 kHz */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIdv    = AX_FALSE; /* 数据有效信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIhs    = AX_FALSE; /* 水平同步信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIvs    = AX_FALSE; /* 垂直同步信号的极性（根据需求配置） */
    /************************************************************************/
    self->stVoConfig.u32LayerNr                                       = 1;
    self->stVoConfig.stVoLayer[0].bindVoDev[0]                        = 2;
    self->stVoConfig.stVoLayer[0].bindVoDev[1]                        = 2;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.stImageSize.u32Width  = 720;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.stImageSize.u32Height = 1280;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.enPixFmt              = AX_FORMAT_YUV420_SEMIPLANAR;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.u32DispatchMode       = 1;
    self->stVoConfig.stVoLayer[0].stVoLayerAttr.f32FrmRate            = 30;
    self->stVoConfig.stVoLayer[0].enVoMode                            = VO_MODE_1MUX;
    self->stVoConfig.stVoLayer[0].enChnFrmFmt                         = AX_FORMAT_YUV420_SEMIPLANAR;
    self->stVoConfig.stVoLayer[0].u32ChnNr                            = 1;
    printf("set config is custum------------------------------------\n");
}

// int main(int argc, char *argv[])
// {
//     signal(SIGPIPE, SIG_IGN);
//     signal(SIGINT, SAMPLE_VO_SigStop);
//     signal(SIGTSTP, SAMPLE_VO_SigStop);
//     signal(SIGQUIT, SAMPLE_VO_SigStop);
//     // COMMON_SYS_ARGS_T tCommonArgs = {0};
//     ax_sensor_hal sensor;
//     ax_create_sensor(&sensor);
//     ax_create_ivps(&ivps);
//     ax_create_vo(&display);
//     display.set_Vo_mode_par[HAL_AX_VO_PAR_CUSTOM]     = set_Vo_mode_par_custum;
//     display.set_Vo_mode_par[HAL_AX_VO_PAR_CUSTOM + 1] = set_Vo_mode_par_init;
//     // sensor.InitSensor(&sensor, SAMPLE_VIN_SINGLE_OS04A10, 1);
//     sensor.InitSensor(&sensor, SAMPLE_VIN_SINGLE_SC850SL, 1);
//     COMMON_SYS_Init(&sensor.tCommonArgs);
//     COMMON_NPU_Init();

//     osd[0].enImgFormat     = AX_FORMAT_ARGB8888;
//     osd[0].u32Width        = 1280;
//     osd[0].u32Height       = 720;
//     osd[0].u32PicStride[0] = ALIGN_UP(osd[0].u32Width, 16);
//     int size               = osd[0].u32PicStride[0] * osd[0].u32Height * 4;
//     AX_SYS_MemAlloc(&osd[0].u64PhyAddr[0], (AX_VOID **)&osd[0].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

//     // osd[0];
//     osd[1].enImgFormat     = AX_FORMAT_ARGB8888;
//     osd[1].u32Width        = 1280;
//     osd[1].u32Height       = 720;
//     osd[1].u32PicStride[0] = ALIGN_UP(osd[1].u32Width, 16);
//     // osd[0].u64PhyAddr[0]
//     // osd[0].u64VirAddr[0]
//     size = osd[1].u32PicStride[0] * osd[1].u32Height * 4;
//     AX_SYS_MemAlloc(&osd[1].u64PhyAddr[0], (AX_VOID **)&osd[1].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

//     osd[2].enImgFormat     = AX_FORMAT_YUV420_SEMIPLANAR;
//     osd[2].u32Width        = 1280;
//     osd[2].u32Height       = 720;
//     osd[2].u32PicStride[0] = ALIGN_UP(osd[1].u32Width, 16);
//     // osd[0].u64PhyAddr[0]
//     // osd[0].u64VirAddr[0]
//     size = osd[2].u32PicStride[0] * osd[2].u32Height * 3 / 2;
//     AX_SYS_MemAlloc(&osd[2].u64PhyAddr[0], (AX_VOID **)&osd[2].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

//     SAMPLE_ENGINE_Load((AX_CHAR *)SAMPLE_ENGINE_MODEL_FILE);
//     hal_AX_SYS_Link(sensor.get_chn_pipe_id(&sensor, HAL_AX_SENSOR_DEV_0, HAL_AX_SENSOR_CHN_0),
//                     ivps.get_chn_pipe_id(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_0));

//     if (access("/tmp/axera_vo_mipi_lcd_init.lock", F_OK) != 0) {
//         // 初始化屏幕模块
//         display.InitVo(&display, HAL_AX_VO_DEV_0, HAL_AX_VO_PAR_CUSTOM + 1);
//         display.OpenVo(&display, HAL_AX_VO_DEV_0);
//         display.CloseVo(&display, HAL_AX_VO_DEV_0);
//         system("touch /tmp/axera_vo_mipi_lcd_init.lock");
//     }

//     // 二次初始化屏幕模块
//     display.InitVo(&display, HAL_AX_VO_DEV_0, HAL_AX_VO_PAR_CUSTOM);
//     display.OpenVo(&display, HAL_AX_VO_DEV_0);

//     ivps.InitGRP(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_PAR_0);
//     ivps.OpenGRP(&ivps, HAL_AX_IVPS_DEV_1);
//     ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1, ai_out_farm, NULL);
//     ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2, vo_out_farm, NULL);
//     ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1);
//     ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2);
//     sensor.OpenSensor(&sensor);
//     // while (!gLoopExit) {
//     //     sleep(1);
//     // }
//     pause();

//     hal_AX_SYS_UnLink(sensor.get_chn_pipe_id(&sensor, HAL_AX_SENSOR_DEV_0, HAL_AX_SENSOR_CHN_0),
//                       ivps.get_chn_pipe_id(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_0));

//     // printf("ivps.CloseGRP(&ivps, 1)\n");
//     ivps.CloseGRP(&ivps, HAL_AX_IVPS_DEV_1);
//     // printf("sensor.CloseSensor(&sensor)\n");
//     sensor.CloseSensor(&sensor);
//     // printf("display.CloseVo(&display, 0)\n");
//     display.CloseVo(&display, HAL_AX_VO_DEV_0);
//     SAMPLE_ENGINE_Release();
//     AX_ENGINE_Deinit();
//     AX_SYS_MemFree(osd[0].u64PhyAddr[0], (AX_VOID *)osd[0].u64VirAddr[0]);
//     AX_SYS_MemFree(osd[1].u64PhyAddr[0], (AX_VOID *)osd[1].u64VirAddr[0]);
//     AX_SYS_MemFree(osd[2].u64PhyAddr[0], (AX_VOID *)osd[2].u64VirAddr[0]);

//     COMMON_SYS_DeInit();
//     return 0;
// }

























#include "lvgl/lvgl.h"
#include <stdio.h>


extern "C"{
    void axeradev_init();
    void axeradev_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p);
    void axera_vivo_exit();
};



COMMON_SYS_ARGS_T tCommonArgs = {0};

cv::Mat dispplay_image;


#include <thread>
std::atomic_bool display_run;
std::unique_ptr<std::thread> display_vo_th;

void display_th();

void axeradev_init()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SAMPLE_VO_SigStop);
    signal(SIGTSTP, SAMPLE_VO_SigStop);
    signal(SIGQUIT, SAMPLE_VO_SigStop);
    ax_create_sensor(&sensor);
    ax_create_vo(&display);
    display.set_Vo_mode_par[HAL_AX_VO_PAR_CUSTOM]     = set_Vo_mode_par_custum;
    display.set_Vo_mode_par[HAL_AX_VO_PAR_CUSTOM + 1] = set_Vo_mode_par_init;
    sensor.InitSensor(&sensor, SAMPLE_VIN_SINGLE_SC850SL, 1);
    COMMON_SYS_Init(&sensor.tCommonArgs);    
    // COMMON_SYS_Init(&tCommonArgs);
    // COMMON_NPU_Init();
    AX_IVPS_Init();
    osd[0].enImgFormat     = AX_FORMAT_ABGR8888;
    osd[0].u32Width        = 1280;
    osd[0].u32Height       = 720;
    osd[0].u32PicStride[0] = ALIGN_UP(osd[0].u32Width, 16);
    int size               = osd[0].u32PicStride[0] * osd[0].u32Height * 4;
    AX_SYS_MemAlloc(&osd[0].u64PhyAddr[0], (AX_VOID **)&osd[0].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");
    dispplay_image = cv::Mat(osd[0].u32Height, osd[0].u32Width, CV_8UC4, (void *)osd[0].u64VirAddr[0]);
    // osd[0];
    osd[1].enImgFormat     = AX_FORMAT_RGBA8888;
    osd[1].u32Width        = 1280;
    osd[1].u32Height       = 720;
    osd[1].u32PicStride[0] = ALIGN_UP(osd[1].u32Width, 16);
    // osd[0].u64PhyAddr[0]
    // osd[0].u64VirAddr[0]
    size = osd[1].u32PicStride[0] * osd[1].u32Height * 4;
    AX_SYS_MemAlloc(&osd[1].u64PhyAddr[0], (AX_VOID **)&osd[1].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

    osd[2].enImgFormat     = AX_FORMAT_YUV420_SEMIPLANAR;
    osd[2].u32Width        = 1280;
    osd[2].u32Height       = 720;
    osd[2].u32PicStride[0] = ALIGN_UP(osd[1].u32Width, 16);
    // osd[0].u64PhyAddr[0]
    // osd[0].u64VirAddr[0]
    size = osd[2].u32PicStride[0] * osd[2].u32Height * 3 / 2;
    AX_SYS_MemAlloc(&osd[2].u64PhyAddr[0], (AX_VOID **)&osd[2].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

    // SAMPLE_ENGINE_Load((AX_CHAR *)SAMPLE_ENGINE_MODEL_FILE);
    // hal_AX_SYS_Link(sensor.get_chn_pipe_id(&sensor, HAL_AX_SENSOR_DEV_0, HAL_AX_SENSOR_CHN_0),
    //                 ivps.get_chn_pipe_id(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_0));

    if (access("/tmp/axera_vo_mipi_lcd_init.lock", F_OK) != 0) {
        // 初始化屏幕模块
        display.InitVo(&display, HAL_AX_VO_DEV_0, HAL_AX_VO_PAR_CUSTOM + 1);
        display.OpenVo(&display, HAL_AX_VO_DEV_0);
        display.CloseVo(&display, HAL_AX_VO_DEV_0);
        system("touch /tmp/axera_vo_mipi_lcd_init.lock");
    }

    // 二次初始化屏幕模块
    display.InitVo(&display, HAL_AX_VO_DEV_0, HAL_AX_VO_PAR_CUSTOM);
    display.OpenVo(&display, HAL_AX_VO_DEV_0);

    // ivps.InitGRP(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_PAR_0);
    // ivps.OpenGRP(&ivps, HAL_AX_IVPS_DEV_1);
    // ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1, ai_out_farm, NULL);
    // ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2, vo_out_farm, NULL);
    // ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1);
    // ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2);
    // sensor.OpenSensor(&sensor);

    display_run = true;
    display_vo_th = std::make_unique<std::thread>(display_th);
}
void axera_vivo_exit()
{
    display_run = false;
    display_vo_th->join();
    display.CloseVo(&display, HAL_AX_VO_DEV_0);
    AX_SYS_MemFree(osd[0].u64PhyAddr[0], (AX_VOID *)osd[0].u64VirAddr[0]);
    AX_SYS_MemFree(osd[1].u64PhyAddr[0], (AX_VOID *)osd[1].u64VirAddr[0]);
    AX_SYS_MemFree(osd[2].u64PhyAddr[0], (AX_VOID *)osd[2].u64VirAddr[0]);
    AX_IVPS_Deinit();
    COMMON_SYS_DeInit();
}

std::mutex ui_info_mtx;
void display_th()
{
    while (display_run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        {
            std::lock_guard<std::mutex> lock(ui_info_mtx);
            AX_VIDEO_FRAME_T *nihao = (AX_VIDEO_FRAME_T *)display.get_frame(&display, 0, 0);
            if (nihao) {
                int ret = AX_IVPS_CscTdp(&osd[0], &osd[2]);
                if (ret == 0) {
                    AX_IVPS_FlipAndRotationTdp(&osd[2], AX_IVPS_CHN_FLIP_NONE, AX_IVPS_ROTATION_90, nihao);
                }
                display.put_frame(&display, 0, 0);
            }
        }
    }
}

void axeradev_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p)
{
    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2;
    int32_t act_y2 = area->y2;
    
    lv_coord_t w               = (act_x2 - act_x1 + 1);
    lv_coord_t h               = (act_y2 - act_y1 + 1);
    long int location          = 0;
    long int byte_location     = 0;
    unsigned char bit_location = 0;
    // printf("act_x1:%d act_y1:%d act_x2:%d act_y2:%d, w:%d\n", act_x1, act_y1, act_x2, act_y2, w);

    {
        std::lock_guard<std::mutex> lock(ui_info_mtx);
        uint8_t *srcpix = (uint8_t *)color_p;
        uint8_t *dstpix = (uint8_t *)dispplay_image.data;
        for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            dstpix = (uint8_t *)dispplay_image.data + (y + act_y1) * dispplay_image.step[0] + (x + act_x1) * dispplay_image.elemSize();
            dstpix[0] = srcpix[2];
            dstpix[1] = srcpix[1];
            dstpix[2] = srcpix[0];
            dstpix[3] = srcpix[3];
            srcpix += 4;
        }
    }

    
    

    // cv::Mat input_image(h, w, CV_8UC4, (void *)color_p);

    // // cv::Vec4b pixel4;
    // // pixel4 = dispplay_image.at<cv::Vec4b>(0, 0);std::cout << "(0,0):" << pixel4 << std::endl;
    // for (size_t y = act_y1; y < act_y2 + 1; y++)
    //     for (size_t x = act_x1; x < act_x2 + 1; x++)
    //         {
    //             cv::Vec4b &pixel4s = input_image.at<cv::Vec4b>(y - act_y1, x - act_x1);
    //             cv::Vec4b &pixel4d = dispplay_image.at<cv::Vec4b>(y, x);
    //             pixel4d[0] = pixel4s[2];
    //             pixel4d[1] = pixel4s[1];
    //             pixel4d[2] = pixel4s[0];
    //             pixel4d[3] = pixel4s[3];
    //         }
    


    // input_image.copyTo(dispplay_image(cv::Rect(act_x1, act_y1, w, act_y2 - act_y1 + 1)));

    // ARGB

    // AX_VIDEO_FRAME_T *nihao       = (AX_VIDEO_FRAME_T *)display.get_frame(&display, 0, 0);
    // if(nihao != NULL)
    // {
    //     memcpy((void *)nihao->u64VirAddr[0], (void *)color_p, 1280 * 720);
    //     display.put_frame(&display, 0, 0);
    // }




    // // AX_VIDEO_FRAME_T ptSrc = osd[2];
    // AX_VIDEO_FRAME_T ptOverlay = {0};
    // // AX_VIDEO_FRAME_T ptDst = {0};
    // ptOverlay.u32Width  = 1280;
    // ptOverlay.u32Height = 720;
    // ptOverlay.enImgFormat     = AX_FORMAT_ARGB8888;
    // ptOverlay.u32PicStride[0] = ALIGN_UP(ptOverlay.u32Width, 16);
    // // osd[0].u64PhyAddr[0]
    // // osd[0].u64VirAddr[0]
    // int size = ptOverlay.u32PicStride[0] * ptOverlay.u32Height * 4;
    // AX_SYS_MemAlloc(&ptOverlay.u64PhyAddr[0], (AX_VOID **)&ptOverlay.u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");
    // memcpy((void *)ptOverlay.u64VirAddr[0], (void *)dispplay_image.data, 1280 * 720 * 4);


    // AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;
    // AX_VIDEO_FRAME_T *nihao       = (AX_VIDEO_FRAME_T *)display.get_frame(&display, 0, 0);
    // if(nihao)
    // {
    //     int ret = AX_IVPS_CscTdp(&osd[0], &osd[2]);if (ret == 0) {
    //         AX_IVPS_FlipAndRotationTdp(&osd[2], AX_IVPS_CHN_FLIP_NONE, AX_IVPS_ROTATION_90, nihao);
    //     }
    //     display.put_frame(&display, 0, 0);
    // }

    // osd_mtx.lock();
    // int ret =
        // AX_IVPS_AlphaBlendingTdp(&osd[2], &osd[0], (AX_IVPS_POINT_T){0, 0}, 255, &osd[2]);
    // if (ret != 0) {
    //     printf("AX_IVPS_AlphaBlendingTdp:%x\n", ret);
    // }
    // osd_mtx.unlock();

    // AX_SYS_MemFree(ptOverlay.u64PhyAddr[0], (AX_VOID *)ptOverlay.u64VirAddr[0]);
    // {
    //     static int count = 0;
    //     if(count < 120) {
    //         cv::Mat vo_yuv_img(nihao->u32Height + nihao->u32Height / 2, nihao->u32Width, CV_8UC(1), (unsigned char
    //         *)nihao->u64VirAddr[0]); cv::Mat vo_rgb_img; cv::cvtColor(vo_yuv_img, vo_rgb_img,
    //         cv::COLOR_YUV2BGR_I420); std::string image_name = "vo_rgb_img_" + std::to_string(count) + ".jpg";
    //         cv::imwrite(image_name, vo_rgb_img);
    //         count++;
    //     }
    // }
    // memset((void*)nihao->u64VirAddr[0], 0 , 1280 * 720 * 3 / 2);
    // cv::Mat output_img(1280 + 1280 / 2, 720, CV_8UC1, (void *)nihao->u64VirAddr[0]);
    // cv::Mat rgbImage;


    // cv::Vec4b pixel4;
    // pixel4 = dispplay_image.at<cv::Vec4b>(0, 0);std::cout << "(0,0):" << pixel4 << std::endl;
    // pixel4 = dispplay_image.at<cv::Vec4b>(1, 1);std::cout << "(1,1):" << pixel4 << std::endl;
    // pixel4 = dispplay_image.at<cv::Vec4b>(2, 2);std::cout << "(2,2):" << pixel4 << std::endl;




    // cv::cvtColor(dispplay_image, rgbImage, cv::COLOR_RGBA2RGB);

    // cv::Vec3b pixel3;

    // pixel3 = rgbImage.at<cv::Vec3b>(0, 0);std::cout << "(0,0):" << pixel3 << std::endl;
    // pixel3 = rgbImage.at<cv::Vec3b>(1, 1);std::cout << "(1,1):" << pixel3 << std::endl;
    // pixel3 = rgbImage.at<cv::Vec3b>(2, 2);std::cout << "(2,2):" << pixel3 << std::endl;

    // cv::Mat rotatedImage;
    // cv::rotate(rgbImage, rotatedImage, cv::ROTATE_90_CLOCKWISE);

    // // pixel3 = rotatedImage.at<cv::Vec3b>(0, 0);std::cout << "(0,0):" << pixel3 << std::endl;
    // // pixel3 = rotatedImage.at<cv::Vec3b>(1, 1);std::cout << "(1,1):" << pixel3 << std::endl;
    // // pixel3 = rotatedImage.at<cv::Vec3b>(2, 2);std::cout << "(2,2):" << pixel3 << std::endl;

    // // {
    // //     static int count = 0;
    // //     if (count < 10) {
    // //         std::string image_name = "vo_rgb_img_" + std::to_string(count) + ".jpg";
    // //         cv::imwrite(image_name, rgbImage);
    // //         count++;
    // //     }
    // // }

    // // cv::cvtColor(rgbImage, output_img, cv::COLOR_BGR2YUV);
    // // convertBGRToNV12(rgbImage, output_img);

    // unsigned char *yPlane = (unsigned char*)nihao->u64VirAddr[0];
    // unsigned char* uvPlane = yPlane + 1280 * 720;
    // for (int j = 0; j < 1280; j++) {
    //     for (int i = 0; i < 720; i++) {
    //         cv::Vec3b bgrPixel = rotatedImage.at<cv::Vec3b>(j, i);
    //         unsigned char B = bgrPixel[0];
    //         unsigned char G = bgrPixel[1];
    //         unsigned char R = bgrPixel[2];

    //         // Calculate Y component
    //         unsigned char Y = static_cast<unsigned char>(0.299 * R + 0.587 * G + 0.114 * B);
    //         yPlane[j * 720 + i] = Y;

    //         // Calculate U and V components for every pair of rows
    //         if (j % 2 == 0 && i % 2 == 0) {
    //             unsigned char U = static_cast<unsigned char>(-0.14713 * R - 0.28886 * G + 0.436 * B + 128);
    //             unsigned char V = static_cast<unsigned char>(0.615 * R - 0.51499 * G - 0.10001 * B + 128);

    //             // Store U and V in the interleaved UV plane
    //             int uvIndex = (j / 2) * 720 + i;
    //             uvPlane[uvIndex] = U;
    //             uvPlane[uvIndex + 1] = V;
    //         }
    //     }
    // }



    




















    lv_disp_flush_ready(drv);
}