/*
 * AXERA is pleased to support the open source community by making ax-samples available.
 *
 * Copyright (c) 2022, AXERA Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

/*
 * Author: ls.wang
 */

#pragma once

#include <cstdint>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
namespace detection
{
typedef struct {
    int grid0;
    int grid1;
    int stride;
} GridAndStride;

typedef struct Object {
    cv::Rect_<float> rect;
    int label;
    float prob;
    cv::Point2f landmark[5];
    /* for yolov5-seg */
    cv::Mat mask;
    std::vector<float> mask_feat;
    std::vector<float> kps_feat;
    /* for yolov8-obb */
    float angle;
} Object;

/* for palm detection */
typedef struct PalmObject {
    cv::Rect_<float> rect;
    float prob;
    cv::Point2f vertices[4];
    cv::Point2f landmarks[7];
    cv::Mat affine_trans_mat;
    cv::Mat affine_trans_mat_inv;
} PalmObject;
};