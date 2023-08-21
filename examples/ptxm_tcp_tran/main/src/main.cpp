#include "hv/TcpServer.h"
#include "hv/hlog.h"
#include "pts_creat.h"
using namespace hv;


TcpServer srv;
ptmx ptm;

void msg_handl(const char* msg, int size)
{
    srv.broadcast(msg, size);
}



int main(int argc, char* argv[]) {


    hlog_set_level(LOG_LEVEL_SILENT);
    if (argc < 3) {
        // printf("Usage: %s exe port\n", argv[0]);
        return -10;
    }
    int port = atoi(argv[2]);


    int listenfd = srv.createsocket(port);
    if (listenfd < 0) {
        return -20;
    }
    // printf("server listen on port %d, listenfd=%d ...\n", port, listenfd);

    // srv.onConnection = [](const SocketChannelPtr& channel) {
    //     std::string peeraddr = channel->peeraddr();
    //     if (channel->isConnected()) {
    //         printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
    //     } else {
    //         printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
    //     }
    // };
    // srv.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
    //     // echo
    //     printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
    //     channel->write(buf);
    // };
    // srv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf) {
    //     printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
    // };
    srv.setThreadNum(1);
    srv.setMaxConnectionNum(2);
    srv.start();
    ptm.set_msg_call_back(msg_handl);
    ptm.open();

    int pid = fork();
    if(pid == 0)
    {
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stdout);
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stderr);
        freopen(ptm.get_slave_ptmx_name().c_str(), "r", stdin);
        execl(argv[1], argv[1],
              NULL);
    }

    // press Enter to stop
    while (1)
    {
        sleep(1);
    }
    return 0;
}



// #include "hv/hloop.h"

// void on_close(hio_t* io) {
// }

// void on_recv(hio_t* io, void* buf, int readbytes) {
// 	// 回显数据
//     hio_write(io, buf, readbytes);
// }

// void on_accept(hio_t* io) {
// 	// 设置close回调
//     hio_setcb_close(io, on_close);
//     // 设置read回调
//     hio_setcb_read(io, on_recv);
//     // 开始读
//     hio_read(io);
// }

// int main(int argc, char** argv) {
//     if (argc < 2) {
//         printf("Usage: cmd port\n");
//         return -10;
//     }
//     int port = atoi(argv[1]);

//     // 创建事件循环
//     hloop_t* loop = hloop_new(0);
//     // 创建TCP服务
//     hio_t* listenio = hloop_create_tcp_server(loop, "0.0.0.0", port, on_accept);
//     if (listenio == NULL) {
//         return -20;
//     }
//     // 运行事件循环
//     hloop_run(loop);
//     // 释放事件循环
//     hloop_free(&loop);
//     return 0;
// }
