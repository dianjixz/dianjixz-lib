#include "hal_vo.hpp"
// #include "tbl_yield.h"
#include <list>
#include <functional>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define BUFFER_COUNT 4

enum V4L2_VIDEO_FORMAT {
    HAL_V4L2_FORMAT_YUYV,
    HAL_V4L2_FORMAT_MJPEG,
    HAL_V4L2_FORMAT_NV12,
};

typedef struct {
    void *start;
    size_t length;
} buffer_t;

class hal_v4l2 {
private:
    int vedio_id;
    std::function<void(int, int, int, void *, int)> frame_put_lists[10];
    int frame_put_func_count;
    std::unique_ptr<std::thread> work_thread;
    int work_flag;
    int _width;
    int _height;
    int _format;
    int _fps;
    int _bytesperline;
    int _imagesize;
    void open_device()
    {
    }
    void get_frame()
    {
    }
    void release_frame()
    {
    }
    void close_device()
    {
    }
    void put_frame(void *buf, int size)
    {
        for (int i = 0; i < 10; i++)
        {
            if (frame_put_lists[i] != nullptr)
            {
                frame_put_lists[i](_width, _height, _format, buf, size);
            }
        }
    }

    void _work_list()
    {
        int fd;
        buffer_t buffers[4];
        char dev_name[20];
        snprintf(dev_name, sizeof(dev_name), "/dev/video%d", vedio_id);
        printf("打开设备: %s\n", dev_name);
        // 1. 打开摄像头设备
        fd = open(dev_name, O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            perror("打开设备失败");
            return;
        }

        // 2. 设置图像格式
        struct v4l2_format fmt  = {0};
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = _width;
        fmt.fmt.pix.height      = _height;
        fmt.fmt.pix.pixelformat = _format;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
            perror("设置格式失败");
            close(fd);
            return;
        }
        if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) {
            perror("获取格式失败");
            close(fd);
            return;
        }
        _width = fmt.fmt.pix.width;
        _height = fmt.fmt.pix.height;
        _format = fmt.fmt.pix.pixelformat;
        _bytesperline = fmt.fmt.pix.bytesperline;  // 建议保存这个值
        _imagesize = fmt.fmt.pix.sizeimage;        // 建议保存这个值

        // 3. 获取/设置帧率
        struct v4l2_streamparm parm;
        memset(&parm, 0, sizeof(parm));
        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        // 如果需要设置特定帧率，先尝试设置
        if (_fps > 0) {
            parm.parm.capture.timeperframe.numerator = 1;
            parm.parm.capture.timeperframe.denominator = _fps;
            if (ioctl(fd, VIDIOC_S_PARM, &parm) < 0) {
                perror("设置帧率失败");
                // 不是致命错误，继续执行
            }
        }
        // 获取实际帧率
        memset(&parm, 0, sizeof(parm));
        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_G_PARM, &parm) == 0) {
            if (parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
                if (parm.parm.capture.timeperframe.numerator && 
                    parm.parm.capture.timeperframe.denominator) {
                    _fps = parm.parm.capture.timeperframe.denominator / 
                        parm.parm.capture.timeperframe.numerator;
                }
            }
        }

        // 3. 请求缓冲区
        struct v4l2_requestbuffers req = {0};
        req.count                      = BUFFER_COUNT;
        req.type                       = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory                     = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
            perror("请求缓冲区失败");
            close(fd);
        }

        // 4. 映射缓冲区并加入队列
        for (int i = 0; i < BUFFER_COUNT; i++) {
            struct v4l2_buffer buf = {0};
            buf.type               = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory             = V4L2_MEMORY_MMAP;
            buf.index              = i;

            // 查询缓冲区
            if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
                perror("查询缓冲区失败");
                close(fd);
            }

            // 映射到用户空间
            buffers[i].length = buf.length;
            buffers[i].start  = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

            if (buffers[i].start == MAP_FAILED) {
                perror("mmap 失败");
                close(fd);
            }

            // 缓冲区加入队列
            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
                perror("缓冲区入队失败");
                close(fd);
            }
        }

        // 5. 开始采集
        enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
            perror("启动采集失败");
            close(fd);
        }

        // 6. 获取 10 帧图像
        while (work_flag) {
            // sleep(1);
            struct v4l2_buffer buf = {0};
            buf.type               = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory             = V4L2_MEMORY_MMAP;

            // 从队列取出已填充的缓冲区
            if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
                // perror("出队失败");
                usleep(5000);
                continue;
            }

            // 处理图像数据
            // printf("第 %d 帧: 大小 %d 字节\n", frame + 1, buf.bytesused);
            // 这里可以保存或处理 buffers[buf.index].start 中的数据
            put_frame(buffers[buf.index].start, buf.bytesused);

            // 缓冲区重新入队
            if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
                perror("重新入队失败");
                continue;
            }
        }

        // 7. 停止采集
        if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
            perror("停止采集失败");
        }

        // 8. 清理资源
        for (int i = 0; i < BUFFER_COUNT; i++) {
            munmap(buffers[i].start, buffers[i].length);
        }
        close(fd);
    }

public:
    hal_v4l2(/* args */)
    {
        frame_put_func_count = 0;
        vedio_id             = 0;
    }
    hal_v4l2(int id)
    {
        frame_put_func_count = 0;
        vedio_id             = id;
    }

    void set_video_frame(int width, int height, int format, int fps)
    {
        _width  = width;
        _height = height;
        _format = format;
        _fps    = fps;
    }

    ~hal_v4l2()
    {
        if (work_flag) {
            stop();
        }
    }

    void work()
    {
        work_flag  = 1;
        work_thread = std::make_unique<std::thread>(std::bind(&hal_v4l2::_work_list, this));
    }
    void stop()
    {
        work_flag = 0;
        work_thread->join();
        work_thread.reset();
    }

    int connect(const std::function<void(int, int, int, void *, int)> &func)
    {
        for (int i = 0; i < 10; i++) {
            if (frame_put_lists[i] == nullptr) {
                frame_put_lists[i] = func;
                return i;
            }
        }
    }
    void disconnect(int handle)
    {
        if (handle >= 0 && handle < 10) {
            frame_put_lists[handle] = nullptr;
        }
    }
};
