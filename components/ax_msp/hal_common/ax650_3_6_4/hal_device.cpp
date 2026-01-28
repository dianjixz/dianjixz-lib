#include "hal_device.h"
#include <iostream>
#include <string>
#include <memory>
#include "ax_sys_api.h"
#include <ax_venc_api.h>
#include <ax_vdec_api.h>
#include <ax_engine_api.h>
SysGuard::SysGuard()
{
    AX_SYS_Init();
}

SysGuard::~SysGuard()
{
    AX_SYS_Deinit();
}

// VencGuard::VencGuard(const hal_device& device)
// {
//     sys = device.get_sys_guard();
//     AX_VENC_Init();
// }
// VencGuard::~VencGuard()
// {
//     AX_VENC_DeInit();
// }

// VdecGuard::VdecGuard(const hal_device& device)
// {
//     sys = device.get_sys_guard();
//     AX_VDEC_Init();
// }
// VdecGuard::~VdecGuard()
// {
//     AX_VDEC_DeInit();
// }

int get_format_size(int format, int width, int height)  // 修复：参数顺序
{
    switch (format) {
        case AX_FORMAT_YUV420_SEMIPLANAR:
            return width * height * 3 / 2;
        default:
            return -1;
    }
};
bool endsWith(const std::string& str, const std::string& suffix)
{
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

