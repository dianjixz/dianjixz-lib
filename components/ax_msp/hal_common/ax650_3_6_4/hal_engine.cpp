#include "hal_engine.hpp"
#include <memory>
#include <string>
#include <unordered_set>
#include <mutex>
#include "hal_device.h"
#include <ax_engine_api.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../../utilities/include/json.hpp"


class AX650ModelHandle : public modelhandle {
private:
    hal_engine& father_dev;
    AX_ENGINE_HANDLE handle;
    AX_ENGINE_IO_INFO_T* io_info;
    AX_ENGINE_IO_T io_data;

public:
    AX650ModelHandle(hal_engine& dev) : father_dev(dev)
    {
        _in_width  = 0;
        _in_height = 0;
    };
    virtual void load_model(const std::string& mode) override
    {
        _model_path = mode;
        if (_model_path.empty()) {
            throw std::runtime_error("Model Path cannot be empty");
        }

        if (endsWith(mode, ".json")) {
            nlohmann::json config;
            std::ifstream file(mode);
            if (!file) {
                throw std::runtime_error("Error opening file: " + mode);
            }
            try {
                file >> config;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Error parsing JSON: ") + e.what());
            }
            if (!config.contains("MODEL_PATH") || !config["MODEL_PATH"].is_string()) {
                throw std::runtime_error("Missing or invalid 'MODEL_PATH' field in json");
            }
            std::string model_path = config["MODEL_PATH"];
            if (model_path.empty()) {
                throw std::runtime_error("Model Path cannot be empty");
            }
            _model_path = model_path;

            try {
                _in_height   = config["INPUT_IMG_H"];
                _in_width    = config["INPUT_IMG_W"];
                _model_type  = config["MODEL_TYPE"];
                _class_names = config["CLASS_NAMES"];
                _anchors     = config["ANCHORS"].get<std::vector<int>>();
            } catch (...) {
            }
        }
        std::cout << "Loading model: " << _model_path << std::endl;
        int fd = open(_model_path.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Error opening file");
        }

        // 获取文件大小
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Error getting file size");
        }
        off_t filesize = sb.st_size;

        // 创建内存映射
        char* mapped = (char*)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Error mapping file");
        }
        int ret = AX_ENGINE_CreateHandle(&handle, mapped, filesize);
        if (0 != ret) {
            // fprintf(stderr, "AX_ENGINE_CreateHandle failed, ret = 0x%x\n", ret);
            throw std::runtime_error("Error creating engine handle");
        }

        // // 访问文件内容（例如打印前100个字符）
        // size_t print_len = filesize < 100 ? filesize : 100;
        // for (size_t i = 0; i < print_len; i++) {
        //     putchar(mapped[i]);
        // }
        // putchar('\n');

        // 取消映射并关闭文件
        if (munmap(mapped, filesize) == -1) {
            perror("Error unmapping file");
        }
        close(fd);

        ret = AX_ENGINE_CreateContext(handle);
        if (0 != ret) {
            // fprintf(stderr, "AX_ENGINE_CreateContext failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            throw std::runtime_error("Error creating engine context");
        }

        ret = AX_ENGINE_GetIOInfo(handle, &io_info);
        if (0 != ret) {
            // fprintf(stderr, "AX_ENGINE_GetIOInfo failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            handle = nullptr;
            throw std::runtime_error("Error getting engine IO info");
        }

        memset(&io_data, 0, sizeof(io_data));
        ret = middleware::prepare_io(io_info, &io_data, std::make_pair(AX_ENGINE_ABST_DEFAULT, AX_ENGINE_ABST_CACHED));
        if (0 != ret) {
            // fprintf(stderr, "prepare_io failed, ret = 0x%x\n", ret);
            AX_ENGINE_DestroyHandle(handle);
            handle = nullptr;
            throw std::runtime_error("Error preparing engine IO");
        }
        model_info = io_info;
        model_data = &io_data;
    }
    virtual bool ForwardComputation(const std::vector<uint8_t>& data) override
    {
        AX_S32 ret;
        if (handle == nullptr) return false;
        memcpy(io_data.pInputs[0].pVirAddr, data.data(), data.size());
        ret = AX_ENGINE_RunSync(handle, &io_data);
        if (0 != ret) {
            fprintf(stderr, "AX_ENGINE_RunSync failed, ret = 0x%x\n", ret);
            return false;
        }
        if (model_result_put_func) {
            model_result_put_func(&io_data, io_info);
        }
        return true;
    }
    virtual int work() override
    {
        return 0;
    }
    virtual void destroy() override
    {
        if (handle) {
            middleware::free_io(&io_data);
            AX_ENGINE_DestroyHandle(handle);
            handle = nullptr;
        }
    };
    ~AX650ModelHandle()
    {
        destroy();
        father_dev.destroy(_handle_index);
    };
};

