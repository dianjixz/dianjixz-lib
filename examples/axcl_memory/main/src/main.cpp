/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include "axcl.h"
#include "cmdline.h"
#include "logger.h"
#include "signal_handler.hpp"

static int32_t setup(const std::string& json, int32_t &device_id);
static void cleanup(int32_t device_id);

/**
 *  loop memcpy sample:
 *          HOST          |               DEVICE
 *       host_mem[0] -----------> dev_mem[0]
 *                                     |---------> dev_mem[1]
 *       host_mem[1] <----------------------------------|
 */
int main(int argc, char *argv[]) {
    SAMPLE_LOG_I("============== %s sample started %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    cmdline::parser a;
    a.add<int32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0, cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.parse_check(argc, argv);
    int32_t device_id = a.get<int32_t>("device");
    const std::string json = a.get<std::string>("json");

    if (AXCL_SUCC != setup(json, device_id)) {
        return 1;
    }

    axclError ret;
    constexpr uint32_t size = 8 * 1024 * 1024;

    void *host_mem[2] = {NULL, NULL};
    void *dev_mem[2] = {NULL, NULL};

    SAMPLE_LOG_I("alloc host and device memory, size: 0x%x", size);
    for (int i = 0; i < 2; ++i) {
        if (ret = axclrtMallocHost(&host_mem[i], size); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("alloc host memory[%d] fail, ret = 0x%x", i, ret);
            goto __END__;
        }

        if (ret = axclrtMalloc(&dev_mem[i], size, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("alloc device memory[%d] fail, ret = 0x%x", i, ret);
            goto __END__;
        }

        SAMPLE_LOG_I("memory [%d]: host %p, device %p", i, host_mem[i], dev_mem[i]);
    }

    ::memset(host_mem[0], 0xA8, size);
    ::memset(host_mem[1], 0x00, size);

    SAMPLE_LOG_I("memcpy from host memory[0] %p to device memory[0] %p", host_mem[0], dev_mem[0]);
    if (ret = axclrtMemcpy(dev_mem[0], host_mem[0], size, AXCL_MEMCPY_HOST_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from host memory[0] %p to device memory[0] %p fail, ret = 0x%x", host_mem[0], dev_mem[0], ret);
        goto __END__;
    }

    SAMPLE_LOG_I("memcpy device memory[0] %p to device memory[1] %p", dev_mem[0], dev_mem[1]);
    if (ret = axclrtMemcpy(dev_mem[1], dev_mem[0], size, AXCL_MEMCPY_DEVICE_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from device memory %p to device memory %p fail, ret = 0x%x", dev_mem[0], dev_mem[1], ret);
        goto __END__;
    }

    SAMPLE_LOG_I("memcpy device memory[1] %p to host memory[0] %p", dev_mem[1], host_mem[1]);
    if (ret = axclrtMemcpy(host_mem[1], dev_mem[1], size, AXCL_MEMCPY_DEVICE_TO_HOST); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy from device memory %p to host memory %p fail, ret = 0x%x", dev_mem[1], host_mem[1], ret);
        goto __END__;
    }

    if (0 == ::memcmp(host_mem[0], host_mem[1], size)) {
        SAMPLE_LOG_I("compare host memory[0] %p and host memory[1] %p success", host_mem[0], host_mem[1]);
    } else {
        SAMPLE_LOG_E("compare host memory[0] %p and host memory[1] %p failure", host_mem[0], host_mem[1]);
    }

__END__:
    for (int i = 0; i < 2; ++i) {
        if (host_mem[i]) {
            axclrtFreeHost(host_mem[i]);
        }

        if (dev_mem[i]) {
            axclrtFree(dev_mem[i]);
        }
    }

    cleanup(device_id);

    SAMPLE_LOG_I("============== %s sample exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}

static int32_t setup(const std::string& json, int32_t &device_id) {
    axclError ret;
    SAMPLE_LOG_I("json: %s", json.c_str());
    if (ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axclInit(%s) fail, ret = 0x%x", json.c_str(), ret);
        return ret;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    }

    if (device_id < 0 || device_id >= (int32_t)device_list.num) {
        SAMPLE_LOG_E("device index %d is out of connected device num %d", device_id, device_list.num);
        axclFinalize();
        return -1;
    }

    SAMPLE_LOG_I("device index: %d, bus number: %d", device_id, device_list.devices[device_id]);
    device_id = device_list.devices[device_id];

    if (ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axclrtSetDevice(%d) fail, ret = 0x%x", device_id, ret);
        axclFinalize();
        return ret;
    }

    return ret;
}

static void cleanup(int32_t device_id) {
    axclrtResetDevice(device_id);
    axclFinalize();
    SAMPLE_LOG_I("deactive device %d and cleanup axcl", device_id);
}
