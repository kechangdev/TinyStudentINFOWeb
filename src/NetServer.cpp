#include "../include/NetServer.h"
#include "../include/Utils.h"
#include "../include/ThreadPool.h"
#include <sys/socket.h> // 套接字相关函数
#include <netinet/in.h> // 网络地址结构
#include <arpa/inet.h>  // inet_ntop
#include <unistd.h>     // 关闭套接字
#include <sstream>      // 字符串流
#include <iostream>     // 输入输出流
#include <cstring>      // memset
#include <csignal>      // std::signal, SIGINT

#include "../conf.h"

// 构造函数：初始化服务器端口，套接字描述符和运行状态
NetServer::NetServer(int port) : serverPort(port), serverSocket(-1), isRunning(false) {}

// 析构函数：确保服务器在对象销毁时停止运行，释放资源
NetServer::~NetServer() {
    if (isRunning) {
        close(serverSocket);
    }
}

// 设置特定路径的请求处理函数
void NetServer::setHandler(const std::string& path, RequestHandler handler) {
    handlers[path] = handler;
}

// 静态成员 stopFlag 初始化
bool NetServer::stopFlag = false;
// 信号处理函数
void NetServer::signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << Utils::getTimeAndMessage(
            0, "INFO", "Received SIGINT (Ctrl + C), shutting down...") << std::endl;
        stopFlag = true;
        exit(EXIT_SUCCESS);
        // 这里仅做标记，不宜直接调用 close 等操作
        // 因为信号处理函数里做太多工作可能不安全
    }
}

// 线程池版本
void NetServer::run() {
    // 1. 注册 SIGINT 的处理函数
    NetServer::stopFlag = false; // 每次 run() 前重置
    std::signal(SIGINT, NetServer::signalHandler);

    // 2. 初始化线程池
    ThreadPool pool(THREAD_POOL_SIZE);

    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket creation failed.") << std::endl;
        return;
    }
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket creation Success.") << std::endl;

    // 设置套接字选项
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket setsockopt failed.") << std::endl;
        close(serverSocket);
        return;
    }
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket setsockopt Success.") << std::endl;

    // 配置地址结构
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 监听所有可用接口
    address.sin_port = htons(serverPort);

    // 绑定套接字到指定的地址和端口
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cout << Utils::getTimeAndMessage(0, "INFO", "IPV4:PORT Bind Socket failed.") << std::endl;
        close(serverSocket);
        return;
    }
    std::cout << Utils::getTimeAndMessage(0, "INFO", "IPV4:PORT Bind Socket Success.") << std::endl;

    // 开始监听
    if (listen(serverSocket, MAX_CONNECT_NUM) < 0) {
        std::cout << Utils::getTimeAndMessage(0, "INFO", "Listen Socket failed.") << std::endl;
        close(serverSocket);
        return;
    }
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Listen Socket Success.") << std::endl;

    isRunning = true;
    std::cout << Utils::getTimeAndMessage(
        0, "INFO", "Server started on port " + std::to_string(serverPort)) << std::endl;
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Waiting for connection...") << std::endl;

    // 3. 主循环
    while (isRunning) {
        // 如果检测到 stopFlag 为 true，就退出循环
        if (NetServer::stopFlag) {
            std::cout << Utils::getTimeAndMessage(
                0, "INFO", "Stop flag set. Terminating server loop...") << std::endl;
            isRunning = false;
            break;
        }

        struct sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (clientSocket < 0) {
            if (isRunning) {
                std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket accept failed.") << std::endl;
            }
            continue;
        }

        // 将处理客户端的任务提交给线程池
        pool.enqueue([this, clientSocket, clientAddress]() {
            this->handleClient(clientSocket, clientAddress);
        });
    }

    // 4. 停止：跳出循环后，关闭服务器套接字
    close(serverSocket);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Server stopped gracefully.") << std::endl;
}

