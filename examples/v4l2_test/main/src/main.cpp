#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libv4l2.h>
// #include <opencv2/opencv.hpp>
#include <vector>
#define WIDTH 1920
#define HEIGHT 1080
#define BUFFER_COUNT 4
#include "http_server.hpp"
struct buffer {
    void *start;
    size_t length;
};

int main() {
    int fd;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    struct buffer *buffers;
    unsigned int i;
    enum v4l2_buf_type type;

    // 1. 打开设备
    fd = v4l2_open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        perror("打开设备失败");
        return -1;
    }
    printf("设备打开成功\n");

    // 2. 设置视频格式
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (v4l2_ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("设置格式失败");
        v4l2_close(fd);
        return -1;
    }
    printf("分辨率: %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

    // 3. 请求缓冲区
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (v4l2_ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("请求缓冲区失败");
        v4l2_close(fd);
        return -1;
    }

    // 4. 映射缓冲区
    buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));
    for (i = 0; i < req.count; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (v4l2_ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("查询缓冲区失败");
            v4l2_close(fd);
            return -1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = v4l2_mmap(NULL, buf.length,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == buffers[i].start) {
            perror("mmap 失败");
            v4l2_close(fd);
            return -1;
        }
    }

    // 5. 将缓冲区放入队列
    for (i = 0; i < req.count; i++) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (v4l2_ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("入队失败");
            v4l2_close(fd);
            return -1;
        }
    }

    // 6. 开始采集
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("开始采集失败");
        v4l2_close(fd);
        return -1;
    }
    printf("开始采集视频...\n");

    // 7. 采集帧
    for (i = 0; ; i++) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r < 0) {
            perror("select 失败");
            break;
        }
        if (r == 0) {
            fprintf(stderr, "select 超时\n");
            break;
        }

        // 出队
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (v4l2_ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("出队失败");
            break;
        }

        printf("帧 %d: 字节数=%d\n", i, buf.bytesused);
        send_jpeg(buffers[buf.index].start, buf.bytesused);
        // 这里可以处理图像数据
        // process_image(buffers[buf.index].start, buf.bytesused);
        // cv::Mat img(buffers[buf.index].start);
        // std::vector<uchar> buffers_vec((uchar *)buffers[buf.index].start, (uchar *)buffers[buf.index].start + buf.bytesused);
        // cv::Mat image = cv::imdecode(buffers_vec, cv::IMREAD_COLOR);
        // cv::imshow("Image", image);
        // cv::waitKey(1);

        // cv::imshow("image", image);
        // 重新入队
        if (v4l2_ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("重新入队失败");
            break;
        }
    }

    // 8. 停止采集
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_ioctl(fd, VIDIOC_STREAMOFF, &type);

    // 9. 清理资源
    for (i = 0; i < req.count; i++) {
        v4l2_munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    v4l2_close(fd);

    printf("采集完成\n");
    return 0;
}