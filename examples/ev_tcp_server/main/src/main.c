#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 12345
#define BUFFER_SIZE 1024

// 客户端读事件回调函数
void client_cb(EV_P_ ev_io *w, int revents) {
    char buffer[BUFFER_SIZE];
    ssize_t read_size;

    if (revents & EV_READ) {
        // 读取数据
        read_size = recv(w->fd, buffer, sizeof(buffer) - 1, 0);
        if (read_size < 0) {
            perror("recv error");
            ev_io_stop(EV_A_ w);
            close(w->fd);
            free(w);
            return;
        } else if (read_size == 0) {
            // 客户端关闭连接
            printf("Client disconnected\n");
            ev_io_stop(EV_A_ w);
            close(w->fd);
            free(w);
            return;
        }

        // 打印接收到的数据
        buffer[read_size] = '\0';
        printf("Received: %s\n", buffer);

        // 回显数据给客户端
        send(w->fd, buffer, read_size, 0);
    }
}

// 服务器监听套接字事件回调函数
void accept_cb(EV_P_ ev_io *w, int revents) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd;

    if (revents & EV_READ) {
        // 接受新的客户端连接
        client_fd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept error");
            return;
        }

        printf("New client connected: %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 为新连接创建一个 ev_io 观察器
        ev_io *client_watcher = (ev_io *)malloc(sizeof(ev_io));
        ev_io_init(client_watcher, client_cb, client_fd, EV_READ);
        ev_io_start(EV_A_ client_watcher);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项，允许端口重用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定地址和端口
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(sockfd, 128) < 0) {
        perror("listen error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 初始化 libev 事件循环
    struct ev_loop *loop = EV_DEFAULT;

    // 创建 ev_io 观察器用于监听套接字
    ev_io server_watcher;
    ev_io_init(&server_watcher, accept_cb, sockfd, EV_READ);
    ev_io_start(loop, &server_watcher);

    // 启动事件循环
    ev_run(loop, 0);

    // 关闭套接字
    close(sockfd);

    return 0;
}