modelhandle::modelhandle()
{
}
modelhandle::~modelhandle()
{
}

hal_engine::hal_engine(const hal_device& device)
{
    sys = device.get_sys_guard();
    if (_self_count == 0) {
        AX_ENGINE_NPU_ATTR_T npu_attr;
        memset(&npu_attr, 0, sizeof(npu_attr));
        npu_attr.eHardMode = device.get_npu_mode();
        int ret            = AX_ENGINE_Init(&npu_attr);
        if (0 != ret) {
            throw std::runtime_error("AX_ENGINE_Init failed");
        }
    }
    _self_count++;
}

std::shared_ptr<modelhandle> hal_engine::creat(const std::string& model_path)
{
    std::shared_ptr<modelhandle> handle = std::make_shared<AX650ModelHandle>(*this);
    handle->_handle_index               = _handle_index++;
    handle->load_model(model_path);
    encoder_list.push_back(handle);
    return handle;
}

hal_engine::~hal_engine()
{
    for (auto it = encoder_list.begin(); it != encoder_list.end(); ++it) {
        (*it)->destroy();
    }
    encoder_list.clear();
    _self_count--;
    if (_self_count == 0) {
        AX_ENGINE_Deinit();
    }
};
int hal_engine::_self_count = 0;
// std::atomic<int> hal_engine::_self_count = 0;



const char* AX_CMM_SESSION_NAME = "npu";

namespace middleware {

void free_io_index(AX_ENGINE_IO_BUFFER_T* io_buf, size_t index)
{
    for (size_t i = 0; i < index; ++i) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io_buf + i;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
}

void free_io(AX_ENGINE_IO_T* io)
{
    for (size_t j = 0; j < io->nInputSize; ++j) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io->pInputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    for (size_t j = 0; j < io->nOutputSize; ++j) {
        AX_ENGINE_IO_BUFFER_T* pBuf = io->pOutputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    delete[] io->pInputs;
    delete[] io->pOutputs;
}

int prepare_io(AX_ENGINE_IO_INFO_T* info, AX_ENGINE_IO_T* io_data, INPUT_OUTPUT_ALLOC_STRATEGY strategy)
{
    memset(io_data, 0, sizeof(*io_data));
    io_data->pInputs = new AX_ENGINE_IO_BUFFER_T[info->nInputSize];
    memset(io_data->pInputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nInputSize);

    io_data->nInputSize = info->nInputSize;

    auto ret = 0;
    for (AX_U32 i = 0; i < info->nInputSize; ++i) {
        auto meta   = info->pInputs[i];
        auto buffer = &io_data->pInputs[i];
        if (strategy.first == AX_ENGINE_ABST_CACHED) {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                        (const AX_S8*)(AX_CMM_SESSION_NAME));
        } else {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                  (const AX_S8*)(AX_CMM_SESSION_NAME));
        }

        if (ret != 0) {
            free_io_index(io_data->pInputs, i);
            fprintf(stderr, "Allocate input{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i,
                    (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            return ret;
        }
        // fprintf(stderr, "Allocate input{%d} { phy: %p, vir: %p, size: %lu Bytes }. \n", i, (void*)buffer->phyAddr,
        // buffer->pVirAddr, (long)meta.nSize);
    }

    io_data->pOutputs = new AX_ENGINE_IO_BUFFER_T[info->nOutputSize];
    memset(io_data->pOutputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nOutputSize);

    io_data->nOutputSize = info->nOutputSize;
    for (AX_U32 i = 0; i < info->nOutputSize; ++i) {
        auto meta     = info->pOutputs[i];
        auto buffer   = &io_data->pOutputs[i];
        buffer->nSize = meta.nSize;
        if (strategy.second == AX_ENGINE_ABST_CACHED) {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                        (const AX_S8*)(AX_CMM_SESSION_NAME));
        } else {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE,
                                  (const AX_S8*)(AX_CMM_SESSION_NAME));
        }
        if (ret != 0) {
            fprintf(stderr, "Allocate output{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i,
                    (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            free_io_index(io_data->pInputs, io_data->nInputSize);
            free_io_index(io_data->pOutputs, i);
            return ret;
        }
        // fprintf(stderr, "Allocate output{%d} { phy: %p, vir: %p, size: %lu Bytes }.\n", i, (void*)buffer->phyAddr,
        // buffer->pVirAddr, (long)meta.nSize);
    }

    return 0;
}

int push_input(const std::vector<uint8_t>& data, AX_ENGINE_IO_T* io_t, AX_ENGINE_IO_INFO_T* info_t)
{
    if (info_t->nInputSize != 1) {
        fprintf(stderr, "Only support Input size == 1 current now");
        return -1;
    }

    if (data.size() != info_t->pInputs[0].nSize) {
        fprintf(stderr, "The input data size is not matched with tensor {name: %s, size: %d}.\n",
                info_t->pInputs[0].pName, info_t->pInputs[0].nSize);
        return -1;
    }

    memcpy(io_t->pInputs[0].pVirAddr, data.data(), data.size());

    return 0;
}
}  // namespace middleware




#include <cmath>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

namespace detection
{
    static inline float sigmoid(float x)
    {
        return static_cast<float>(1.f / (1.f + std::exp(-x)));
    }

