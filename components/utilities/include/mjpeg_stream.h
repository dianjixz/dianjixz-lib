





#include <hv/TcpServer.h>
#include <sys/time.h>
#include <time.h>
using namespace hv;
static int get_jpeg_status = 0;
std::vector<uint8_t> jpeg_buf_vec;
TcpServer srv;
#ifndef STREAM_PORT
#define STREAM_PORT 8081
#endif

const char *http_response = "HTTP/1.0 200 OK\r\n"
                            "Server: BaseHTTP/0.6 Python/3.10.12\r\n"
                            "Date: %s\r\n"
                            "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n"
                            "Connection: close\r\n"
                            "Content-Type: multipart/x-mixed-replace;boundary=--boundarydonotcross\r\n"
                            "Expires: Mon, 1 Jan 2130 00:00:00 GMT\r\n"
                            "Pragma: no-cache\r\n"
                            "Access-Control-Allow-Origin: *\r\n";
const char *http_jpeg_response = "\r\n"
                                 "--boundarydonotcross\r\n"
                                 "X-Timestamp: %lf\r\n"
                                 "Content-Length: %d\r\n"
                                 "Content-Type: image/jpeg\r\n"
                                 "\r\n";
const char *http_static_response = "HTTP/1.0 200 OK\r\n"
                                  "Server: BaseHTTP/0.6 Python/3.10.12\r\n"
                                  "Date: %s\r\n"
                                  "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n"
                                  "Connection: close\r\n"
                                  "Content-Type: image/jpeg\r\n"
                                  "Expires: Mon, 1 Jan 2130 00:00:00 GMT\r\n"
                                  "Pragma: no-cache\r\n"
                                  "Access-Control-Allow-Origin: *\r\n"
                                  "\r\n";
char http_response_buff[1024];
char http_response_buff1[1024];

void send_jpeg(void *buff, int size)
{
    if(size > jpeg_buf_vec.size())
    {
        jpeg_buf_vec.resize(size);
    }
    memcpy(jpeg_buf_vec.data(), buff, size);
    switch (get_jpeg_status) {
        case 0: {
            int listenfd = srv.createsocket(STREAM_PORT);
            if (listenfd < 0) {
                break;
            }
            srv.onConnection = [](const SocketChannelPtr &channel) {
                std::string peeraddr = channel->peeraddr();
                if (channel->isConnected()) {
                    auto st =  channel->newContextPtr<std::vector<int>>();
                    st->resize(2);
                    st->at(0) = 0;
                    st->at(1) = 0;
                    printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
                } else {
                    channel->deleteContextPtr();
                    printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
                }
            };
            srv.onMessage = [](const SocketChannelPtr &channel, Buffer *buf) {
                // echo
                auto st = channel->getContextPtr<std::vector<int>>();
                printf("< %.*s\n", (int)buf->size(), (char *)buf->data());
                if (strstr((char *)buf->data(), "static_image") != NULL)
                {
                    st->at(0) = 1;
                }
                if (strstr((char *)buf->data(), "\r\n\r\n") != NULL)
                {
                    if (st->at(0) == 1)
                    {
                        memset(http_response_buff, 0, 1024);

                        time_t current_time;
                        struct tm *time_info;

                        time(&current_time);
                        time_info = gmtime(&current_time);  // 使用gmtime以获取GMT时间

                        char time_str[30];  // 存储时间字符串的数组

                        // 使用strftime函数将时间格式化为指定的格式
                        strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", time_info);

                        sprintf(http_response_buff, http_static_response, time_str);
                        channel->write(http_response_buff);
                        channel->write(jpeg_buf_vec.data(), jpeg_buf_vec.size());
                        channel->close();
                    }else
                    {
                        memset(http_response_buff, 0, 1024);

                        time_t current_time;
                        struct tm *time_info;

                        time(&current_time);
                        time_info = gmtime(&current_time);  // 使用gmtime以获取GMT时间

                        char time_str[30];  // 存储时间字符串的数组

                        // 使用strftime函数将时间格式化为指定的格式
                        strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", time_info);

                        sprintf(http_response_buff, http_response, time_str);
                        channel->write(http_response_buff);
                        st->at(1) = 1;
                    }
                }

                // channel->write(buf);
            };
            srv.setThreadNum(2);
            srv.start();
            get_jpeg_status = 1;
        } break;
        case 1: {
            char tmpsdas[256];

            struct timeval tv;
            gettimeofday(&tv, NULL);

            double timestamp = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

            memset(http_response_buff1, 0, 1024);
            sprintf(http_response_buff1, http_jpeg_response, timestamp, size);

            srv.foreachChannel([&](const SocketChannelPtr &channel) {
                auto st = channel->getContextPtr<std::vector<int>>();
                if (st->at(1) == 1)
                {
                    channel->write(http_response_buff1);
                    channel->write(buff, size);
                }
            });
        } break;
        default:
            break;
    }

    // static int nihasd = 0;
    // if(nihasd < 60)
    // {

    //     char filename[100];
    //     sprintf(filename, "./nihao_%d.jpg", nihasd);

    //     FILE *fp_w = fopen(filename, "wb");
    //     if (fp_w) {
    //         // ALOGN("\tWrite new JPG result image to file: %s", dstFile);
    //         fwrite(buff->p_vir, 1, buff->n_size, fp_w);
    //         fclose(fp_w);
    //     }
    //     nihasd ++;
    // }
}