// 处理单个客户端连接的函数
void NetServer::handleClient(int clientSocket, const struct sockaddr_in& clientAddress) {
    // 获取客户端的 IP 地址
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::string clientIPStr(clientIP);

    // 记录连接日志：包含 IP 地址
    std::cout << Utils::getTimeAndMessage(0, "ACCESS", "IP: " + clientIPStr + ", Action: Connected") << std::endl;

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    // 从客户端套接字读取数据
    int valread = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (valread <= 0) { // 如果读取失败或没有数据
        // 记录断开连接的日志
        std::cout << Utils::getTimeAndMessage(0, "ACCESS", "IP: " + clientIPStr + ", Action: Read Failed or No Data") << std::endl;
        close(clientSocket); // 关闭客户端套接字
        return; // 结束处理
    }

    std::string requestStr(buffer, valread);
    std::istringstream requestStream(requestStr);
    HttpRequest request;
    std::string line;

    // 解析请求行（第一行）
    if (std::getline(requestStream, line)) {
        std::istringstream lineStream(line);
        lineStream >> request.method >> request.path;
    }

    // 解析请求头部
    while (std::getline(requestStream, line) && line != "\r") {
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);

            // 去除前导空格和末尾换行符
            size_t start = value.find_first_not_of(" ");
            size_t end = value.find_last_not_of("\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                value = value.substr(start, end - start + 1);
                request.headers[key] = value;
            }
        }
    }

    // 解析请求体
    if (requestStream.good()) {
        std::ostringstream bodyStream;
        bodyStream << requestStream.rdbuf();
        request.body = bodyStream.str();
    }

    // 解析Cookie
    if (request.headers.find("Cookie") != request.headers.end()) {
        request.cookie = request.headers["Cookie"];
    }

    // 记录请求日志：包含 IP 地址、路径和 Cookie
    std::string logMessage = "IP: " + clientIPStr + ", Path: " + request.path;
    if (!request.cookie.empty()) {
        logMessage += ", Cookie: " + request.cookie;
    }
    std::cout << Utils::getTimeAndMessage(0, "ACCESS", logMessage) << std::endl;

    // 处理请求，生成响应
    HttpResponse response;
    if (handlers.find(request.path) != handlers.end()) {
        response = handlers[request.path](request);
    } else {
        // 默认返回404 Not Found
        response.status_code = 404;
        response.headers["Content-Type"] = "text/html";
        response.body = Utils::readHTML(std::string(WWW_ADDR) + std::string(_404_PATH) + ".html");
    }

    // 生成HTTP响应字符串
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << response.status_code << " ";

    // 根据状态码添加状态描述
    switch(response.status_code) {
        case 200: responseStream << "OK"; break;
        case 302: responseStream << "Found"; break;
        case 400: responseStream << "Bad Request"; break;
        case 401: responseStream << "Unauthorized"; break;
        case 404: responseStream << "Not Found"; break;
        case 405: responseStream << "Method Not Allowed"; break;
        case 500: responseStream << "Internal Server Error"; break;
        default: responseStream << "OK"; break;
    }
    responseStream << "\r\n";

    // 添加响应头部
    for (const auto& header : response.headers) {
        responseStream << header.first << ": " << header.second << "\r\n";
    }
    responseStream << "Content-Length: " << response.body.size() << "\r\n";
    responseStream << "\r\n";
    responseStream << response.body;

    std::string responseStr = responseStream.str();
    send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

    // 记录响应日志：包含 IP 地址、路径和 Cookie
    std::string responseLog = "IP: " + clientIPStr + ", Path: " + request.path + ", Action: Responded with " + std::to_string(response.status_code);
    if (!request.cookie.empty()) {
        responseLog += ", Cookie: " + request.cookie;
    }
    std::cout << Utils::getTimeAndMessage(0, "ACCESS", responseLog) << std::endl;

    close(clientSocket); // 关闭客户端套接字
}

// 启动并运行服务器，阻塞调用线程
// void NetServer::run() {
//
//     struct sockaddr_in address; // 定义地址结构
//     int opt = 1; // 套接字选项
//     socklen_t addrlen = sizeof(address); // 地址结构的长度
//
//     // 创建套接字
//     serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket creation failed.") << std::endl;
//         return;
//     }
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket creation Success.") << std::endl;
//
//     // 设置套接字选项
//     if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
//         std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket setsockopt failed.") << std::endl;
//         close(serverSocket);
//         return;
//     }
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket setsockopt Success.") << std::endl;
//
//     // 配置地址结构
//     memset(&address, 0, sizeof(address));
//     address.sin_family = AF_INET; // IPv4
//     address.sin_addr.s_addr = INADDR_ANY; // 监听所有可用接口
//     address.sin_port = htons(serverPort); // 设置端口，使用网络字节序
//
//     // 绑定套接字到指定的地址和端口
//     if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         std::cout << Utils::getTimeAndMessage(0, "INFO", "IPV4:PORT Bind Socket failed.") << std::endl;
//         close(serverSocket);
//         return;
//     }
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "IPV4:PORT Bind Socket Success.") << std::endl;
//
//     // 开始监听，设置最大等待连接数
//     if (listen(serverSocket, MAX_CONNECT_NUM) < 0) {
//         std::cout << Utils::getTimeAndMessage(0, "INFO", "Listen Socket failed.") << std::endl;
//         close(serverSocket);
//         return;
//     }
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "Listen Socket Success.") << std::endl;
//
//     isRunning = true;
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "Server started on port " + std::to_string(serverPort)) << std::endl;
//     std::cout << Utils::getTimeAndMessage(0, "INFO", "Waiting for connection...") << std::endl;
//
//     // 主循环，持续接受并处理客户端连接
//     while (isRunning) {
//         struct sockaddr_in clientAddress;
//         socklen_t clientAddrLen = sizeof(clientAddress);
//         int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
//         if (clientSocket < 0) {
//             if (isRunning) { // 如果服务器仍在运行，输出错误信息
//                 std::cout << Utils::getTimeAndMessage(0, "INFO", "Socket accept failed.") << std::endl;
//             }
//             continue; // 继续接受下一个连接
//         }
//
//         // 处理客户端连接
//         handleClient(clientSocket, clientAddress);
//     }
//
//     // 关闭服务器套接字
//     close(serverSocket);
// }