    static inline float intersection_area(const Object& a, const Object& b)
    {
        cv::Rect_<float> inter = a.rect & b.rect;
        return inter.area();
    }

    static void qsort_descent_inplace(std::vector<Object>& faceobjects, int left, int right)
    {
        int i = left;
        int j = right;
        float p = faceobjects[(left + right) / 2].prob;

        while (i <= j)
        {
            while (faceobjects[i].prob > p)
                i++;

            while (faceobjects[j].prob < p)
                j--;

            if (i <= j)
            {
                // swap
                std::swap(faceobjects[i], faceobjects[j]);

                i++;
                j--;
            }
        }
#pragma omp parallel sections
        {
#pragma omp section
            {
                if (left < j) qsort_descent_inplace(faceobjects, left, j);
            }
#pragma omp section
            {
                if (i < right) qsort_descent_inplace(faceobjects, i, right);
            }
        }
    }

    static void qsort_descent_inplace(std::vector<Object>& faceobjects)
    {
        if (faceobjects.empty())
            return;

        qsort_descent_inplace(faceobjects, 0, faceobjects.size() - 1);
    }

    void nms_sorted_bboxes(const std::vector<Object>& faceobjects, std::vector<int>& picked, float nms_threshold)
    {
        picked.clear();

        const int n = faceobjects.size();

        std::vector<float> areas(n);
        for (int i = 0; i < n; i++)
        {
            areas[i] = faceobjects[i].rect.area();
        }

        for (int i = 0; i < n; i++)
        {
            const Object& a = faceobjects[i];

            int keep = 1;
            for (int j = 0; j < (int)picked.size(); j++)
            {
                const Object& b = faceobjects[picked[j]];

                // intersection over union
                float inter_area = intersection_area(a, b);
                float union_area = areas[i] + areas[picked[j]] - inter_area;
                // float IoU = inter_area / union_area
                if (inter_area / union_area > nms_threshold)
                    keep = 0;
            }

            if (keep)
                picked.push_back(i);
        }
    }

    // static void generate_grids_and_stride(const int target_w, const int target_h, std::vector<int>& strides, std::vector<GridAndStride>& grid_strides)
    // {
    //     for (auto stride : strides)
    //     {
    //         int num_grid_w = target_w / stride;
    //         int num_grid_h = target_h / stride;
    //         for (int g1 = 0; g1 < num_grid_h; g1++)
    //         {
    //             for (int g0 = 0; g0 < num_grid_w; g0++)
    //             {
    //                 GridAndStride gs;
    //                 gs.grid0 = g0;
    //                 gs.grid1 = g1;
    //                 gs.stride = stride;
    //                 grid_strides.push_back(gs);
    //             }
    //         }
    //     }
    // }

    // static void generate_proposals_scrfd(int feat_stride, const float* score_blob,
    //                                      const float* bbox_blob, const float* kps_blob,
    //                                      float prob_threshold, std::vector<detection::Object>& faceobjects, int letterbox_cols, int letterbox_rows)
    // {
    //     static float anchors[] = {-8.f, -8.f, 8.f, 8.f, -16.f, -16.f, 16.f, 16.f, -32.f, -32.f, 32.f, 32.f, -64.f, -64.f, 64.f, 64.f, -128.f, -128.f, 128.f, 128.f, -256.f, -256.f, 256.f, 256.f};
    //     int feat_w = letterbox_cols / feat_stride;
    //     int feat_h = letterbox_rows / feat_stride;
    //     int feat_size = feat_w * feat_h;
    //     int anchor_group = 1;
    //     if (feat_stride == 8)
    //         anchor_group = 1;
    //     if (feat_stride == 16)
    //         anchor_group = 2;
    //     if (feat_stride == 32)
    //         anchor_group = 3;

    //     // generate face proposal from bbox deltas and shifted anchors
    //     const int num_anchors = 2;

    //     for (int q = 0; q < num_anchors; q++)
    //     {
    //         // shifted anchor
    //         float anchor_y = anchors[(anchor_group - 1) * 8 + q * 4 + 1];

