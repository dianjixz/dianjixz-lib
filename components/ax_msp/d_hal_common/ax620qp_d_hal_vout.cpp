#include "d_hal_vout_mipi.hpp"
#include "d_hal_vout.hpp"
#include "ax_vo_api.h"





const char* AX_CMM_SESSION_NAME = "ax_vo";

namespace d_hal {





class d_hal_vout_mipi : public d_hal_vout {
private:
    static int hardware_initialized;
    int _vo_opened;
private:
    static int hardware_init()
    {
        if (hardware_initialized == 0) {
            AX_VO_Init();
        }
        hardware_initialized++;
    }
    static int hardware_deinit()
    {
        hardware_initialized--;
        if (hardware_initialized == 0) {
            AX_VO_Deinit();
        }
    }

public:
    AX_VO_PUB_ATTR_T _PubAttr;
    VO_DEV _VoDev;
    VO_LAYER _VoLayer;
    AX_VO_VIDEO_LAYER_ATTR_T _LayerAttr[16];

    AX_VO_PUB_ATTR_T get_vo_pub_attr_default(){
        AX_VO_PUB_ATTR_T pubAttr;
        pubAttr.enMode = AX_VO_MODE_OFFLINE;
        pubAttr.enIntfType = AX_VO_INTF_DSI;
        pubAttr.enIntfFmt = AX_VO_OUT_FMT_RGB888;
        pubAttr.enIntfSync = AX_VO_OUTPUT_720x1280_60;
        // todo 通过设备树配置分辨率参数
        // pubAttr.enIntfSync = AX_VO_OUTPUT_USER;
        // pubAttr.stSyncInfo.u16Vact = 1280;
        // pubAttr.stSyncInfo.u16Vbb = 12;
        // pubAttr.stSyncInfo.u16Vfp = 12;
        // pubAttr.stSyncInfo.u16Vsync = 1;
        // pubAttr.stSyncInfo.u16Hact = 720;
        // pubAttr.stSyncInfo.u16Hbb = 12;
        // pubAttr.stSyncInfo.u16Hfp = 12;
        // pubAttr.stSyncInfo.u16Hsync = 1;
        // pubAttr.stSyncInfo.u32Vfreq = 60;
        return pubAttr;
    }

    AX_VO_VIDEO_LAYER_ATTR_T get_vo_layer_attr_default(){
        AX_VO_VIDEO_LAYER_ATTR_T layerAttr = {0};
        layerAttr.stImageSize.u32Width  = 720;
        layerAttr.stImageSize.u32Height = 1280;
        layerAttr.enPixFmt = AX_FORMAT_RGB888;
        layerAttr.f32FrmRate = 60;
        // layerAttr.u32PrimaryChnId = 
        // u32FifoDepth
        // u32BkClr
        // layerAttr.u32DispatchMode
        // enWBMode
        // u32InplaceChnId
        // u32PoolId
        // enPartMode
        // enBlendMode
        // u32EngineId
        // u32EngineId
        // u32Toleration
        // f32FrmRate





    }





public:
    d_hal_vout_mipi(D_HAL_VOUT_TYPE_E type)
    {
        hardware_init();
        _type    = type;
        _vo_opened = 0;
        init();
    }
    virtual int init() override
    {
        

        return 0;
    }
    virtual int deinit() override
    {
        if(_vo_opened)
        {
            // 解绑vo和mipi
            // 关闭 vo
            // 释放物理内存
            _vo_opened = 0;
        }
        
        return 0;
    }
    virtual int open(const std::string& device) override
    {
        int ret  = 0;
        // vo 启动
        _VoDev = std::stoi(device);
        _PubAttr = get_vo_pub_attr_default();
        AX_VO_SetPubAttr(_VoDev, &_PubAttr);
        AX_VO_Enable(_VoDev);
        // 创建图层
        // AX_VO_CreateVideoLayer(&_VoLayer);
        // AX_VO_SetVideoLayerAttr(_VoLayer, &_LayerAttr);
        // AX_VO_BindVideoLayer(_VoLayer, _VoDev);
        // AX_VO_EnableVideoLayer(_VoLayer);
        return ret;
    }
    virtual int creat_layer() override
    {
        AX_VO_CreateVideoLayer(&_VoLayer);

    }

    virtual int get_vout(int layer, void** data) override
    {
        return 0;
    }
    virtual int set_vout(int layer, void* data) override
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
    //         _thread = std::make_shared<std::thread>(&d_hal_vout_mipi::async_get_vi_thread, this);
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
    virtual ~d_hal_vout_mipi() override
    {
        deinit();
        hardware_deinit();
    };
};







int d_hal_vout_mipi::hardware_initialized = 0;








std::shared_ptr<d_hal_vout> create_d_hal_vout(D_HAL_VOUT_TYPE_E type)
{
    if (type == D_HAL_VOUT_FBDEV) {
        return std::make_shared<d_hal_vout_fbdev>(D_HAL_VOUT_FBDEV);
    } 
    else if (type == D_HAL_VOUT_AXERA_MIPI) {
        return std::make_shared<d_hal_vout_mipi>(D_HAL_VOUT_AXERA_MIPI);
    } 
    else {
        return nullptr;
    }
}
}  // namespace  d_hal
