#ifndef NETSERVER_H
#define NETSERVER_H

#include <string>
#include <functional>
#include <unordered_map>
#include <netinet/in.h> // 用于 sockaddr_in 结构体

// 结构体：表示收到的 HTTP 请求
struct HttpRequest {
    std::string method; // HTTP 方法，例如 GET、POST
    std::string path; // 请求的路径，例如 /index.html
    std::unordered_map<std::string, std::string> headers; // 请求头部，键值对形式
    std::string body; // 请求体，包含 POST 数据等
    std::string cookie; // 请求中的 Cookie 信息
};

// 结构体：表示将要发送的 HTTP 响应
struct HttpResponse {
    int status_code; // HTTP 状态码，例如 200、404
    std::unordered_map<std::string, std::string> headers; // 响应头部，键值对形式
    std::string body; // 响应体，包含返回的数据
};

// 类型定义：请求处理函数的类型
using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;

// 类：NetServer
// 一个简单的单线程 HTTP 服务器
class NetServer {
public:
    // 构造函数：初始化服务器，指定监听端口
    NetServer(int port);

    // 析构函数：确保服务器在销毁时停止运行
    ~NetServer();

    // 启动并运行服务器，阻塞调用线程
    void run();

    // 设置特定路径的请求处理函数
    void setHandler(const std::string& path, RequestHandler handler);

private:
    // 处理单个客户端连接的函数
    void handleClient(int clientSocket, const struct sockaddr_in& clientAddress);

    int serverPort; // 服务器监听的端口号
    int serverSocket; // 服务器的套接字描述符
    bool isRunning; // 标志服务器是否正在运行

    // 存储路径与对应处理函数的映射关系
    std::unordered_map<std::string, RequestHandler> handlers;
};

#endif //NETSERVER_H