    //         float anchor_w = anchors[(anchor_group - 1) * 8 + q * 4 + 2] - anchors[(anchor_group - 1) * 8 + q * 4 + 0];
    //         float anchor_h = anchors[(anchor_group - 1) * 8 + q * 4 + 3] - anchors[(anchor_group - 1) * 8 + q * 4 + 1];

    //         for (int i = 0; i < feat_h; i++)
    //         {
    //             float anchor_x = anchors[(anchor_group - 1) * 8 + q * 4 + 0];

    //             for (int j = 0; j < feat_w; j++)
    //             {
    //                 int index = i * feat_w + j;

    //                 float prob = sigmoid(score_blob[q * feat_size + index]);

    //                 if (prob >= prob_threshold)
    //                 {
    //                     // insightface/detection/scrfd/mmdet/models/dense_heads/scrfd_head.py _get_bboxes_single()
    //                     float dx = bbox_blob[(q * 4 + 0) * feat_size + index] * feat_stride;
    //                     float dy = bbox_blob[(q * 4 + 1) * feat_size + index] * feat_stride;
    //                     float dw = bbox_blob[(q * 4 + 2) * feat_size + index] * feat_stride;
    //                     float dh = bbox_blob[(q * 4 + 3) * feat_size + index] * feat_stride;
    //                     // insightface/detection/scrfd/mmdet/core/bbox/transforms.py distance2bbox()
    //                     float cx = anchor_x + anchor_w * 0.5f;
    //                     float cy = anchor_y + anchor_h * 0.5f;

    //                     float x0 = cx - dx;
    //                     float y0 = cy - dy;
    //                     float x1 = cx + dw;
    //                     float y1 = cy + dh;

    //                     Object obj;
    //                     obj.label = 0;
    //                     obj.rect.x = x0;
    //                     obj.rect.y = y0;
    //                     obj.rect.width = x1 - x0 + 1;
    //                     obj.rect.height = y1 - y0 + 1;
    //                     obj.prob = prob;

    //                     if (kps_blob != 0)
    //                     {
    //                         obj.landmark[0].x = cx + kps_blob[index] * feat_stride;
    //                         obj.landmark[0].y = cy + kps_blob[1 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[1].x = cx + kps_blob[2 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[1].y = cy + kps_blob[3 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[2].x = cx + kps_blob[4 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[2].y = cy + kps_blob[5 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[3].x = cx + kps_blob[6 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[3].y = cy + kps_blob[7 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[4].x = cx + kps_blob[8 * feat_h * feat_w + index] * feat_stride;
    //                         obj.landmark[4].y = cy + kps_blob[9 * feat_h * feat_w + index] * feat_stride;
    //                     }

    //                     faceobjects.push_back(obj);
    //                 }

    //                 anchor_x += feat_stride;
    //             }

    //             anchor_y += feat_stride;
    //         }
    //     }
    // }

    // static void generate_proposals_mobilenet_ssd(const float* score, const float* boxes, const int head_count, const int* feature_map_size, const int* anchor_size, const int cls_num,
    //                                              float prob_threshold, const float* strides, const float center_val, const float scale_val, const float* anchor_info, std::vector<detection::Object>& objects)
    // {
    //     auto ptr_score = score;
    //     auto ptr_boxes = boxes;
    //     auto ptr_anchor_info = anchor_info;
    //     for (int head = 0; head < head_count; ++head)
    //     {
    //         for (int fea_h = 0; fea_h < feature_map_size[head]; ++fea_h)
    //         {
    //             for (int fea_w = 0; fea_w < feature_map_size[head]; ++fea_w)
    //             {
    //                 for (int anchor_i = 0; anchor_i < anchor_size[head]; ++anchor_i)
    //                 {
    //                     float softmax_sum = 0;
    //                     float class_score = -FLT_MAX;
    //                     int class_index = 0;
    //                     for (int s = 0; s < cls_num + 1; s++)
    //                     {
    //                         softmax_sum += std::exp(ptr_score[s]);
    //                     }
    //                     for (int i = 0; i < cls_num + 1; ++i)
    //                     {
    //                         float temp = std::exp(ptr_score[i]) / softmax_sum;
    //                         //                            if (temp > class_score)
    //                         //                            {
    //                         //                                class_index = i;
    //                         //                                class_score = temp;
    //                         //                            }

    //                         class_index = i;
    //                         class_score = temp;

    //                         if (temp >= prob_threshold and class_index != 0)
    //                         {
    //                             // fprintf(stderr, "class_score: %f %d \n", class_score, i);

