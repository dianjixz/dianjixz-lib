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

static void ai_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
{
    // printf("ai_out_farm ---- \n");
    AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;
    SAMPLE_ENGINE_Results stResults;
    // stResults.obj_count = 0;
    printf("tVideoFrame\n");
    AX_VIDEO_FRAME_T *thisaa = &osd[osd_switch];
    cv::Mat aamat(720, 1280, CV_8UC(4), (unsigned char *)thisaa->u64VirAddr[0]);
    memset(aamat.data, 0, 720 * 1280 * 4);
    int ret = SAMPLE_ENGINE_Inference(tVideoFrame, &stResults, aamat);
    if (ret != 0) {
        printf("inference failed, ret=%d.\n", ret);
    }
    if (stResults.objects.size() > 0) printf("Detected %d object(s)\n", stResults.objects.size());

    osd_switch = osd_switch ? 0 : 1;
    // printf("tVideoFrame over\n");
}

ax_vo_hal display;
static void vo_out_farm(AX_VIDEO_FRAME_T *fram, void *ctx)
{
    AX_VIDEO_FRAME_T *tVideoFrame = (AX_VIDEO_FRAME_T *)fram;
    void *nihao                   = display.get_frame(&display, 0, 0);
    int ret =
        AX_IVPS_AlphaBlendingTdp(tVideoFrame, (osd_switch ? &osd[0] : &osd[1]), (AX_IVPS_POINT_T){0, 0}, 255, &osd[2]);
    if (ret != 0) {
        printf("AX_IVPS_AlphaBlendingTdp:%x\n", ret);
    }
    AX_IVPS_FlipAndRotationTdp(&osd[2], AX_IVPS_CHN_MIRROR, AX_IVPS_ROTATION_90, (AX_VIDEO_FRAME_T *)nihao);
    display.put_frame(&display, 0, 0);
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
    ivps.superior_link(&ivps, 1, sensor.get_link_mod(&sensor, 0));
    display.InitVo(&display, 0, 0);
    display.OpenVo(&display, 0);
    ivps.InitGRP(&ivps, 1, 0);
    ivps.OpenGRP(&ivps, 1);
    ivps.set_chn_farm_on(&ivps, 1, 1, ai_out_farm, NULL);
    ivps.set_chn_farm_on(&ivps, 1, 2, vo_out_farm, NULL);
    ivps.start(&ivps, 1, 1);
    ivps.start(&ivps, 1, 2);
    sensor.OpenSensor(&sensor);
    while (!gLoopExit) {
        sleep(1);
    }
    // printf("ivps.CloseGRP(&ivps, 1)\n");
    ivps.CloseGRP(&ivps, 1);
    // printf("sensor.CloseSensor(&sensor)\n");
    sensor.CloseSensor(&sensor);
    // printf("display.CloseVo(&display, 0)\n");
    display.CloseVo(&display, 0);
    SAMPLE_ENGINE_Release();
    AX_ENGINE_Deinit();
    AX_SYS_MemFree(osd[0].u64PhyAddr[0], (AX_VOID *)osd[0].u64VirAddr[0]);
    AX_SYS_MemFree(osd[1].u64PhyAddr[0], (AX_VOID *)osd[1].u64VirAddr[0]);
    AX_SYS_MemFree(osd[2].u64PhyAddr[0], (AX_VOID *)osd[2].u64VirAddr[0]);

    COMMON_SYS_DeInit();
    return 0;
}