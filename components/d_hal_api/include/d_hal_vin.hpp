#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "d_hal_type.h"
namespace  d_hal
{
    class d_hal_vin
    {
    private:
        /* data */
    public:
        D_HAL_VIN_TYPE_E _type;
    public:
        d_hal_vin(){};
        virtual int init() = 0;
        virtual int deinit() = 0;
        virtual int open(const std::string& device, int w, int h, int type) = 0;
        virtual int get_vi(std::vector<char>& out) = 0;
        virtual int get_vi(void **data) = 0;
        virtual int connect(std::function<int(const std::vector<char>&)> func) = 0;
        virtual std::shared_ptr<void> get_param(const std::string& param_name) = 0;
        virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) = 0;
        D_HAL_VIN_TYPE_E get_type() { return _type; };
        virtual ~d_hal_vin() {};
    };
    std::shared_ptr<d_hal_vin> create_d_hal_vin(D_HAL_VIN_TYPE_E type);
} // namespace  d_hal

