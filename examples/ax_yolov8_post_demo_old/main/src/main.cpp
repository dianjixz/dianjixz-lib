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
#include "sample_engine.h"
#include "ax_venc_hal.h"
#include <atomic>

#include "run_time.h"

#define CALC_FPS(tips)                                                                                     \
  {                                                                                                        \
    static int fcnt = 0;                                                                                   \
    fcnt++;                                                                                                \
    static struct timespec ts1, ts2;                                                                       \
    clock_gettime(CLOCK_MONOTONIC, &ts2);                                                                  \
    if ((ts2.tv_sec * 1000 + ts2.tv_nsec / 1000000) - (ts1.tv_sec * 1000 + ts1.tv_nsec / 1000000) >= 1000) \
    {                                                                                                      \
      printf("%s => H26X FPS:%d\n", tips, fcnt);                                                  \
      ts1 = ts2;                                                                                           \
      fcnt = 0;                                                                                            \
    }                                                                                                      \
  }



int gLoopExit = 0;
AX_VOID SAMPLE_VO_SigStop(AX_S32 s32SigNo)
{
    gLoopExit = 1;
}

AX_VIDEO_FRAME_T osd[3];
// int osd_switch = 0;
// std::mutex osd_switch_mtx;
std::atomic_int osd_switch;

ax_ivps_hal ivps;
std::mutex osd_mtx;
static void ai_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
{
    // printf("ai_out_farm ---- \n");
    AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;

    // {
    //     static int count = 0;
    //     if(count < 120) {
    //         cv::Mat camera_rgb_img(tVideoFrame->u32Height, tVideoFrame->u32Width, CV_8UC(3), (unsigned char *)tVideoFrame->u64VirAddr[0]);
    //         std::string image_name = "camera_rgb_img_" + std::to_string(count) + ".jpg";
    //         cv::imwrite(image_name, camera_rgb_img);
    //         count++;
    //     }
    // }

    SAMPLE_ENGINE_Results stResults;
    // stResults.obj_count = 0;
    // printf("tVideoFrame\n");
    AX_VIDEO_FRAME_T *thisaa = &osd[osd_switch];
    cv::Mat aamat(720, 1280, CV_8UC(4), (unsigned char *)thisaa->u64VirAddr[0]);
    memset(aamat.data, 0, 720 * 1280 * 4);
    int ret = SAMPLE_ENGINE_Inference(tVideoFrame, &stResults, aamat);
    if (ret != 0) {
        printf("inference failed, ret=%d.\n", ret);
    }
    if (stResults.objects.size() > 0) printf("Detected %d object(s)\n", stResults.objects.size());
    osd_mtx.lock();
    osd_switch = osd_switch ? 0 : 1;
    osd_mtx.unlock();
    // printf("tVideoFrame over\n");
}
ax_vo_hal display;
static void vo_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
{
    
    AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;
    AX_VIDEO_FRAME_T *nihao                   = (AX_VIDEO_FRAME_T *)display.get_frame(&display, 0, 0);
    osd_mtx.lock();
    int ret =
        AX_IVPS_AlphaBlendingTdp(tVideoFrame, (osd_switch ? &osd[0] : &osd[1]), (AX_IVPS_POINT_T){0, 0}, 255, &osd[2]);
        if (ret != 0) {
        printf("AX_IVPS_AlphaBlendingTdp:%x\n", ret);
    }
    osd_mtx.unlock();
    
    AX_IVPS_FlipAndRotationTdp(&osd[2], AX_IVPS_CHN_MIRROR, AX_IVPS_ROTATION_90, nihao);
    
    // {
    //     static int count = 0;
    //     if(count < 120) {
    //         cv::Mat vo_yuv_img(nihao->u32Height + nihao->u32Height / 2, nihao->u32Width, CV_8UC(1), (unsigned char *)nihao->u64VirAddr[0]);
    //         cv::Mat vo_rgb_img;
    //         cv::cvtColor(vo_yuv_img, vo_rgb_img, cv::COLOR_YUV2BGR_I420);
    //         std::string image_name = "vo_rgb_img_" + std::to_string(count) + ".jpg";
    //         cv::imwrite(image_name, vo_rgb_img);
    //         count++;
    //     }
    // }
    
    display.put_frame(&display, 0, 0);
    // CALC_FPS("vo");
    
}

