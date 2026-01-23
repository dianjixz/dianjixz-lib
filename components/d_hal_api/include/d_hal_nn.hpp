#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "d_hal_type.h"
namespace  d_hal
{
    class d_hal_nn
    {
    private:
        /* data */
    public:
        D_HAL_NN_TYPE_E _type;
    public:
        d_hal_nn(D_HAL_NN_TYPE_E type) {_type = type; init();};
        virtual int init() = 0;
        virtual int deinit() = 0;
        virtual int load_model(const std::string& model) = 0;
        
        virtual int infer(const std::vector<char>& in, std::vector<char>& out) = 0;
        virtual std::shared_ptr<void> get_param(const std::string& param_name) = 0;
        virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) = 0;
        D_HAL_NN_TYPE_E get_type() { return _type; };
        ~d_hal_nn(){deinit();};
    };
    std::shared_ptr<d_hal_nn> create_d_hal_nn(D_HAL_NN_TYPE_E type);
} // namespace  d_hal

