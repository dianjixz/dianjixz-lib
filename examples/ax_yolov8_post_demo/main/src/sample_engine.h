/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _SAMPLE_ENGINE_H_
#define _SAMPLE_ENGINE_H_
#include "ax_sys_api.h"
#include "ax_ivps_api.h"
#include "base/obj_type.h"
#define SAMPLE_ENGINE_MODEL_FILE "./axera-share-npu-yolov8s-pose.axmodel"
#define SAMPLE_ENGINE_OBJ_MAX_COUNT 32
#define INFER_WIDTH     640
#define INFER_HEIHGT    640
#define INFER_FORMAT    AX_FORMAT_RGB888

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        struct
        {
            AX_S32 x, y, width, height;
            AX_S32 class_label;
            AX_F32 prob;

            AX_S32 color;
            AX_CHAR class_name[32];
        } objs[SAMPLE_ENGINE_OBJ_MAX_COUNT];
        AX_S32 obj_count;
        std::vector<detection::Object> proposals;
        std::vector<detection::Object> objects;
    } SAMPLE_ENGINE_Results;

    AX_S32 SAMPLE_ENGINE_Load(AX_CHAR *model_file);
    AX_S32 SAMPLE_ENGINE_Release();
    AX_S32 SAMPLE_ENGINE_Inference(AX_VIDEO_FRAME_T *pFrame, SAMPLE_ENGINE_Results *pResults, cv::Mat &mat);
#ifdef __cplusplus
}

#endif

#endif