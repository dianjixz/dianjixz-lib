#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "d_hal_type.h"
#include <linux/videodev2.h>
#include "../../utilities/include/linalg.h"
namespace  d_hal
{
    class d_hal_nn;

    class d_hal_nn_yolo
    {
    protected:
        
    private:
        
    public:
        d_hal_nn * _nn;
        d_hal_nn_yolo(){};
        virtual int init() = 0;
        virtual int deinit() = 0;
        // d_hal_nn_yolo(d_hal_nn* nn):_nn(nn) {};
        virtual int set_class(const std::vector<std::string> &class_name) = 0;
        virtual int set_anchors(const std::vector<float> &anchors) = 0;
        virtual int set_image_size(int width, int height, int type) = 0;
        virtual int encode_image(void *img, int width, int height, int type,int yolo_type) = 0;
        virtual int infer_with_decode_object(int yolo_type, float prob_threshold, float nms_threshold, std::shared_ptr<void> &result) = 0;
        virtual ~d_hal_nn_yolo();
    };
    





    class d_hal_nn
    {
    private:
        /* data */
    public:
        D_HAL_NN_TYPE_E _type;
    public:
        d_hal_nn() {};
        virtual int init() = 0;
        virtual int deinit() = 0;
        virtual int load_model(const std::string& model) = 0;
        // virtual int prepare_image(const std::vector<char>& in, int w, int h, int fmt, const std::vector<char>& out) = 0;
        virtual int request_infer_mem_size() = 0;
        virtual int request_infer_mem(void** mem) = 0;
        virtual int infer(const void* in, std::shared_ptr<std::vector<void*>> &io) = 0;

        virtual std::shared_ptr<d_hal_nn_yolo> yolo(int yolo_type) = 0;

        // yolo组合接口
        virtual int yolo_set_class(const std::vector<std::string> &class_name) = 0;
        virtual int yolo_set_anchors(const std::vector<float> &anchors) = 0;
        virtual int yolo_set_image_size(int width, int height, int type) = 0;
        virtual int yolo_encode_image(void *img, int width, int height, int type,int yolo_type) = 0;
        virtual int yolo_infer_with_decode_object(int yolo_type, float prob_threshold, float nms_threshold, std::shared_ptr<void> &result) = 0;


        virtual std::shared_ptr<void> get_param(const std::string& param_name) = 0;
        virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) = 0;
        D_HAL_NN_TYPE_E get_type() { return _type; };
        virtual ~d_hal_nn(){};
    };
    std::shared_ptr<d_hal_nn> create_d_hal_nn(D_HAL_NN_TYPE_E type);
} // namespace  d_hal