    //                             float pred_x = (((float)fea_w + 0.5f) / (300.0f / strides[head]) + ptr_boxes[0] * center_val * ptr_anchor_info[anchor_i * 2] / 300.0f);
    //                             float pred_y = (((float)fea_h + 0.5f) / (300.0f / strides[head]) + ptr_boxes[1] * center_val * ptr_anchor_info[anchor_i * 2 + 1] / 300.0f);
    //                             float pred_w = std::exp(ptr_boxes[2] * scale_val) * ptr_anchor_info[anchor_i * 2] / 300.0f;
    //                             float pred_h = std::exp(ptr_boxes[3] * scale_val) * ptr_anchor_info[anchor_i * 2 + 1] / 300.0f;

    //                             float x0 = (pred_x - pred_w * 0.5f) * 300.0f;
    //                             float y0 = (pred_y - pred_h * 0.5f) * 300.0f;
    //                             float x1 = (pred_x + pred_w * 0.5f) * 300.0f;
    //                             float y1 = (pred_y + pred_h * 0.5f) * 300.0f;

    //                             Object obj;
    //                             obj.rect.x = x0;
    //                             obj.rect.y = y0;
    //                             obj.rect.width = x1 - x0;
    //                             obj.rect.height = y1 - y0;
    //                             obj.label = class_index;
    //                             obj.prob = class_score;

    //                             objects.push_back(obj);
    //                         }
    //                     }

    //                     ptr_score += cls_num + 1;
    //                     ptr_boxes += 4;
    //                 }
    //             }
    //         }
    //         ptr_anchor_info += anchor_size[head] * 2;
    //     }
    // }

    // static void generate_proposals_yolox(int stride, const float* feat, float prob_threshold, std::vector<Object>& objects,
    //                                      int letterbox_cols, int letterbox_rows)
    // {
    //     int feat_w = letterbox_cols / stride;
    //     int feat_h = letterbox_rows / stride;
    //     int cls_num = 80;

    //     auto feat_ptr = feat;

    //     for (int h = 0; h <= feat_h - 1; h++)
    //     {
    //         for (int w = 0; w <= feat_w - 1; w++)
    //         {
    //             float box_objectness = feat_ptr[4];
    //             if (box_objectness < prob_threshold)
    //             {
    //                 feat_ptr += 85;
    //                 continue;
    //             }

    //             //process cls score
    //             int class_index = 0;
    //             float class_score = -FLT_MAX;
    //             for (int s = 0; s <= cls_num - 1; s++)
    //             {
    //                 float score = feat_ptr[s + 5];
    //                 if (score > class_score)
    //                 {
    //                     class_index = s;
    //                     class_score = score;
    //                 }
    //             }

    //             float box_prob = box_objectness * class_score;

    //             if (box_prob > prob_threshold)
    //             {
    //                 float x_center = (feat_ptr[0] + w) * stride;
    //                 float y_center = (feat_ptr[1] + h) * stride;
    //                 float w = exp(feat_ptr[2]) * stride;
    //                 float h = exp(feat_ptr[3]) * stride;
    //                 float x0 = x_center - w * 0.5f;
    //                 float y0 = y_center - h * 0.5f;

    //                 Object obj;
    //                 obj.rect.x = x0;
    //                 obj.rect.y = y0;
    //                 obj.rect.width = w;
    //                 obj.rect.height = h;
    //                 obj.label = class_index;
    //                 obj.prob = box_prob;

    //                 objects.push_back(obj);
    //             }

    //             feat_ptr += 85;
    //         }
    //     }
    // }

    // static void generate_proposals_yolov7(int stride, const float* feat, float prob_threshold, std::vector<Object>& objects,
    //                                       int letterbox_cols, int letterbox_rows, const float* anchors)
    // {
    //     int feat_w = letterbox_cols / stride;
    //     int feat_h = letterbox_rows / stride;
    //     int cls_num = 80;

    //     auto feat_ptr = feat;

    //     for (int h = 0; h <= feat_h - 1; h++)
    //     {
    //         for (int w = 0; w <= feat_w - 1; w++)
    //         {
    //             for (int a_index = 0; a_index < 3; ++a_index)
    //             {
    //                 float box_objectness = feat_ptr[4];
    //                 if (box_objectness < prob_threshold)
    //                 {
    //                     feat_ptr += 85;
    //                     continue;
    //                 }

    //                 //process cls score
    //                 int class_index = 0;
    //                 float class_score = -FLT_MAX;
    //                 for (int s = 0; s <= cls_num - 1; s++)
    //                 {
    //                     float score = feat_ptr[s + 5];
    //                     if (score > class_score)
    //                     {
    //                         class_index = s;
    //                         class_score = score;
    //                     }
    //                 }

    //                 float box_prob = box_objectness * class_score;

