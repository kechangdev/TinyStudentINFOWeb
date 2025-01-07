#include <iostream>
#include <sstream>
#include "conf.h"
#include "./include/DatabaseManager.h"
#include "./include/NetServer.h"
#include "./include/Utils.h"
#include "include/Handlers.h"

// 处理函数



int main() {
    // 初始化数据库
    if (!DatabaseManager::getInstance().initialize(DB_ADDR)) {
        std::cerr << "Failed to initialize database." << std::endl;
        return 1;
    }
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Database initialized.") << std::endl;

    NetServer server(SERVER_PORT);

    // 设置处理 HTML 请求
    server.setHandler(STUDENT_PATH,Handlers::studentHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Student Handler initialized.") << std::endl;
    server.setHandler(LOGS_PATH,Handlers::logsHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Logs Handler initialized.") << std::endl;
    server.setHandler(ROOT_PATH,Handlers::rootHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Root Handler initialized.") << std::endl;
    server.setHandler(INDEX_PATH, Handlers::indexHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Index Handler initialized.") << std::endl;
    server.setHandler(LOGIN_PATH, Handlers::loginHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Login Handler initialized.") << std::endl;
    server.setHandler(REGISTER_PATH, Handlers::registerHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Register Handler initialized.") << std::endl;
    server.setHandler(ADMIN_PATH, Handlers::adminHandler);
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Admin Handler initialized.") << std::endl;

    // 设置 API
    server.setHandler("/api/modifystudentinfo", Handlers::modifyStudentInfo);
    server.setHandler("/api/addstudentinfo", Handlers::addStudentInfo);
    server.setHandler("/api/getstudentinfo", Handlers::getStudentInfo);
    server.setHandler("/api/getlogs", Handlers::getLogs);
    server.setHandler("/api/deluser", Handlers::delUser);
    server.setHandler("/api/getuserlist", Handlers::getUserlist);
    server.setHandler("/api/isroot",Handlers::isrootHandler);
    server.setHandler("/api/islogin", Handlers::isloginHandler);
    server.setHandler("/api/login", Handlers::loginVHandler);
    server.setHandler("/api/register", Handlers::registerVHandler);
    server.setHandler("/api/username", Handlers::getUsername);
    server.setHandler("/api/logout", Handlers::logOut);

    // 运行服务器（阻塞调用）
    std::cout << Utils::getTimeAndMessage(0, "INFO", "Try to initialize Web Server...") << std::endl;
    server.run();

    // 由于 run() 是阻塞的，以下代码将不会被执行，除非 run() 返回
    return 0;
}