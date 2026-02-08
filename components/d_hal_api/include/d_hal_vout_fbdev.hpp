#include "d_hal_vout.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <sys/select.h>
#include <map>
namespace d_hal {


class d_hal_vout_fbdev : public d_hal_vout {
private:


private:


public:
public:
    d_hal_vout_fbdev(D_HAL_VOUT_TYPE_E type)
    {
        _type    = type;
        init();
    }
    virtual int init() override
    {

        return 0;
    }
    virtual int deinit() override
    {

        return 0;
    }
    virtual int open(const std::string& device) override
    {
        int ret  = 0;
        return ret;
    }

    virtual int get_vout(void** data) override
    {
        return 0;
    }
    virtual int set_vout(void* data) override
    {
        int ret    = -1;
        return ret;
    }

    // void async_get_vi_thread()
    // {
    //     while (_running.load()) {
    //         void* data = nullptr;
    //         int ret    = get_vi(&data);
    //         if (ret > 0) {
    //             spinlock_guard lock(_callback_flag);
    //             if (_callback) {
    //                 _callback(data, ret);
    //             }
    //         }
    //     }
    // }

    // virtual int connect(d_hal_vin_callback_t func) override
    // {
    //     {
    //         spinlock_guard lock(_callback_flag);
    //         _callback = func;
    //     }
        
    //     if (_thread == nullptr) {
    //         _thread = std::make_shared<std::thread>(&d_hal_vout_fbdev::async_get_vi_thread, this);
    //     }
    //     return 0;
    // }
    virtual std::shared_ptr<void> get_param(const std::string& param_name) override
    {
        // std::map<std::string, std::function<std::shared_ptr<void>(void)>> params = {
        //     {"width", [this]() { return std::make_shared<int>(_fmt.fmt.pix.width); }},
        //     {"height", [this]() { return std::make_shared<int>(_fmt.fmt.pix.height); }},
        //     {"type", [this]() { return std::make_shared<int>(_fmt.fmt.pix.pixelformat); }},
        // };
        // return params[param_name]();
        return nullptr;
    }
    virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) override
    {
        // std::map<std::string, std::function<int(void)>> params = {
        //     {"fps",
        //      [this, value]() {
        //          int ret = 0;
        //          if (_v4l2_fd > 0) {
        //              _parm.type                                  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        //              _parm.parm.capture.timeperframe.numerator   = 1;
        //              _parm.parm.capture.timeperframe.denominator = *(std::static_pointer_cast<int>(value));
        //              ret                                         = ioctl(_v4l2_fd, VIDIOC_S_PARM, &_parm);
        //              if (ret < 0) {
        //                  perror("设置帧率失败");
        //              }
        //              return ret;
        //          }
        //          return -1;
        //      }},
        // };
        // return params[param_name]();
        return 0;
    }
    virtual ~d_hal_vout_fbdev() override
    {
        deinit();
    };
};
}  // namespace  d_hal
