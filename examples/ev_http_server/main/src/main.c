// #include <ev.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <errno.h>
// #include <http_parser.h>

// #define PORT 8080
// #define BUFFER_SIZE 1024

// typedef struct {
//     int fd;
//     struct ev_io ev_read;
//     http_parser parser;
//     http_parser_settings settings;
// } client_t;

// // HTTP 请求解析回调
// int on_url(http_parser *parser, const char *at, size_t length) {
//     printf("URL: %.*s\n", (int)length, at);
//     return 0;
// }

// // 客户端读事件回调
// void client_read_cb(EV_P_ ev_io *w, int revents) {
//     client_t *client = (client_t *)w;
//     char buffer[BUFFER_SIZE];
//     ssize_t nread = recv(client->fd, buffer, sizeof(buffer) - 1, 0);

//     if (nread <= 0) {
//         if (nread < 0) {
//             perror("recv");
//         }
//         ev_io_stop(loop, w);
//         close(client->fd);
//         free(client);
//         printf("Client disconnected\n");
//     } else {
//         buffer[nread] = '\0';

//         // 解析 HTTP 请求
//         size_t nparsed = http_parser_execute(&client->parser, &client->settings, buffer, nread);
//         if (nparsed != nread) {
//             fprintf(stderr, "HTTP parse error\n");
//         }

//         // 返回简单响应
//         const char *response =
//             "HTTP/1.1 200 OK\r\n"
//             "Content-Type: text/plain\r\n"
//             "Content-Length: 13\r\n"
//             "\r\n"
//             "Hello, World!";
//         send(client->fd, response, strlen(response), 0);
//     }
// }

// // 服务端监听事件回调
// void accept_cb(EV_P_ ev_io *w, int revents) {
//     struct sockaddr_in client_addr;
//     socklen_t client_len = sizeof(client_addr);
//     int client_fd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);

//     if (client_fd < 0) {
//         perror("accept");
//         return;
//     }

//     printf("New client connected: %d\n", client_fd);

//     // 创建客户端结构体
//     client_t *client = (client_t *)malloc(sizeof(client_t));
//     client->fd = client_fd;

//     // 初始化 HTTP 解析器
//     http_parser_init(&client->parser, HTTP_REQUEST);
//     client->parser.data = client;

//     // 设置回调
//     http_parser_settings_init(&client->settings);
//     client->settings.on_url = on_url;

//     // 初始化读事件
//     ev_io_init(&client->ev_read, client_read_cb, client_fd, EV_READ);
//     ev_io_start(loop, &client->ev_read);
// }

// // 主函数
// int main() {
//     int server_fd;
//     struct sockaddr_in server_addr;

//     // 创建套接字
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0) {
//         perror("socket");
//         return 1;
//     }

//     // 设置地址复用
//     int opt = 1;
//     setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     // 绑定地址和端口
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         perror("bind");
//         close(server_fd);
//         return 1;
//     }

//     // 开始监听
//     if (listen(server_fd, SOMAXCONN) < 0) {
//         perror("listen");
//         close(server_fd);
//         return 1;
//     }

//     printf("HTTP server started on port %d\n", PORT);

//     // 初始化事件循环
//     struct ev_loop *loop = EV_DEFAULT;
//     ev_io server_watcher;

//     // 初始化服务端监听事件
//     ev_io_init(&server_watcher, accept_cb, server_fd, EV_READ);
//     ev_io_start(loop, &server_watcher);

//     // 开始事件循环
//     ev_run(loop, 0);

//     // 清理资源
//     close(server_fd);
//     return 0;
// }

#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <http_parser.h>
#include <time.h>
#include <sys/time.h>
#define PORT        12345
#define BUFFER_SIZE 1024

const char *http_response =
    "HTTP/1.0 200 OK\n"
    "Server: BaseHTTP/0.6 Python/3.10.12\n"
    "Date: %s\n"
    "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\n"
    "Connection: close\n"
    "Content-Type: multipart/x-mixed-replace;boundary=--boundarydonotcross\n"
    "Expires: Mon, 1 Jan 2130 00:00:00 GMT\n"
    "Pragma: no-cache\n"
    "Access-Control-Allow-Origin: *\n";
const char *http_jpeg_response =
    "\n"
    "--boundarydonotcross\n"
    "X-Timestamp: %lf\n"
    "Content-Length: %d\n"
    "Content-Type: image/jpeg\n"
    "\n";


// 客户端结构体
typedef struct {
    int fd;
    struct ev_io ev_read;
    http_parser parser;
    http_parser_settings settings;
    int status;
} client_t;

// 全局客户端列表（简单实现，固定大小）
#define MAX_CLIENTS 1024
client_t *clients[MAX_CLIENTS];

// 添加客户端到全局列表
void add_client(client_t *client)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            clients[i] = client;
            return;
        }
    }
}

// 从全局列表移除客户端
void remove_client(client_t *client)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == client) {
            clients[i] = NULL;
            return;
        }
    }
}

