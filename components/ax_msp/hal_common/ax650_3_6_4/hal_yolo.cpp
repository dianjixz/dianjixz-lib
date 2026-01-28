#include "hal_yolo.hpp"

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




class AX650YoloHandle : public posthandle {
private:
    hal_yolo& father_dev;

    // AX_ENGINE_HANDLE handle;

public:
    AX650YoloHandle(hal_yolo& dev, std::shared_ptr<modelhandle> handle) : father_dev(dev)
    {
        model_handle = handle;
        _width       = 0;
        _height      = 0;
    };

    virtual void PostComputation(void* data, void* info) override
    {
        AX_ENGINE_IO_T* io_data      = (AX_ENGINE_IO_T*)data;
        AX_ENGINE_IO_INFO_T* io_info = (AX_ENGINE_IO_INFO_T*)info;
        std::vector<detection::Object> proposals;
        objects.clear();
        float prob_threshold_u_sigmoid = -1.0f * (float)std::log((1.0f / PROB_THRESHOLD) - 1.0f);
        // timer timer_postprocess;
        for (uint32_t i = 0; i < io_info->nOutputSize; ++i) {
            auto& output = io_data->pOutputs[i];
            // auto& info = io_info->pOutputs[i];
            auto ptr       = (float*)output.pVirAddr;
            int32_t stride = (1 << i) * 8;
            std::vector<float> anchors;
            for (auto it : model_handle->_anchors) {
                anchors.push_back((float)it);
            }
            detection::generate_proposals_255(stride, ptr, PROB_THRESHOLD, proposals, _width, _height, anchors.data(),
                                              prob_threshold_u_sigmoid);
        }
        detection::get_out_bbox(proposals, objects, NMS_THRESHOLD, _height, _width, model_handle->_in_width,
                                model_handle->_in_height);
    }
    virtual void yolo_computation(cv::Mat& image) override
    {
    }
    virtual std::vector<detection::Object> yolo_computation_result(cv::Mat& image) override
    {
        _width  = image.cols;
        _height = image.rows;
        /* letterbox process to support different letterbox size */
        float scale_letterbox;
        int resize_rows;
        int resize_cols;
        if ((model_handle->_in_width * 1.0 / image.rows) < (model_handle->_in_height * 1.0 / image.cols)) {
            scale_letterbox = (float)model_handle->_in_width * 1.0f / (float)image.rows;
        } else {
            scale_letterbox = (float)model_handle->_in_height * 1.0f / (float)image.cols;
        }
        resize_cols = int(scale_letterbox * (float)image.cols);
        resize_rows = int(scale_letterbox * (float)image.rows);
        std::vector<uint8_t> computation_data;
        computation_data.resize(model_handle->_in_width * model_handle->_in_height * 3);
        cv::Mat img_new(model_handle->_in_width, model_handle->_in_height, CV_8UC3, computation_data.data());

        cv::resize(image, image, cv::Size(resize_cols, resize_rows));

        int top   = (model_handle->_in_width - resize_rows) / 2;
        int bot   = (model_handle->_in_width - resize_rows + 1) / 2;
        int left  = (model_handle->_in_height - resize_cols) / 2;
        int right = (model_handle->_in_height - resize_cols + 1) / 2;

        // Letterbox filling
        cv::copyMakeBorder(image, img_new, top, bot, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        model_handle->ForwardComputation(computation_data);
        PostComputation(model_handle->model_data, model_handle->model_info);
        if (yolo_result_put_func) {
            for (auto it : objects) {
                yolo_result_put_func(it);
            }
        }
        return std::move(objects);
    }
    virtual int work() override
    {
        return 0;
    }
    virtual void destroy() override
    {
        if (model_handle_is_self) {
            model_handle->destroy();
        }
        model_handle_is_self = 0;
    };
    ~AX650YoloHandle()
    {
        destroy();
        father_dev.destroy(_handle_index);
    };
};


posthandle::posthandle()
{

}
posthandle::~posthandle()
{

}

hal_yolo::hal_yolo(hal_device& device) : _device(device)
{
}

std::shared_ptr<posthandle> hal_yolo::creat(std::shared_ptr<modelhandle> handle)
{
    auto post_handle                                = std::make_shared<AX650YoloHandle>(*this, handle);
    std::weak_ptr<AX650YoloHandle> weak_post_handle = post_handle;
    handle->connect([weak_post_handle](void* data, void* info) {
        auto post_handle = weak_post_handle.lock();
        if (post_handle) {
            post_handle->PostComputation(data, info);
        }
    });
    post_handle->_handle_index = _handle_index++;
    encoder_list.push_back(post_handle);
    return post_handle;
}

std::shared_ptr<posthandle> hal_yolo::creat(const std::string& model_path)
{
    if (engine == nullptr) {
        engine = std::make_shared<hal_engine>(_device);
    }
    auto model_handle                               = engine->creat(model_path);
    auto post_handle                                = std::make_shared<AX650YoloHandle>(*this, model_handle);
    post_handle->model_handle_is_self = 1;
    post_handle->_handle_index        = _handle_index++;
    encoder_list.push_back(post_handle);
    return post_handle;
}

hal_yolo::~hal_yolo()
{
    for (auto it = encoder_list.begin(); it != encoder_list.end(); ++it) {
        (*it)->destroy();
    }
    encoder_list.clear();
};
