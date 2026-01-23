#include <d_hal_vin.hpp>
#include "http_server.hpp"
#include <linux/videodev2.h>

int main() {
    auto cam = d_hal::create_d_hal_vin(d_hal::D_HAL_VIN_V4L2);
    int ret = cam->open("/dev/video0", 1920, 1080, V4L2_PIX_FMT_MJPEG);
    printf("open cam ret=%d\n", ret);
    for (size_t i = 0; i < 100; )
    
    {
        void *image ;
        int ret = cam->get_vi(&image);
        if(ret > 0)
        {
            i++;
            send_jpeg(image, ret);
            printf("get vi success:%d\n", ret);
        }
        // if(ret <= 0)
        // {
        //     printf("get vi error:%d\n", ret);
        //     break;
        // }
        // send_jpeg(image, ret);

    }
    cam.reset();

    return 0;
}