    //                 if (box_prob > prob_threshold)
    //                 {
    //                     float x_center = (feat_ptr[0] * 2 - 0.5f + (float)w) * (float)stride;
    //                     float y_center = (feat_ptr[1] * 2 - 0.5f + (float)h) * (float)stride;
    //                     float box_w = (feat_ptr[2] * 2) * (feat_ptr[2] * 2) * anchors[a_index * 2];
    //                     float box_h = (feat_ptr[3] * 2) * (feat_ptr[3] * 2) * anchors[a_index * 2 + 1];
    //                     float x0 = x_center - box_w * 0.5f;
    //                     float y0 = y_center - box_h * 0.5f;

    //                     Object obj;
    //                     obj.rect.x = x0;
    //                     obj.rect.y = y0;
    //                     obj.rect.width = box_w;
    //                     obj.rect.height = box_h;
    //                     obj.label = class_index;
    //                     obj.prob = box_prob;

    //                     objects.push_back(obj);
    //                 }

    //                 feat_ptr += 85;
    //             }
    //         }
    //     }
    // }

    void generate_proposals_255(int stride, const float* feat, float prob_threshold, std::vector<Object>& objects,
                                       int letterbox_cols, int letterbox_rows, const float* anchors, float prob_threshold_unsigmoid)
    {
        int anchor_num = 3;
        int feat_w = letterbox_cols / stride;
        int feat_h = letterbox_rows / stride;
        int cls_num = 80;
        int anchor_group;
        if (stride == 8)
            anchor_group = 1;
        if (stride == 16)
            anchor_group = 2;
        if (stride == 32)
            anchor_group = 3;

        auto feature_ptr = feat;

        for (int h = 0; h <= feat_h - 1; h++)
        {
            for (int w = 0; w <= feat_w - 1; w++)
            {
                for (int a = 0; a <= anchor_num - 1; a++)
                {
                    if (feature_ptr[4] < prob_threshold_unsigmoid)
                    {
                        feature_ptr += (cls_num + 5);
                        continue;
                    }

                    //process cls score
                    int class_index = 0;
                    float class_score = -FLT_MAX;
                    for (int s = 0; s <= cls_num - 1; s++)
                    {
                        float score = feature_ptr[s + 5];
                        if (score > class_score)
                        {
                            class_index = s;
                            class_score = score;
                        }
                    }
                    //process box score
                    float box_score = feature_ptr[4];
                    float final_score = sigmoid(box_score) * sigmoid(class_score);

                    if (final_score >= prob_threshold)
                    {
                        float dx = sigmoid(feature_ptr[0]);
                        float dy = sigmoid(feature_ptr[1]);
                        float dw = sigmoid(feature_ptr[2]);
                        float dh = sigmoid(feature_ptr[3]);
                        float pred_cx = (dx * 2.0f - 0.5f + w) * stride;
                        float pred_cy = (dy * 2.0f - 0.5f + h) * stride;
                        float anchor_w = anchors[(anchor_group - 1) * 6 + a * 2 + 0];
                        float anchor_h = anchors[(anchor_group - 1) * 6 + a * 2 + 1];
                        float pred_w = dw * dw * 4.0f * anchor_w;
                        float pred_h = dh * dh * 4.0f * anchor_h;
                        float x0 = pred_cx - pred_w * 0.5f;
                        float y0 = pred_cy - pred_h * 0.5f;
                        float x1 = pred_cx + pred_w * 0.5f;
                        float y1 = pred_cy + pred_h * 0.5f;

                        Object obj;
                        obj.rect.x = x0;
                        obj.rect.y = y0;
                        obj.rect.width = x1 - x0;
                        obj.rect.height = y1 - y0;
                        obj.label = class_index;
                        obj.prob = final_score;
                        objects.push_back(obj);
                    }

                    feature_ptr += (cls_num + 5);
                }
            }
        }
    }

    // static void generate_proposals(int stride, const float* feat, float prob_threshold, std::vector<Object>& objects,
    //                                int letterbox_cols, int letterbox_rows, const float* anchors)
    // {
    //     int anchor_num = 3;
    //     int feat_w = letterbox_cols / stride;
    //     int feat_h = letterbox_rows / stride;
    //     int cls_num = 80;
    //     int anchor_group;
    //     if (stride == 8)
    //         anchor_group = 1;
    //     if (stride == 16)
    //         anchor_group = 2;
    //     if (stride == 32)
    //         anchor_group = 3;

    //     int w_stride = (cls_num + 5);
    //     int h_stride = feat_w * w_stride;
    //     int a_stride = feat_h * h_stride;

