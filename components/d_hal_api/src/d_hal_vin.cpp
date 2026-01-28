#include "d_hal_vin_v4l2.hpp"

namespace d_hal {
std::shared_ptr<d_hal_vin> create_d_hal_vin(D_HAL_VIN_TYPE_E type)
{
    if (type == D_HAL_VIN_V4L2) {
        return std::make_shared<d_hal_v4l2>(D_HAL_VIN_V4L2);
    } else {
        return nullptr;
    }
}
}  // namespace  d_hal