// HTTP 请求解析回调
int on_url(http_parser *parser, const char *at, size_t length)
{
    printf("URL: %.*s\n", (int)length, at);
    return 0;  // 返回 0 表示成功
}

int on_header_field(http_parser *parser, const char *at, size_t length)
{
    printf("Header Field: %.*s\n", (int)length, at);
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length)
{
    printf("Header Value: %.*s\n", (int)length, at);
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length)
{
    printf("Body: %.*s\n", (int)length, at);
    return 0;
}

int on_message_complete(http_parser *parser)
{
    printf("Message Complete\n");
    client_t *client = (client_t *)parser->data;
    client->status   = 1;
    return 0;
}

// 遍历所有客户端并发送数据
void broadcast_message(const char *message, size_t len)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            client_t *client = clients[i];

            char http_response_buff1[1024];
            struct timeval tv;
            gettimeofday(&tv, NULL);
            double timestamp = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
            memset(http_response_buff1, 0, 1024);
            sprintf(http_response_buff1, http_jpeg_response, timestamp, 1024);
            ssize_t nwrite = send(client->fd, message, len, 0);
            if (nwrite < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // 发送缓冲区满，丢弃数据
                    fprintf(stderr, "Client %d buffer full, message dropped\n", client->fd);
                } else {
                    // 其他错误，关闭客户端
                    perror("send");
                    ev_io_stop(EV_DEFAULT, &client->ev_read);
                    close(client->fd);
                    remove_client(client);
                    free(client);
                }
            }
            //jpeg data
            // ssize_t nwrite = send(client->fd, message, len, 0);
            // if (nwrite < 0) {
            //     if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //         // 发送缓冲区满，丢弃数据
            //         fprintf(stderr, "Client %d buffer full, message dropped\n", client->fd);
            //     } else {
            //         // 其他错误，关闭客户端
            //         perror("send");
            //         ev_io_stop(EV_DEFAULT, &client->ev_read);
            //         close(client->fd);
            //         remove_client(client);
            //         free(client);
            //     }
            // }
        }
    }
}

// 客户端读事件回调
void client_read_cb(EV_P_ ev_io *w, int revents)
{
    client_t *client = (client_t *)w;
    char buffer[BUFFER_SIZE];
    ssize_t nread = recv(client->fd, buffer, sizeof(buffer) - 1, 0);
    if (nread <= 0) {
        if (nread < 0) {
            perror("recv");
        }
        ev_io_stop(loop, w);
        close(client->fd);
        remove_client(client);
        free(client);
        printf("Client disconnected\n");
    } else {
        buffer[nread] = '\0';

        // 解析 HTTP 请求
        if (!client->status) {
            size_t nparsed = http_parser_execute(&client->parser, &client->settings, buffer, nread);
            if (nparsed != nread) {
                fprintf(stderr, "HTTP parse error\n");
            }
        } else {
            // 返回简单响应
            char http_response_buff[1024];
            memset(http_response_buff, 0, 1024);
            time_t current_time;
            struct tm *time_info;
            time(&current_time);
            time_info = gmtime(&current_time);
            char time_str[30];
            strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", time_info);
            send(client->fd, http_response_buff, sprintf(http_response_buff, http_response, time_str), 0);
        }
    }
}

// 服务端监听事件回调
void accept_cb(EV_P_ ev_io *w, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd        = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
        perror("accept");
        return;
    }

    printf("New client connected: %d\n", client_fd);

    // 创建客户端结构体并初始化
    client_t *client = (client_t *)malloc(sizeof(client_t));
    client->fd       = client_fd;

    // 初始化 HTTP 解析器
    http_parser_init(&client->parser, HTTP_REQUEST);
    client->parser.data = client;
    client->status      = 0;
    // 设置回调
    http_parser_settings_init(&client->settings);
    client->settings.on_url              = on_url;
    client->settings.on_header_field     = on_header_field;
    client->settings.on_header_value     = on_header_value;
    client->settings.on_body             = on_body;
    client->settings.on_message_complete = on_message_complete;

    // 添加到全局客户端列表
    add_client(client);

    // 设置为非阻塞模式
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    // 初始化读事件
    ev_io_init(&client->ev_read, client_read_cb, client_fd, EV_READ);
    ev_io_start(loop, &client->ev_read);
}

// 主函数
int main()
{
    int server_fd;
    struct sockaddr_in server_addr;

    // 创建套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 设置地址复用
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // 开始监听
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server started on port %d\n", PORT);

    // 初始化 libev 事件循环
    struct ev_loop *loop = EV_DEFAULT;
    ev_io server_watcher;

    // 初始化服务端监听事件
    ev_io_init(&server_watcher, accept_cb, server_fd, EV_READ);
    ev_io_start(loop, &server_watcher);

    // 开始事件循环
    ev_run(loop, 0);

    // 清理资源
    close(server_fd);
    return 0;
}