    //     for (int h = 0; h <= feat_h - 1; h++)
    //     {
    //         for (int w = 0; w <= feat_w - 1; w++)
    //         {
    //             for (int a = 0; a <= anchor_num - 1; a++)
    //             {
    //                 //process cls score
    //                 int class_index = 0;
    //                 float class_score = -FLT_MAX;
    //                 int offset = a * a_stride + h * h_stride + w * w_stride;
    //                 for (int s = 0; s <= cls_num - 1; s++)
    //                 {
    //                     float score = feat[offset + s + 5];
    //                     if (score > class_score)
    //                     {
    //                         class_index = s;
    //                         class_score = score;
    //                     }
    //                 }
    //                 //process box score
    //                 float box_score = feat[offset + 4];
    //                 float final_score = sigmoid(box_score) * sigmoid(class_score);

    //                 if (final_score >= prob_threshold)
    //                 {
    //                     int loc_idx = offset;
    //                     float dx = sigmoid(feat[loc_idx + 0]);
    //                     float dy = sigmoid(feat[loc_idx + 1]);
    //                     float dw = sigmoid(feat[loc_idx + 2]);
    //                     float dh = sigmoid(feat[loc_idx + 3]);
    //                     float pred_cx = (dx * 2.0f - 0.5f + w) * stride;
    //                     float pred_cy = (dy * 2.0f - 0.5f + h) * stride;
    //                     float anchor_w = anchors[(anchor_group - 1) * 6 + a * 2 + 0];
    //                     float anchor_h = anchors[(anchor_group - 1) * 6 + a * 2 + 1];
    //                     float pred_w = dw * dw * 4.0f * anchor_w;
    //                     float pred_h = dh * dh * 4.0f * anchor_h;
    //                     float x0 = pred_cx - pred_w * 0.5f;
    //                     float y0 = pred_cy - pred_h * 0.5f;
    //                     float x1 = pred_cx + pred_w * 0.5f;
    //                     float y1 = pred_cy + pred_h * 0.5f;

    //                     Object obj;
    //                     obj.rect.x = x0;
    //                     obj.rect.y = y0;
    //                     obj.rect.width = x1 - x0;
    //                     obj.rect.height = y1 - y0;
    //                     obj.label = class_index;
    //                     obj.prob = final_score;
    //                     objects.push_back(obj);
    //                 }
    //             }
    //         }
    //     }
    // }

    void draw_objects(const cv::Mat& bgr, const std::vector<Object>& objects, const char** class_names, const char* output_name)
    {
        cv::Mat image = bgr.clone();

        for (size_t i = 0; i < objects.size(); i++)
        {
            const Object& obj = objects[i];

            fprintf(stdout, "%2d: %3.0f%%, [%4.0f, %4.0f, %4.0f, %4.0f], %s\n", obj.label, obj.prob * 100, obj.rect.x,
                    obj.rect.y, obj.rect.x + obj.rect.width, obj.rect.y + obj.rect.height, class_names[obj.label]);

            cv::rectangle(image, obj.rect, cv::Scalar(255, 0, 0));

            char text[256];
            sprintf(text, "%s %.1f%%", class_names[obj.label], obj.prob * 100);

            int baseLine = 0;
            cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

            int x = obj.rect.x;
            int y = obj.rect.y - label_size.height - baseLine;
            if (y < 0)
                y = 0;
            if (x + label_size.width > image.cols)
                x = image.cols - label_size.width;

            cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                          cv::Scalar(255, 255, 255), -1);

            cv::putText(image, text, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                        cv::Scalar(0, 0, 0));
        }

        cv::imwrite(std::string(output_name) + ".jpg", image);
    }

    void reverse_letterbox(std::vector<Object>& proposal, std::vector<Object>& objects, int letterbox_rows, int letterbox_cols, int src_rows, int src_cols)
    {
        float scale_letterbox;
        int resize_rows;
        int resize_cols;
        if ((letterbox_rows * 1.0 / src_rows) < (letterbox_cols * 1.0 / src_cols))
        {
            scale_letterbox = letterbox_rows * 1.0 / src_rows;
        }
        else
        {
            scale_letterbox = letterbox_cols * 1.0 / src_cols;
        }
        resize_cols = int(scale_letterbox * src_cols);
        resize_rows = int(scale_letterbox * src_rows);

        int tmp_h = (letterbox_rows - resize_rows) / 2;
        int tmp_w = (letterbox_cols - resize_cols) / 2;

        float ratio_x = (float)src_rows / resize_rows;
        float ratio_y = (float)src_cols / resize_cols;

        int count = proposal.size();

        objects.resize(count);
        for (int i = 0; i < count; i++)
        {
            objects[i] = proposal[i];
            float x0 = (objects[i].rect.x);
            float y0 = (objects[i].rect.y);
            float x1 = (objects[i].rect.x + objects[i].rect.width);
            float y1 = (objects[i].rect.y + objects[i].rect.height);

            x0 = (x0 - tmp_w) * ratio_x;
            y0 = (y0 - tmp_h) * ratio_y;
            x1 = (x1 - tmp_w) * ratio_x;
            y1 = (y1 - tmp_h) * ratio_y;

            x0 = std::max(std::min(x0, (float)(src_cols - 1)), 0.f);
            y0 = std::max(std::min(y0, (float)(src_rows - 1)), 0.f);
            x1 = std::max(std::min(x1, (float)(src_cols - 1)), 0.f);
            y1 = std::max(std::min(y1, (float)(src_rows - 1)), 0.f);

            objects[i].rect.x = x0;
            objects[i].rect.y = y0;
            objects[i].rect.width = x1 - x0;
            objects[i].rect.height = y1 - y0;
        }
    }

