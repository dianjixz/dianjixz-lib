#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include "d_hal_type.h"
namespace  d_hal
{
    // typedef std::function<int(void *data, int size)> d_hal_vout_callback_t;
    class d_hal_vout
    {
    private:
        /* data */
    public:
        D_HAL_VOUT_TYPE_E _type;
        // d_hal_vout_callback_t _callback;
        std::atomic<int> _running;
        std::shared_ptr<std::thread> _thread;
    public:
        d_hal_vout(){};
        virtual int init() = 0;
        virtual int deinit() = 0;
        virtual int open(const std::string& device) = 0;
        virtual int get_vo_info(int * width, int *height, int *type, int *fps) = 0;

        // virtual int get_vi(std::vector<char>& out) = 0;
        virtual int get_vout(void **data) = 0;
        virtual int set_vout(void *data) = 0;
        virtual int with_vout(std::function<int(int width, int height, int type, void *mem)> func) = 0;
        // virtual int connect(d_hal_vout_callback_t func) = 0;
        virtual std::shared_ptr<void> get_param(const std::string& param_name) = 0;
        virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) = 0;
        D_HAL_VOUT_TYPE_E get_type() { return _type; };
        virtual ~d_hal_vout() {};
    };
    std::shared_ptr<d_hal_vout> create_d_hal_vout(D_HAL_VOUT_TYPE_E type);
} // namespace  d_hal

