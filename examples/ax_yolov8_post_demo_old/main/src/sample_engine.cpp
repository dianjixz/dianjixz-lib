/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "sample_engine.h"
#include <vector>
#include "base/detection.hpp"
#include "utilities/file.hpp"
#include "middleware/io.hpp"
#include "ax_engine_api.h"

union ax_abgr_t {
    unsigned char abgr[4];
    int iargb;

    ax_abgr_t()
    {
        iargb = 0;
    }

    ax_abgr_t(unsigned char a, unsigned char b, unsigned char g, unsigned char r)
    {
        abgr[0] = a;
        abgr[1] = b;
        abgr[2] = g;
        abgr[3] = r;
    }
};

static const std::vector<ax_abgr_t> COCO_COLORS_ARGB = {
    {0, 255, 0, 255},   {128, 226, 255, 0}, {128, 0, 94, 255},  {128, 0, 37, 255},  {128, 0, 255, 94},
    {128, 255, 226, 0}, {128, 0, 18, 255},  {128, 255, 151, 0}, {128, 170, 0, 255}, {128, 0, 255, 56},
    {128, 255, 0, 75},  {128, 0, 75, 255},  {128, 0, 255, 169}, {128, 255, 0, 207}, {128, 75, 255, 0},
    {128, 207, 0, 255}, {128, 37, 0, 255},  {128, 0, 207, 255}, {128, 94, 0, 255},  {128, 0, 255, 113},
    {128, 255, 18, 0},  {128, 255, 0, 56},  {128, 18, 0, 255},  {128, 0, 255, 226}, {128, 170, 255, 0},
    {128, 255, 0, 245}, {128, 151, 255, 0}, {128, 132, 255, 0}, {128, 75, 0, 255},  {128, 151, 0, 255},
    {128, 0, 151, 255}, {128, 132, 0, 255}, {128, 0, 255, 245}, {128, 255, 132, 0}, {128, 226, 0, 255},
    {128, 255, 37, 0},  {128, 207, 255, 0}, {128, 0, 255, 207}, {128, 94, 255, 0},  {128, 0, 226, 255},
    {128, 56, 255, 0},  {128, 255, 94, 0},  {128, 255, 113, 0}, {128, 0, 132, 255}, {128, 255, 0, 132},
    {128, 255, 170, 0}, {128, 255, 0, 188}, {128, 113, 255, 0}, {128, 245, 0, 255}, {128, 113, 0, 255},
    {128, 255, 188, 0}, {128, 0, 113, 255}, {128, 255, 0, 0},   {128, 0, 56, 255},  {128, 255, 0, 113},
    {128, 0, 255, 188}, {128, 255, 0, 94},  {128, 255, 0, 18},  {128, 18, 255, 0},  {128, 0, 255, 132},
    {128, 0, 188, 255}, {128, 0, 245, 255}, {128, 0, 169, 255}, {128, 37, 255, 0},  {128, 255, 0, 151},
    {128, 188, 0, 255}, {128, 0, 255, 37},  {128, 0, 255, 0},   {128, 255, 0, 170}, {128, 255, 0, 37},
    {128, 255, 75, 0},  {128, 0, 0, 255},   {128, 255, 207, 0}, {128, 255, 0, 226}, {128, 255, 245, 0},
    {128, 188, 255, 0}, {128, 0, 255, 18},  {128, 0, 255, 75},  {128, 0, 255, 151}, {128, 255, 56, 0},
    {128, 245, 255, 0}};

const char *CLASS_NAMES[] = {
    "person",
};

// const std::vector<std::vector<uint8_t> > KPS_COLORS = {
//     {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},   {255, 128, 0},
//     {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0}, {51, 153, 255},
//     {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255}};
// const std::vector<std::vector<uint8_t> > LIMB_COLORS = {
//     {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {51, 153, 255}, {255, 51, 255}, {255, 51, 255}, {255, 51, 255},
//     {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {255, 128, 0},  {0, 255, 0},    {0, 255, 0},
//     {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0},    {0, 255, 0}};
// const std::vector<std::vector<uint8_t> > SKELETON = {{16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13}, {6, 12}, {7, 13},
//                                                      {6, 7},   {6, 8},   {7, 9},   {8, 10},  {9, 11},  {2, 3},  {1, 2},
//                                                      {1, 3},   {2, 4},   {3, 5},   {4, 6},   {5, 7}};

int NUM_CLASS = 1;
int NUM_POINT = 17;

const int DEFAULT_LOOP_COUNT = 1;

