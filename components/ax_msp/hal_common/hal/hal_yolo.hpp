#pragma once
#include <iostream>
#include "hal_device.h"
#include "hal_engine.hpp"
#include <vector>

class posthandle {
private:
    /* data */
public:
    std::shared_ptr<modelhandle> model_handle;
    int model_handle_is_self = 0;
    int _width;
    int _height;
    typedef std::function<void(detection::Object)> yolo_result_put_func_t;
    // void on_result(const AX_ENGINE_IO_T* data, const AX_ENGINE_IO_INFO_T* info)
    yolo_result_put_func_t yolo_result_put_func;
    int id_info[3];
    int _handle_index;
    float PROB_THRESHOLD = 0.45f;
    float NMS_THRESHOLD  = 0.45f;
    std::vector<detection::Object> objects;
public:
    posthandle(/* args */);
    virtual void PostComputation(void* data, void* info) = 0;

    
    virtual void yolo_computation(cv::Mat& image) = 0;
    virtual std::vector<detection::Object> yolo_computation_result(cv::Mat& image) = 0;

    void connect(yolo_result_put_func_t func)
    {
        yolo_result_put_func = func;
    }
    virtual int work() = 0;  // 修复：应该是纯虚函数或提供实现
    template <class T>
    std::shared_ptr<T> link_id();
    virtual void destroy() = 0;
    virtual ~posthandle();  // 修复：虚析构函数
};

class hal_yolo {
private:
    hal_device& _device;
    std::list<std::shared_ptr<posthandle>> encoder_list;
    int _handle_index;

public:
    std::shared_ptr<hal_engine> engine;
    hal_yolo(hal_device& device);
    std::shared_ptr<posthandle> creat(std::shared_ptr<modelhandle> handle);
    std::shared_ptr<posthandle> creat(const std::string& model_path);
    void destroy(const std::shared_ptr<posthandle>& encoder)
    {
        for (auto it = encoder_list.begin(); it != encoder_list.end();) {
            if (it->get() == encoder.get()) {
                it = encoder_list.erase(it);
                break;
            } else {
                ++it;
            }
        }
    };
    void destroy(int encoder)
    {
        for (auto it = encoder_list.begin(); it != encoder_list.end();) {
            if ((*it)->_handle_index == encoder) {
                it = encoder_list.erase(it);
                break;
            } else {
                ++it;
            }
        }
    };
    ~hal_yolo();
};
