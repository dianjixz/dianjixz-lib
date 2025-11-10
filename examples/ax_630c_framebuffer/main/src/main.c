#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "ax_sys_api.h"
#include "ax_base_type.h"
#include "ax_vo_api.h"

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt,...) \
do{ \
    printf("[SAMPLE-VO][%s-%d] "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
}while (0)

#endif

static AX_S32 SAMPLE_VO_FB_INIT(AX_U32 u32Width, AX_U32 u32Height, AX_U32 u32FbIndex)
{
    AX_S32 s32Fd, s32Ret = 0;
    AX_CHAR fbPath[32];
    struct fb_var_screeninfo stVar;
    struct fb_bitfield stR32 = {24, 8, 0};
    struct fb_bitfield stG32 = {16, 8, 0};
    struct fb_bitfield stB32 = {8, 8, 0};
    struct fb_bitfield stA32 = {0, 8, 0};

    /* 1.Open framebuffer device */
    snprintf(fbPath, sizeof(fbPath), "/dev/fb%d", u32FbIndex);
    s32Fd = open(fbPath, O_RDWR);
    if (s32Fd < 0) {
        SAMPLE_PRT("open %s failed, err:%s\n", fbPath, strerror(errno));
        return s32Fd;
    }

    /* 2.Get the variable screen info */
    s32Ret = ioctl(s32Fd, FBIOGET_VSCREENINFO, &stVar);
    if (s32Ret < 0) {
        SAMPLE_PRT("get variable screen info from fb%d failed\n", u32FbIndex);
        goto exit;
    }

    /* 3.Modify the variable screen info, the screen size: u32Width*u32Height, 
       the virtual screen size: u32Width*(u32Height*2), the pixel format: ARGB8888 */
    stVar.xres = stVar.xres_virtual = u32Width;
    stVar.yres = u32Height;
    stVar.yres_virtual = u32Height * 2;
    stVar.transp = stA32;
    stVar.red = stR32;
    stVar.green = stG32;
    stVar.blue = stB32;
    stVar.bits_per_pixel = 32;

    /* 4.Set the variable screeninfo */
    s32Ret = ioctl(s32Fd, FBIOPUT_VSCREENINFO, &stVar);
    if (s32Ret < 0) {
        SAMPLE_PRT("put variable screen info to fb%d failed\n", u32FbIndex);
        goto exit;
    }

    SAMPLE_PRT("init fb%d done\n", u32FbIndex);

exit:
    close(s32Fd);
    return s32Ret;
}

static AX_VOID SAMPLE_VO_FB_FILL(AX_U32 u32Width, AX_U32 u32Height, AX_U32 u32Color, AX_U8 *pShowScreen)
{
    AX_S32 i, j;
    AX_U32 *u32Pixel;

    for (i = 0; i < u32Height; i++) {
        u32Pixel = (AX_U32 *)(pShowScreen + i * u32Width * 4);
        for (j = 0; j < u32Width; j++) {
            u32Pixel[j] = u32Color;
        }
    }
}

static AX_S32 SAMPLE_VO_FB_DRAW(AX_U32 u32FbIndex)
{
    AX_S32 i, s32Fd, s32Ret = 0;
    AX_U32 u32Offs, u32Color;
    AX_CHAR fbPath[32];
    AX_U8 *pShowScreen;
    struct fb_var_screeninfo stVar;
    struct fb_fix_screeninfo stFix;

    /* 1.Open framebuffer device */
    snprintf(fbPath, sizeof(fbPath), "/dev/fb%d", u32FbIndex);
    s32Fd = open(fbPath, O_RDWR);
    if (s32Fd < 0) {
        SAMPLE_PRT("open %s failed, err:%s\n", fbPath, strerror(errno));
        return s32Fd;
    }

    /* 2.Get the variable screen info */
    s32Ret = ioctl(s32Fd, FBIOGET_VSCREENINFO, &stVar);
    if (s32Ret < 0) {
        SAMPLE_PRT("get variable screen info from fb%d failed\n", u32FbIndex);
        goto exit;
    }

    /* 3.Get the fix screen info */
    s32Ret = ioctl(s32Fd, FBIOGET_FSCREENINFO, &stFix);
    if (s32Ret < 0) {
        SAMPLE_PRT("get fix screen info from fb%d failed\n", u32FbIndex);
        goto exit;
    }

    /* 4.Map the physical video memory for user use */
    pShowScreen = mmap(NULL, stFix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, s32Fd, 0);
    if (pShowScreen == (AX_U8 *) - 1) {
        SAMPLE_PRT("map fb%d failed\n", u32FbIndex);
        goto exit;
    }

    for (i = 0; i < 200; i++) {
        if (i % 2) {
            stVar.yoffset = 0;
            u32Color = 0xFF0000FF;
            u32Offs = 0;
        } else {
            stVar.yoffset = stVar.yres;
            u32Color = 0xFFFF0000;
            u32Offs = stVar.xres * stVar.yres * 4;
        }

        SAMPLE_VO_FB_FILL(stVar.xres, stVar.yres, u32Color, pShowScreen + u32Offs);

        s32Ret = ioctl(s32Fd, FBIOPAN_DISPLAY, &stVar);
        if (s32Ret) {
            SAMPLE_PRT("pan fb%d failed, i = %d\n", u32FbIndex, i);
            break;
        }

        usleep(50000);
    }

    munmap(pShowScreen, stFix.smem_len);

exit:
    close(s32Fd);
    return s32Ret;
}

AX_S32 main(AX_S32 argc, AX_CHAR *argv[])
{
    AX_S32 s32Ret = 0;
    AX_U32 u32Width, u32Height, u32FbIndex = 0;
    VO_DEV VoDev = 0;
    GRAPHIC_LAYER GraphicLayer = 0;
    AX_VO_PUB_ATTR_T stVoPubAttr;

    memset(&stVoPubAttr, 0, sizeof(stVoPubAttr));
    stVoPubAttr.enIntfType = AX_VO_INTF_DSI;
    stVoPubAttr.enIntfSync = AX_VO_OUTPUT_720P60;

    u32Width = 1280;
    u32Height = 720;

    s32Ret = AX_VO_Init();
    if (s32Ret) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        goto exit0;
    }

    s32Ret = AX_VO_SetPubAttr(VoDev, &stVoPubAttr);
    if (s32Ret) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        goto exit1;
    }

    s32Ret = SAMPLE_VO_FB_INIT(u32Width, u32Height, u32FbIndex);
    if (s32Ret) {
        SAMPLE_PRT("SAMPLE_VO_FB_INIT failed, s32Ret = %d\n", s32Ret);
        goto exit1;
    }

    s32Ret = AX_VO_Enable(VoDev);
    if (s32Ret) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        goto exit1;
    }

    s32Ret = AX_VO_BindGraphicLayer(GraphicLayer, VoDev);
    if (s32Ret) {
        SAMPLE_PRT("AX_VO_BindGraphicLayer failed, s32Ret = 0x%x\n", s32Ret);
        goto exit2;
    }

    SAMPLE_PRT("sample fb start show!!\n");
    SAMPLE_VO_FB_DRAW(u32FbIndex);

    s32Ret = AX_VO_UnBindGraphicLayer(GraphicLayer, VoDev);
    if (s32Ret) {
        SAMPLE_PRT("AX_VO_UnBindGraphicLayer failed, s32Ret = 0x%x\n", s32Ret);
    }

exit2:
    s32Ret = AX_VO_Disable(VoDev);
    if (s32Ret) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
    }

exit1:
    s32Ret = AX_VO_Deinit();
    if (s32Ret) {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
    }

exit0:
    SAMPLE_PRT("sample fb exit!!\n");
    return s32Ret;
}