const float PROB_THRESHOLD = 0.45f;
const float NMS_THRESHOLD  = 0.45f;

#define AX_CMM_ALIGN_SIZE 128

const std::vector<std::vector<uint8_t> > KPS_COLORS = {
    {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0},
    {255, 255, 128, 0},  {255, 255, 128, 0},  {255, 255, 128, 0},  {255, 255, 128, 0},  {255, 255, 128, 0},
    {255, 255, 128, 0},  {255, 51, 153, 255}, {255, 51, 153, 255}, {255, 51, 153, 255}, {255, 51, 153, 255},
    {255, 51, 153, 255}, {255, 51, 153, 255}};
const std::vector<std::vector<uint8_t> > LIMB_COLORS = {
    {255, 51, 153, 255}, {255, 51, 153, 255}, {255, 51, 153, 255}, {255, 51, 153, 255}, {255, 255, 51, 255},
    {255, 255, 51, 255}, {255, 255, 51, 255}, {255, 255, 128, 0},  {255, 255, 128, 0},  {255, 255, 128, 0},
    {255, 255, 128, 0},  {255, 255, 128, 0},  {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0},
    {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0},    {255, 0, 255, 0}};
const std::vector<std::vector<uint8_t> > SKELETON = {{16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13}, {6, 12}, {7, 13},
                                                     {6, 7},   {6, 8},   {7, 9},   {8, 10},  {9, 11},  {2, 3},  {1, 2},
                                                     {1, 3},   {2, 4},   {3, 5},   {4, 6},   {5, 7}};

static void my_draw_keypoints(const cv::Mat &bgr, const std::vector<detection::Object> &objects,
                           const std::vector<std::vector<uint8_t> > &kps_colors,
                           const std::vector<std::vector<uint8_t> > &limb_colors,
                           const std::vector<std::vector<uint8_t> > &skeleton, const char *output_name)
{
    float text_size = 1.0;
    int line_size = 5;
    // cv::Mat image = bgr;

    for (size_t i = 0; i < objects.size(); i++) {
        const detection::Object &obj = objects[i];

        fprintf(stdout, "%2d: %3.0f%%, [%4.0f, %4.0f, %4.0f, %4.0f], person\n", obj.label, obj.prob * 100, obj.rect.x,
                obj.rect.y, obj.rect.x + obj.rect.width, obj.rect.y + obj.rect.height);

        cv::rectangle(bgr, obj.rect, cv::Scalar(255, 0, 0, 255), line_size);

        char text[256];
        sprintf(text, "person %.1f%%", obj.prob * 100);

        {

            
            int baseLine        = 0;
            cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, text_size, 1, &baseLine);
    
            int x = obj.rect.x;
            int y = obj.rect.y - label_size.height - baseLine;
            if (y < 0) y = 0;
            if (x + label_size.width > bgr.cols) x = bgr.cols - label_size.width;
            // {
            //     cv::rectangle(bgr, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
            //                 cv::Scalar(255, 255, 255, 255), -1);
            //     cv::putText(bgr, text, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, text_size,
            //                 cv::Scalar(0, 0, 0, 255));
            // }
            {
                cv::Mat textImage(label_size.height + baseLine, label_size.width, CV_8UC4, cv::Scalar(255, 255, 255, 255));
                cv::putText(textImage, text, cv::Point(0, label_size.height), cv::FONT_HERSHEY_SIMPLEX, text_size,
                            cv::Scalar(0, 0, 0, 255));
                cv::flip(textImage, textImage, 1);
                textImage.copyTo(bgr(cv::Rect(x, y, textImage.cols, textImage.rows)));
            }
        }

        const int num_point = obj.kps_feat.size() / 3;
        for (int j = 0; j < num_point + 2; j++) {
            // draw circle
            if (j < num_point) {
                int kps_x   = std::round(obj.kps_feat[j * 3]);
                int kps_y   = std::round(obj.kps_feat[j * 3 + 1]);
                float kps_s = obj.kps_feat[j * 3 + 2];
                if (kps_s > 0.5f) {
                    auto kps_color = cv::Scalar(kps_colors[j][1], kps_colors[j][2], kps_colors[j][3], kps_colors[j][0]);
                    cv::circle(bgr, {kps_x, kps_y}, line_size, kps_color, -1);
                }
            }
            // draw line
            auto &ske  = skeleton[j];
            int pos1_x = obj.kps_feat[(ske[0] - 1) * 3];
            int pos1_y = obj.kps_feat[(ske[0] - 1) * 3 + 1];

            int pos2_x = obj.kps_feat[(ske[1] - 1) * 3];
            int pos2_y = obj.kps_feat[(ske[1] - 1) * 3 + 1];

            float pos1_s = obj.kps_feat[(ske[0] - 1) * 3 + 2];
            float pos2_s = obj.kps_feat[(ske[1] - 1) * 3 + 2];

            if (pos1_s > 0.5f && pos2_s > 0.5f) {
                auto limb_color =
                    cv::Scalar(limb_colors[j][1], limb_colors[j][2], limb_colors[j][3], limb_colors[j][0]);
                cv::line(bgr, {pos1_x, pos1_y}, {pos2_x, pos2_y}, limb_color, line_size);
            }
        }
    }
}


