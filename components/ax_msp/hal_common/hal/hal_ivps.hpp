#include "hal_device.h"
#include <memory>
#include <iostream>
#include <string>

#include "device_interface.h"


class ivps_group {
private:
    /* data */
public:
    int _in_width;
    int _in_height;
    int _in_format;
    int _fps;
    typedef std::function<void(AX_U64, void*, int, int)> frame_put_func_t;
    // void on_fream(AX_U64 phy, void *Vir, int size, int format)
    frame_put_func_t frame_put_func;
    int id_info[3];
    int _handle_index;
public:
    ivps_group(/* args */);

    virtual int set_encode_frame(int width, int height, int format, bool malloc_in = false, bool malloc_out = false)
    {
        int ret;
        _in_width  = width;
        _in_height = height;
        _in_format = format;
        if (malloc_in) {
            ret = malloc_encode_in_frame();
            if (ret) {
                return ret;
            }
        }
        if (malloc_out) {
            ret = malloc_encode_out_frame();
            if (ret) {
                free_encode_in_frame();
                return ret;
            }
        }
        return 0;  // 修复：添加返回值
    };
    virtual int malloc_encode_in_frame()  = 0;
    virtual int malloc_encode_out_frame() = 0;
    virtual void free_encode_in_frame()   = 0;
    virtual void free_encode_out_frame()  = 0;
    virtual int malloc_encode_chn()
    {
        return -1;
    };
    virtual void free_encode_chn() = 0;
    virtual void set_encode_fps(int fps)
    {
        _fps = fps;
    };
    virtual int encode_frame(int qfactor = 35)      = 0;
    virtual int encode_frame_once(int qfactor = 35) = 0;
    void connect(frame_put_func_t func)
    {
        frame_put_func = func;
    }
    virtual int work() = 0;  // 修复：应该是纯虚函数或提供实现
    template <class T>
    std::shared_ptr<T> link_id();
    virtual ~ivps_group();  // 修复：虚析构函数
};

class hal_ivps {
private:
    std::shared_ptr<SysGuard> sys;
    std::list<std::shared_ptr<ivps_group>> encoder_list;
    // std::list<std::pair<std::shared_ptr<ivps_group>, int>> encoder_list;
    int encoder_count;
public:
    hal_ivps(const hal_device& device);
    std::shared_ptr<ivps_group> creat(int type);
    void destroy(const std::shared_ptr<ivps_group>& encoder)
    {
        for (auto it = encoder_list.begin(); it != encoder_list.end();) {
            if (it->get() == encoder.get()) {
                it = encoder_list.erase(it);
                break;
            } else {
                ++it;
            }
        }
    }
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
    }
    ~hal_ivps();
};