    void get_out_bbox_no_letterbox(std::vector<Object>& proposals, std::vector<Object>& objects, const float nms_threshold, int model_h, int model_w, int src_rows, int src_cols)
    {
        qsort_descent_inplace(proposals);
        std::vector<int> picked;
        nms_sorted_bboxes(proposals, picked, nms_threshold);

        /* yolov5 draw the result */
        float ratio_x = (float)src_cols / (float)model_w;
        float ratio_y = (float)src_rows / (float)model_h;

        int count = picked.size();

        objects.resize(count);
        for (int i = 0; i < count; i++)
        {
            objects[i] = proposals[picked[i]];
            float x0 = (objects[i].rect.x);
            float y0 = (objects[i].rect.y);
            float x1 = (objects[i].rect.x + objects[i].rect.width);
            float y1 = (objects[i].rect.y + objects[i].rect.height);

            x0 = (x0)*ratio_x;
            y0 = (y0)*ratio_y;
            x1 = (x1)*ratio_x;
            y1 = (y1)*ratio_y;

            x0 = std::max(std::min(x0, (float)(src_cols - 1)), 0.f);
            y0 = std::max(std::min(y0, (float)(src_rows - 1)), 0.f);
            x1 = std::max(std::min(x1, (float)(src_cols - 1)), 0.f);
            y1 = std::max(std::min(y1, (float)(src_rows - 1)), 0.f);

            objects[i].rect.x = x0;
            objects[i].rect.y = y0;
            objects[i].rect.width = x1 - x0;
            objects[i].rect.height = y1 - y0;
        }
    }

    void get_out_bbox(std::vector<Object>& proposals, std::vector<Object>& objects, const float nms_threshold, int letterbox_rows, int letterbox_cols, int src_rows, int src_cols)
    {
        qsort_descent_inplace(proposals);
        std::vector<int> picked;
        nms_sorted_bboxes(proposals, picked, nms_threshold);

        /* yolov5 draw the result */
        float scale_letterbox;
        int resize_rows;
        int resize_cols;
        if ((letterbox_rows * 1.0 / src_rows) < (letterbox_cols * 1.0 / src_cols))
        {
            scale_letterbox = letterbox_rows * 1.0 / src_rows;
        }
        else
        {
            scale_letterbox = letterbox_cols * 1.0 / src_cols;
        }
        resize_cols = int(scale_letterbox * src_cols);
        resize_rows = int(scale_letterbox * src_rows);

        int tmp_h = (letterbox_rows - resize_rows) / 2;
        int tmp_w = (letterbox_cols - resize_cols) / 2;

        float ratio_x = (float)src_rows / resize_rows;
        float ratio_y = (float)src_cols / resize_cols;

        int count = picked.size();

        objects.resize(count);
        for (int i = 0; i < count; i++)
        {
            objects[i] = proposals[picked[i]];
            float x0 = (objects[i].rect.x);
            float y0 = (objects[i].rect.y);
            float x1 = (objects[i].rect.x + objects[i].rect.width);
            float y1 = (objects[i].rect.y + objects[i].rect.height);

            x0 = (x0 - tmp_w) * ratio_x;
            y0 = (y0 - tmp_h) * ratio_y;
            x1 = (x1 - tmp_w) * ratio_x;
            y1 = (y1 - tmp_h) * ratio_y;

            x0 = std::max(std::min(x0, (float)(src_cols - 1)), 0.f);
            y0 = std::max(std::min(y0, (float)(src_rows - 1)), 0.f);
            x1 = std::max(std::min(x1, (float)(src_cols - 1)), 0.f);
            y1 = std::max(std::min(y1, (float)(src_rows - 1)), 0.f);

            objects[i].rect.x = x0;
            objects[i].rect.y = y0;
            objects[i].rect.width = x1 - x0;
            objects[i].rect.height = y1 - y0;
        }
    }

} // namespace detection