static void post_process_yolov8s_pose(AX_ENGINE_IO_INFO_T *io_info, AX_ENGINE_IO_T *io_data, int input_w, int input_h,
                                      int output_w, int output_h)
{
    std::vector<detection::Object> proposals;
    std::vector<detection::Object> objects;
    // timer timer_postprocess;

    float *output_ptr[3]     = {(float *)io_data->pOutputs[0].pVirAddr,   // 1*80*80*65
                                (float *)io_data->pOutputs[1].pVirAddr,   // 1*40*40*65
                                (float *)io_data->pOutputs[2].pVirAddr};  // 1*20*20*65
    float *output_kps_ptr[3] = {(float *)io_data->pOutputs[3].pVirAddr,   // 1*80*80*51
                                (float *)io_data->pOutputs[4].pVirAddr,   // 1*40*40*51
                                (float *)io_data->pOutputs[5].pVirAddr};  // 1*20*20*51

    for (int i = 0; i < 3; ++i) {
        auto feat_ptr     = output_ptr[i];
        auto feat_kps_ptr = output_kps_ptr[i];
        int32_t stride    = (1 << i) * 8;
        detection::generate_proposals_yolov8_pose_native(stride, feat_ptr, feat_kps_ptr, PROB_THRESHOLD, proposals,
                                                         input_w, input_h, NUM_POINT, NUM_CLASS);
    }

    detection::get_out_bbox_kps(proposals, objects, NMS_THRESHOLD, input_h, input_w, output_h, output_w);
}

struct ax_runner_ax650_handle_t {
    AX_ENGINE_HANDLE handle;
    AX_ENGINE_IO_INFO_T *io_info;
    AX_ENGINE_IO_T io_data;
    AX_ENGINE_HANDLE_EXTRA_T handle_extra;
    int algo_width, algo_height;
    int algo_colorformat;
    std::string mode_name;
};

static struct ax_runner_ax650_handle_t *m_handle = nullptr;

