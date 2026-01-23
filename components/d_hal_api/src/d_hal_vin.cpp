#include "d_hal_vin.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <map>
namespace d_hal {
typedef struct {
    void* start;
    size_t length;
} buffer_t;
class d_hal_v4l2 : public d_hal_vin {
private:
    int _v4l2_fd;
    struct v4l2_format _fmt;
    struct v4l2_streamparm _parm;
    struct v4l2_requestbuffers _req;
    buffer_t* _buffers;
    struct v4l2_buffer _buf;

public:
public:
    d_hal_v4l2(D_HAL_VIN_TYPE_E type)
    {
        _type    = type;
        _v4l2_fd = 0;
        _buffers = nullptr;
        init();
    }
    virtual int init() override
    {
        memset(&_fmt, 0, sizeof(_fmt));
        memset(&_parm, 0, sizeof(_parm));
        memset(&_req, 0, sizeof(_req));
        memset(&_buf, 0, sizeof(_buf));
        _buf.index         = -1;
        _buf.type          = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory        = V4L2_MEMORY_MMAP;
        _fmt.type          = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
        _req.count         = 4;
        _req.type          = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _req.memory        = V4L2_MEMORY_MMAP;
        _buffers           = (buffer_t*)malloc(sizeof(buffer_t) * _req.count);
        return 0;
    }
    virtual int deinit() override
    {
        if (_v4l2_fd > 0) {
            enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (ioctl(_v4l2_fd, VIDIOC_STREAMOFF, &type) < 0) {
                perror("停止采集失败");
            }
            for (int i = 0; i < _req.count; i++) {
                munmap(_buffers[i].start, _buffers[i].length);
            }
            close(_v4l2_fd);
            _v4l2_fd = 0;
            printf("设备已关闭\n");
        }
        free(_buffers);
        return 0;
    }
    virtual int open(const std::string& device, int w, int h, int type) override
    {
        int ret  = 0;
        _v4l2_fd = ::open(device.c_str(), O_RDWR | O_NONBLOCK);
        if (_v4l2_fd < 0) {
            perror("打开设备失败");
            return -1;
        }
        _fmt.fmt.pix.width       = w;
        _fmt.fmt.pix.height      = h;
        _fmt.fmt.pix.pixelformat = type;
        if (ioctl(_v4l2_fd, VIDIOC_S_FMT, &_fmt) < 0) {
            perror("设置格式失败");
            ret = -2;
        }
        if (ioctl(_v4l2_fd, VIDIOC_G_FMT, &_fmt) < 0) {
            perror("获取格式失败");
            ret = -3;
        }
        if (ioctl(_v4l2_fd, VIDIOC_REQBUFS, &_req) < 0) {
            perror("请求缓冲区失败");
            close(_v4l2_fd);
            _v4l2_fd = 0;
            return -1;
        }
        for (int i = 0; i < _req.count; i++) {
            struct v4l2_buffer buf = {0};
            buf.type               = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory             = V4L2_MEMORY_MMAP;
            buf.index              = i;

            // 查询缓冲区
            if (ioctl(_v4l2_fd, VIDIOC_QUERYBUF, &buf) < 0) {
                perror("查询缓冲区失败");
                close(_v4l2_fd);
                _v4l2_fd = 0;
                return -1;
            }

            // 映射到用户空间
            _buffers[i].length = buf.length;
            _buffers[i].start  = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _v4l2_fd, buf.m.offset);
            if (_buffers[i].start == MAP_FAILED) {
                perror("mmap 失败");
                close(_v4l2_fd);
                _v4l2_fd = 0;
                return -1;
            }

            // 缓冲区加入队列
            if (ioctl(_v4l2_fd, VIDIOC_QBUF, &buf) < 0) {
                perror("缓冲区入队失败");
                close(_v4l2_fd);
                _v4l2_fd = 0;
                return -1;
            }
        }
        enum v4l2_buf_type v4l2_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(_v4l2_fd, VIDIOC_STREAMON, &v4l2_type) < 0) {
            perror("启动采集失败");
            close(_v4l2_fd);
            _v4l2_fd = 0;
            return -1;
        }
        return ret;
    }
    // virtual int get_vi(std::vector<char>& out) override
    virtual int get_vi(void** data) override
    {
        if (_buf.index != -1) {
            if (ioctl(_v4l2_fd, VIDIOC_QBUF, &_buf) < 0) {
                perror("重新入队失败");
                return -1;
            }
        }
        memset(&_buf, 0, sizeof(_buf));
        _buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(_v4l2_fd, VIDIOC_DQBUF, &_buf) < 0) {
            _buf.index = -1;
            return -2;
        }
        *data = _buffers[_buf.index].start;
        return _buf.bytesused;
    }
    virtual int get_vi(std::vector<char>& out) override
    {
        void* data = nullptr;
        int ret    = get_vi(&data);
        if (ret > 0) {
            if (out.size() >= ret) {
                memcpy(out.data(), data, ret);
            } else {
                out.resize(ret);
                memcpy(out.data(), data, ret);
            }
        }
        return ret;
    }
    virtual int connect(std::function<int(const std::vector<char>&)> func) override
    {
        return -1;
    }
    virtual std::shared_ptr<void> get_param(const std::string& param_name) override
    {
        std::map<std::string, std::function<std::shared_ptr<void>(void)>> params = {
            {"width", [this]() { return std::make_shared<int>(_fmt.fmt.pix.width); }},
            {"height", [this]() { return std::make_shared<int>(_fmt.fmt.pix.height); }},
            {"type", [this]() { return std::make_shared<int>(_fmt.fmt.pix.pixelformat); }},
        };
        return params[param_name]();
    }
    virtual int set_param(const std::string& param_name, const std::shared_ptr<void>& value) override
    {
        std::map<std::string, std::function<int(void)>> params = {
            {"fps",
             [this, value]() {
                 int ret = 0;
                 if (_v4l2_fd > 0) {
                     _parm.type                                  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                     _parm.parm.capture.timeperframe.numerator   = 1;
                     _parm.parm.capture.timeperframe.denominator = *(std::static_pointer_cast<int>(value));
                     ret                                         = ioctl(_v4l2_fd, VIDIOC_S_PARM, &_parm);
                     if (ret < 0) {
                         perror("设置帧率失败");
                     }
                     return ret;
                 }
                 return -1;
             }},
        };
        return params[param_name]();
    }
    virtual ~d_hal_v4l2() override
    {
        deinit();
    };
};
std::shared_ptr<d_hal_vin> create_d_hal_vin(D_HAL_VIN_TYPE_E type)
{
    if (type == D_HAL_VIN_V4L2) {
        return std::make_shared<d_hal_v4l2>(D_HAL_VIN_V4L2);
    } else {
        return nullptr;
    }
}
}  // namespace  d_hal
