#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <netdb.h>
#include <iostream>
#include <string>

class llm_TcpClient {
   public:
    llm_TcpClient(const std::string &host, int port)
        : host(host), port(port), sock(-1), status(0), message_buff_len(0) {
        connect();
    }

    ~llm_TcpClient() {
        if (sock != -1) {
            close(sock);
        }
        if (message_buff_len) {
            free(message_buff);
        }
    }

    int sendMessage(const std::string &message) {
        if (status > 0) {
            if (send(sock, message.c_str(), message.size(), 0) < 0) {
                std::cerr << "Send failed" << std::endl;
                return -1;
            }
            return 0;
        }
        return -2;
    }

    int receiveMessage_pool(std::string &message, int bufferSize = 4096) {
        // char *buffer = (char *)malloc(bufferSize);
        if (message_buff_len != bufferSize) {
            if (message_buff_len) {
                free(message_buff);
            }
            message_buff_len = bufferSize;
            message_buff     = (char *)malloc(bufferSize);
        }
        // std::string message;
        int ret = 0;
        fd_set readfd;
        FD_ZERO(&readfd);
        struct timeval timeout;
        timeout.tv_sec  = 0;
        timeout.tv_usec = 0;
        if (status > 0) {
            FD_SET(sock, &readfd);
            select(sock + 1, &readfd, NULL, NULL, &timeout);
            if (FD_ISSET(sock, &readfd)) {
                int bytesReceived = recv(sock, message_buff, bufferSize - 1, 0);
                if (bytesReceived < 0) {
                    std::cerr << "Receive failed" << std::endl;
                    ret = -1;
                } else if (bytesReceived == 0) {
                    std::cerr << "Connection closed by server" << std::endl;
                    ret = -2;
                } else {
                    message_buff[bytesReceived] = '\0';
                    message                     = std::string(message_buff);
                    ret                         = bytesReceived;
                }
            }
        }

        return ret;
    }

    std::string receiveMessage(int bufferSize = 4096) {
        // char buffer[bufferSize];
        // char *buffer = (char *)malloc(bufferSize);
        if (message_buff_len != bufferSize) {
            if (message_buff_len) {
                free(message_buff);
            }
            message_buff_len = bufferSize;
            message_buff     = (char *)malloc(bufferSize);
        }
        std::string message;
        if (status > 0) {
            int bytesReceived = recv(sock, message_buff, bufferSize - 1, 0);
            if (bytesReceived < 0) {
                std::cerr << "Receive failed" << std::endl;
            } else if (bytesReceived == 0) {
                std::cerr << "Connection closed by server" << std::endl;
            } else {
                message_buff[bytesReceived] = '\0';
                message                     = std::string(message_buff);
            }
        }
        // free(buffer);
        return message;
    }

   private:
    int sock;
    std::string host;
    int port;
    // struct sockaddr_in server_addr;
    struct addrinfo hints, *res;
    int status;
    char *message_buff;
    int message_buff_len;

    void connect() {
        struct addrinfo hints, *res, *p;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
            std::cerr << "getaddrinfo failed" << std::endl;
            status = -1;
            return ;    
        }
        for (p = res; p != nullptr; p = p->ai_next) {
            if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                std::cerr << "socket failed" << std::endl;
                status = -2;
                continue;
            }
            // Set timeout for connect
            struct timeval timeout;
            timeout.tv_sec  = 5;
            timeout.tv_usec = 0;
            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
                std::cerr << "setsockopt failed (SO_RCVTIMEO)" << std::endl;
                status = -3;
            }
            if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
                std::cerr << "setsockopt failed (SO_SNDTIMEO)" << std::endl;
                status = -4;
            }

            if (::connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
                close(sock);
                std::cerr << "connect failed" << std::endl;
                status = -4;
                continue;
            }
            break; // Successfully connected
        }
        if (p == nullptr) {
            std::cerr << "failed to connect" << std::endl;
        }
        freeaddrinfo(res);
        status = 1;
        // std::cout << "Connected to " << res->ai_addr << ":" << port << std::endl;
    }
};

#endif  // TCPCLIENT_HPP