AX_S32 SAMPLE_ENGINE_Load(AX_CHAR *model_file)
{
    if (m_handle) {
        return 0;
    }
    m_handle            = new ax_runner_ax650_handle_t;
    m_handle->mode_name = std::string(model_file);
    m_handle->mode_name = m_handle->mode_name.substr(m_handle->mode_name.rfind('/') + 1);
    // m_handle->mode_name = m_handle->mode_name.substr(0, m_handle->mode_name.find('.'));

    // 1. init engine(vin has init)
    // AX_ENGINE_NPU_ATTR_T npu_attr;
    // memset(&npu_attr, 0, sizeof(npu_attr));
    // npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_DISABLE;
    // auto ret = AX_ENGINE_Init(&npu_attr);
    // if (0 != ret)
    // {
    //     return ret;
    // }

    // 2. load model
    std::vector<char> model_buffer;
    if (!utilities::read_file(model_file, model_buffer)) {
        fprintf(stderr, "Read Run-Joint model(%s) file failed.\n", model_file);
        return -1;
    }

    // 3. create handle
    // m_handle->handle_extra.nNpuSet = 0b10;
    // m_handle->handle_extra.pName = (AX_S8 *)m_handle->mode_name.c_str();
    auto ret = AX_ENGINE_CreateHandle(&m_handle->handle, model_buffer.data(), model_buffer.size());
    // auto ret = AX_ENGINE_CreateHandleV2(&m_handle->handle, model_buffer.data(), model_buffer.size(),
    // &m_handle->handle_extra);
    if (0 != ret) {
        return ret;
    }
    // fprintf(stdout, "Engine creating handle is done.\n");

    // 4. create context
    ret = AX_ENGINE_CreateContext(m_handle->handle);
    if (0 != ret) {
        return ret;
    }
    // fprintf(stdout, "Engine creating context is done.\n");

    // 5. set io

    ret = AX_ENGINE_GetIOInfo(m_handle->handle, &m_handle->io_info);
    if (0 != ret) {
        return ret;
    }
    // fprintf(stdout, "Engine get io info is done. \n");

    // 6. alloc io

    ret = middleware::prepare_io(m_handle->io_info, &m_handle->io_data,
                                 std::make_pair(AX_ENGINE_ABST_DEFAULT, AX_ENGINE_ABST_CACHED));
    if (0 != ret) {
        return ret;
    }
    // fprintf(stdout, "Engine alloc io is done. \n");

    m_handle->algo_width = m_handle->io_info->pInputs[0].pShape[2];

    switch (m_handle->io_info->pInputs[0].pExtraMeta->eColorSpace) {
        case AX_ENGINE_CS_NV12:
            m_handle->algo_colorformat = (int)AX_FORMAT_YUV420_SEMIPLANAR;
            m_handle->algo_height      = m_handle->io_info->pInputs[0].pShape[1] / 1.5;
            // printf("NV12 MODEL\n");
            break;
        case AX_ENGINE_CS_RGB:
            m_handle->algo_colorformat = (int)AX_FORMAT_RGB888;
            m_handle->algo_height      = m_handle->io_info->pInputs[0].pShape[1];
            // printf("RGB MODEL\n");
            break;
        case AX_ENGINE_CS_BGR:
            m_handle->algo_colorformat = (int)AX_FORMAT_BGR888;
            m_handle->algo_height      = m_handle->io_info->pInputs[0].pShape[1];
            // printf("BGR MODEL\n");
            break;
        default:
            printf("now just only support NV12/RGB/BGR input format,you can modify by yourself");
            return -1;
    }

    return ret;
}
AX_S32 SAMPLE_ENGINE_Release()
{
    if (m_handle && m_handle->handle) {
        middleware::free_io(&m_handle->io_data);
        AX_ENGINE_DestroyHandle(m_handle->handle);
    }
    delete m_handle;
    m_handle = nullptr;
    return 0;
}

AX_S32 SAMPLE_ENGINE_Inference(AX_VIDEO_FRAME_T *pFrame, SAMPLE_ENGINE_Results *pResults, cv::Mat &mat)
{
    unsigned char *dst = (unsigned char *)m_handle->io_data.pInputs[0].pVirAddr;
    unsigned char *src = (unsigned char *)pFrame->u64VirAddr[0];

    AX_U32 stride = (0 == pFrame->u32PicStride[0]) ? pFrame->u32Width : pFrame->u32PicStride[0];
    for (size_t i = 0; i < pFrame->u32Height; i++) {
        memcpy(dst + i * pFrame->u32Width * 3, src + i * stride * 3, pFrame->u32Width * 3);
    }
    AX_S32 ret = AX_ENGINE_RunSync(m_handle->handle, &m_handle->io_data);
    if (ret) {
        fprintf(stderr, "AX_ENGINE_RunSync 0x%x\n", ret);
        return -1;
    }

    {
        float *output_ptr[3]     = {(float *)m_handle->io_data.pOutputs[0].pVirAddr,   // 1*80*80*65
                                    (float *)m_handle->io_data.pOutputs[1].pVirAddr,   // 1*40*40*65
                                    (float *)m_handle->io_data.pOutputs[2].pVirAddr};  // 1*20*20*65
        float *output_kps_ptr[3] = {(float *)m_handle->io_data.pOutputs[3].pVirAddr,   // 1*80*80*51
                                    (float *)m_handle->io_data.pOutputs[4].pVirAddr,   // 1*40*40*51
                                    (float *)m_handle->io_data.pOutputs[5].pVirAddr};  // 1*20*20*51

        for (int i = 0; i < 3; ++i) {
            auto feat_ptr     = output_ptr[i];
            auto feat_kps_ptr = output_kps_ptr[i];
            int32_t stride    = (1 << i) * 8;
            detection::generate_proposals_yolov8_pose_native(stride, feat_ptr, feat_kps_ptr, PROB_THRESHOLD,
                                                             pResults->proposals, 640, 640, NUM_POINT, NUM_CLASS);
        }

        detection::get_out_bbox_kps(pResults->proposals, pResults->objects, NMS_THRESHOLD, 640, 640, 720, 1280);
    }
    if(mat.cols != 0)
        my_draw_keypoints(mat, pResults->objects, KPS_COLORS, LIMB_COLORS, SKELETON, "yolov8_pose_out");
    return 0;
}
