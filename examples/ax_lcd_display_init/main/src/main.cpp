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
#include "ax_venc_hal.h"
#include <atomic>

int gLoopExit = 0;
AX_VOID SAMPLE_VO_SigStop(AX_S32 s32SigNo)
{
    gLoopExit = 1;
}
ax_vo_hal display;

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SAMPLE_VO_SigStop);
    signal(SIGTSTP, SAMPLE_VO_SigStop);
    signal(SIGQUIT, SAMPLE_VO_SigStop);
    AX_SYS_Init();
    AX_POOL_Exit();
    // 第一次初始化
    ax_create_vo(&display);
    display.InitVo(&display, 0, 0);
    display.OpenVo(&display, 0);
    display.CloseVo(&display, 0);
    ax_destroy_vo(&display);
    AX_SYS_Deinit();
    return 0;
}