static void set_Vo_mode_par_custum(struct ax_vo_hal_t *self)
{
    self->stVoConfig.u32VDevNr                                        = 1;
    self->stVoConfig.stVoDev[0].enMode                                = AX_VO_MODE_OFFLINE;
    self->stVoConfig.stVoDev[0].enVoIntfType                          = AX_VO_INTF_DSI;
    self->stVoConfig.stVoDev[0].enIntfSync                            = AX_VO_OUTPUT_USER;
    self->stVoConfig.stVoDev[0].u32SyncIndex                          = SAMPLE_VO_SYNC_USER_CUSTUM;
    self->stVoConfig.stVoDev[0].enVoOutfmt                            = AX_VO_OUT_FMT_RGB888;
    /************************************************************************/
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vact                   = 1280;  /* 垂直有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vbb                    = 16;    /* 垂直后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vfb                    = 16;    /* 垂直前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Vpw                    = 4;     /* 垂直同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hact                   = 720;   /* 水平有效显示区域 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hbb                    = 40;    /* 水平后肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hfb                    = 40;    /* 水平前肩 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u16Hpw                    = 4;     /* 水平同步脉冲宽度 */
    self->stVoConfig.stVoDev[0].stSyncInfos.u32Pclk                   = 54000; /* 像素时钟，单位 kHz */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIdv                      = AX_FALSE;     /* 数据有效信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIhs                      = AX_FALSE;     /* 水平同步信号的极性（根据需求配置） */
    self->stVoConfig.stVoDev[0].stSyncInfos.bIvs                      = AX_FALSE;     /* 垂直同步信号的极性（根据需求配置） */
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

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SAMPLE_VO_SigStop);
    signal(SIGTSTP, SAMPLE_VO_SigStop);
    signal(SIGQUIT, SAMPLE_VO_SigStop);
    // COMMON_SYS_ARGS_T tCommonArgs = {0};
    ax_sensor_hal sensor;
    ax_create_sensor(&sensor);
    ax_create_ivps(&ivps);
    ax_create_vo(&display);
    display.set_Vo_mode_par[HAL_AX_VO_PAR_CUSTOM] = set_Vo_mode_par_custum;
    // sensor.InitSensor(&sensor, SAMPLE_VIN_SINGLE_OS04A10, 1);
    sensor.InitSensor(&sensor, SAMPLE_VIN_SINGLE_SC850SL, 1);
    COMMON_SYS_Init(&sensor.tCommonArgs);
    COMMON_NPU_Init();

    osd[0].enImgFormat     = AX_FORMAT_ARGB8888;
    osd[0].u32Width        = 1280;
    osd[0].u32Height       = 720;
    osd[0].u32PicStride[0] = ALIGN_UP(osd[0].u32Width, 16);
    int size               = osd[0].u32PicStride[0] * osd[0].u32Height * 4;
    AX_SYS_MemAlloc(&osd[0].u64PhyAddr[0], (AX_VOID **)&osd[0].u64VirAddr[0], size, 0x100, (AX_S8 *)"MY_OSD");

    // osd[0];
    osd[1].enImgFormat     = AX_FORMAT_ARGB8888;
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

    SAMPLE_ENGINE_Load((AX_CHAR *)SAMPLE_ENGINE_MODEL_FILE);
    hal_AX_SYS_Link(sensor.get_chn_pipe_id(&sensor, HAL_AX_SENSOR_DEV_0, HAL_AX_SENSOR_CHN_0), 
        ivps.get_chn_pipe_id(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_0));
    display.InitVo(&display, HAL_AX_VO_DEV_0, HAL_AX_VO_PAR_CUSTOM);
    display.OpenVo(&display, HAL_AX_VO_DEV_0);
    ivps.InitGRP(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_PAR_0);
    ivps.OpenGRP(&ivps, HAL_AX_IVPS_DEV_1);
    ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1, ai_out_farm, NULL);
    ivps.set_chn_farm_on(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2, vo_out_farm, NULL);
    ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_1);
    ivps.start(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_2);
    sensor.OpenSensor(&sensor);
    // while (!gLoopExit) {
    //     sleep(1);
    // }
    pause();

    hal_AX_SYS_UnLink(sensor.get_chn_pipe_id(&sensor, HAL_AX_SENSOR_DEV_0, HAL_AX_SENSOR_CHN_0), 
        ivps.get_chn_pipe_id(&ivps, HAL_AX_IVPS_DEV_1, HAL_AX_IVPS_CHN_0));

    // printf("ivps.CloseGRP(&ivps, 1)\n");
    ivps.CloseGRP(&ivps, HAL_AX_IVPS_DEV_1);
    // printf("sensor.CloseSensor(&sensor)\n");
    sensor.CloseSensor(&sensor);
    // printf("display.CloseVo(&display, 0)\n");
    display.CloseVo(&display, HAL_AX_VO_DEV_0);
    SAMPLE_ENGINE_Release();
    AX_ENGINE_Deinit();
    AX_SYS_MemFree(osd[0].u64PhyAddr[0], (AX_VOID *)osd[0].u64VirAddr[0]);
    AX_SYS_MemFree(osd[1].u64PhyAddr[0], (AX_VOID *)osd[1].u64VirAddr[0]);
    AX_SYS_MemFree(osd[2].u64PhyAddr[0], (AX_VOID *)osd[2].u64VirAddr[0]);

    COMMON_SYS_DeInit();
    return 0;
}