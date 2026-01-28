#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "d_hal_type.h"
namespace  d_hal
{









    class d_hal_hardware
    {
    private:
        /* data */
    public:
        D_HAL_HARDWARE_TYPE_E _type;
    public:
        d_hal_hardware(D_HAL_HARDWARE_TYPE_E type) {_type = type; init();};
        virtual int init() = 0;
        virtual int deinit() = 0;
        virtual std::shared_ptr<void> get_param(const std::string& param_name) = 0;
        virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) = 0;
        D_HAL_HARDWARE_TYPE_E get_type() { return _type; };
        ~d_hal_hardware(){deinit();};
    };
    std::shared_ptr<d_hal_hardware> create_d_hal_hardware(D_HAL_HARDWARE_TYPE_E type)
    {
        
    }
} // namespace  d_hal

