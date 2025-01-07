//
// Created by KeChang on 6/1/2025.
//

#include "../include/Handlers.h"

#include <iostream>
#include <ostream>
#include <sstream>
#include "../include/NetServer.h"
#include "../include/Utils.h"
#include "../include/DatabaseManager.h"
#include "../include/Auth.h"
#include "../conf.h"

// HTML
HttpResponse Handlers::studentHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(STUDENT_PATH) + ".html");
    return res;
}
HttpResponse Handlers::logsHandler(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.body = "Missing authentication cookie.";
        return res;
    }

    if (!Auth::isRoot(cookie)) {
        res.status_code = 400;
        res.headers["Content-Type"] = "text/plain";
        res.body = "权限不足";
        return res;
    }
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(LOGS_PATH) + ".html");
    return res;
}
HttpResponse Handlers::rootHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(ROOT_PATH) + "login.html");
    return res;
}
HttpResponse Handlers::indexHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(INDEX_PATH) + ".html");
    return res;
}
HttpResponse Handlers::loginHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(LOGIN_PATH) + ".html");
    return res;
}
HttpResponse Handlers::registerHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(REGISTER_PATH) + ".html");
    return res;
}
HttpResponse Handlers::adminHandler(const HttpRequest& req) {
    HttpResponse res;
    res.status_code = 200;
    res.headers["Content-Type"] = "html";
    res.body = Utils::readHTML(static_cast<std::string>(WWW_ADDR) + static_cast<std::string>(ADMIN_PATH) + ".html");
    return res;
}

// API

// 登录验证
HttpResponse Handlers::loginVHandler(const HttpRequest& req) {
    HttpResponse res;

    // 直接解析请求体，假设为 application/x-www-form-urlencoded 格式
    std::string username;
    std::string password;

    // 查找并提取 username
    size_t pos = req.body.find("username=");
    if (pos != std::string::npos) {
        size_t start = pos + 9; // "username=".length() == 9
        size_t end = req.body.find('&', start);
        if (end == std::string::npos) {
            end = req.body.length();
        }
        username = Utils::url_decode(req.body.substr(start, end - start));
    }

    // 查找并提取 password
    pos = req.body.find("password=");
    if (pos != std::string::npos) {
        size_t start = pos + 9; // "password=".length() == 9
        size_t end = req.body.find('&', start);
        if (end == std::string::npos) {
            end = req.body.length();
        }
        password = Utils::url_decode(req.body.substr(start, end - start));
    }

    // 检查是否提供了用户名和密码
    if (username.empty() || password.empty()) {
        res.status_code = 400; // Bad Request
        res.headers["Content-Type"] = "text/plain";
        res.body = "用户名和密码是必需的。";
        return res;
    }

    // 尝试登录
    std::string cookie = Auth::loginUser(username, password);

    if (cookie != "null") {
        // 登录成功
        res.status_code = 200;
        res.headers["Content-Type"] = "text/plain";
        res.headers["Set-Cookie"] = "cookie=" + cookie + "; path=/";
        res.body = "登录成功!";
    }
    else {
        // 登录失败
        res.status_code = 401; // Unauthorized
        res.headers["Content-Type"] = "text/plain";
        res.body = "用户名或密码错误，请重试。";
    }

    return res;
}

// 注册
HttpResponse Handlers::registerVHandler(const HttpRequest& req) {
    HttpResponse res;

    // 直接解析请求体，假设为 application/x-www-form-urlencoded 格式
    std::string username;
    std::string password;
    std::string invite_code;

    // 查找并提取 username
    size_t pos = req.body.find("username=");
    if (pos != std::string::npos) {
        size_t start = pos + 9; // "username=".length() == 9
        size_t end = req.body.find('&', start);
        if (end == std::string::npos) {
            end = req.body.length();
        }
        username = Utils::url_decode(req.body.substr(start, end - start));
    }

    // 查找并提取 password
    pos = req.body.find("password=");
    if (pos != std::string::npos) {
        size_t start = pos + 9; // "password=".length() == 9
        size_t end = req.body.find('&', start);
        if (end == std::string::npos) {
            end = req.body.length();
        }
        password = Utils::url_decode(req.body.substr(start, end - start));
    }

    // 查找并提取 invite_code
    pos = req.body.find("invite_code=");
    if (pos != std::string::npos) {
        size_t start = pos + 12; // "invite_code=".length() == 12
        size_t end = req.body.find('&', start);
        if (end == std::string::npos) {
            end = req.body.length();
        }
        invite_code = Utils::url_decode(req.body.substr(start, end - start));
    }

    // 检查是否提供了用户名和密码
    if (username.empty() || password.empty() || invite_code.empty()) {
        res.status_code = 400; // Bad Request
        res.headers["Content-Type"] = "text/plain";
        res.body = "用户名和密码和邀请码是必需的。";
        return res;
    }

    if (!Utils::verifyInviteCode(invite_code)) {
        res.status_code = 400; // Bad Request
        res.headers["Content-Type"] = "text/plain";
        res.body = "该邀请码无效";
        return res;
    }

    std::string cookie = Auth::registerUser(username,password,invite_code);
    if (cookie == "null") {
        res.status_code = 400; // Bad Request
        res.headers["Content-Type"] = "text/plain";
        res.body = "用户名已被占用";
        return res;
    }

    // 注册成功，登录成功
    res.status_code = 200;
    res.headers["Content-Type"] = "text/plain";
    res.headers["Set-Cookie"] = "cookie=" + cookie + "; path=/";
    res.body = "注册成功!";
    return res;
}

HttpResponse Handlers::getUsername(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.body = "Missing authentication cookie.";
        return res;
    }

    sqlite3* db = DatabaseManager::getInstance().getDB();
    if (!db) {
        res.status_code = 500; // Internal Server Error
        res.body = "Database connection not initialized.";
        return res;
    }

    // 第一步：根据 cookie 查询 user_id
    const char* auth_query = "SELECT user_id FROM auth WHERE cookie = ? AND expires_at > strftime('%s','now');";
    sqlite3_stmt* auth_stmt;
    int rc = sqlite3_prepare_v2(db, auth_query, -1, &auth_stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "Failed to prepare auth statement: " << sqlite3_errmsg(db) << std::endl;
        res.status_code = 500;
        res.body = "Database query preparation failed.";
        return res;
    }

    // 绑定 cookie 参数
    sqlite3_bind_text(auth_stmt, 1, cookie.c_str(), -1, SQLITE_STATIC);

    // 执行查询
    rc = sqlite3_step(auth_stmt);
    if (rc == SQLITE_ROW) {
        // 获取 user_id
        int user_id = sqlite3_column_int(auth_stmt, 0);
        sqlite3_finalize(auth_stmt);

        // 第二步：根据 user_id 查询用户名
        const char* user_query = "SELECT username FROM users WHERE id = ?;";
        sqlite3_stmt* user_stmt;
        rc = sqlite3_prepare_v2(db, user_query, -1, &user_stmt, nullptr);
        if (rc != SQLITE_OK) {
            // std::cerr << "Failed to prepare user statement: " << sqlite3_errmsg(db) << std::endl;
            res.status_code = 500;
            res.body = "Database query preparation failed.";
            return res;
        }

        // 绑定 user_id 参数
        sqlite3_bind_int(user_stmt, 1, user_id);

        // 执行查询
        rc = sqlite3_step(user_stmt);
        if (rc == SQLITE_ROW) {
            // 获取用户名
            const unsigned char* username_text = sqlite3_column_text(user_stmt, 0);
            if (username_text) {
                res.body = reinterpret_cast<const char*>(username_text);
                res.status_code = 200; // OK
                res.headers["Content-Type"] = "text/plain";
            } else {
                res.status_code = 500;
                res.headers["Content-Type"] = "text/plain";
                res.body = "Failed to retrieve username.";
            }
        } else {
            res.status_code = 404; // Not Found
            res.headers["Content-Type"] = "text/plain";
            res.body = "User not found.";
        }

        sqlite3_finalize(user_stmt);
    } else if (rc == SQLITE_DONE) {
        // 未找到对应的 cookie
        sqlite3_finalize(auth_stmt);
        res.status_code = 401; // Unauthorized
        res.headers["Content-Type"] = "text/plain";
        res.headers["Set-Cookie"] = "cookie=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/";
        res.body = "Invalid or expired authentication cookie.";
    } else {
        // 查询失败
        // std::cerr << "Failed to execute auth statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(auth_stmt);
        res.status_code = 500;
        res.body = "Database query failed.";
    }

    return res;
}

HttpResponse Handlers::logOut(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];

    if (Auth::logoutUser(cookie)) {
        res.status_code = 200; // OK
        res.headers["Content-Type"] = "text/plain";
        res.headers["Set-Cookie"] = "cookie=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/";
        res.body = "success";
    }
    else {
        res.status_code = 200; // OK
        res.headers["Content-Type"] = "text/plain";
        res.headers["Set-Cookie"] = "cookie=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/";
        res.body = "success";
    }
    return res;
}

HttpResponse Handlers::isloginHandler(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];

    if (Auth::validateCookie(cookie)) res.status_code = 200;
    else res.status_code = 400;
    return res;
}
HttpResponse Handlers::isrootHandler(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];

    if (Auth::isRoot(cookie)) res.status_code = 200;
    else res.status_code = 400;
    return res;
}
HttpResponse Handlers::getUserlist(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];

    // 验证是否为管理员
    if (!Auth::isRoot(cookie)) {
        res.status_code = 403; // 403 Forbidden 更合适
        res.body = "权限不足。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 获取数据库实例
    sqlite3* db = DatabaseManager::getInstance().getDB();
    if (!db) {
        res.status_code = 500;
        res.body = "服务器内部错误：无法连接到数据库。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 查询用户列表，仅选择 id 和 username
    const char* sql = "SELECT id, username FROM users;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        res.status_code = 500;
        res.body = "服务器内部错误：无法准备数据库查询。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 执行查询并构建响应体
    std::ostringstream oss;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username_text = sqlite3_column_text(stmt, 1);
        std::string username = username_text ? reinterpret_cast<const char*>(username_text) : "";

        // 格式化为 "id,username\n"
        oss << id << "," << username << "\n";
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "SQL step error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        res.status_code = 500;
        res.body = "服务器内部错误：数据库查询失败。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    sqlite3_finalize(stmt);

    // 设置响应
    res.status_code = 200;
    res.body = oss.str();
    res.headers["Content-Type"] = "text/plain";

    return res;
}
HttpResponse Handlers::delUser(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    auto it = cookies.find("cookie");
    if (it == cookies.end()) {
        res.status_code = 400; // Bad Request
        res.body = "缺少认证信息。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    std::string cookie = it->second;

    // 验证是否为管理员
    if (!Auth::isRoot(cookie)) {
        res.status_code = 403; // 403 Forbidden
        res.body = "权限不足。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 解析用户ID
    std::string id_str = req.body;
    int user_id;
    try {
        user_id = std::stoi(id_str);
    } catch (const std::invalid_argument&) {
        res.status_code = 400; // Bad Request
        res.body = "无效的用户ID。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    } catch (const std::out_of_range&) {
        res.status_code = 400; // Bad Request
        res.body = "用户ID超出范围。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    if (user_id == 1) {
        res.status_code = 500; // Bad Request
        res.body = "Root Cannot Delete rootself";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    // 获取数据库实例
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();

    if (!db) {
        res.status_code = 500; // Internal Server Error
        res.body = "Database not inited.";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    char* errMsg = nullptr;

    // 开始事务
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法开始事务: " << errMsg << std::endl;
        res.status_code = 500;
        res.body = "Cannot begin transaction.";
        res.headers["Content-Type"] = "text/plain";
        sqlite3_free(errMsg);
        return res;
    }

    // 定义删除语句
    const char* delete_auth_sql = "DELETE FROM auth WHERE user_id = ?;";
    const char* delete_tasks_sql = "DELETE FROM tasks WHERE user_id = ?;";
    const char* delete_logs_sql = "DELETE FROM logs WHERE user_id = ?;";
    const char* delete_user_sql = "DELETE FROM users WHERE id = ?;";

    // 删除 auth 表中的记录
    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, delete_auth_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法准备删除 auth 语句: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // std::cerr << "删除 auth 失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    sqlite3_finalize(stmt);

    // 删除 tasks 表中的记录
    rc = sqlite3_prepare_v2(db, delete_tasks_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法准备删除 tasks 语句: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // std::cerr << "删除 tasks 失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    sqlite3_finalize(stmt);

    // 删除 logs 表中的记录
    rc = sqlite3_prepare_v2(db, delete_logs_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法准备删除 logs 语句: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // std::cerr << "删除 logs 失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    sqlite3_finalize(stmt);

    // 删除 users 表中的记录
    rc = sqlite3_prepare_v2(db, delete_user_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法准备删除 users 语句: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // std::cerr << "删除 users 失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }
    sqlite3_finalize(stmt);

    // 提交事务
    rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        // std::cerr << "无法提交事务: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        res.status_code = 500;
        res.body = "Database wrong";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 设置成功响应
    res.status_code = 200; // OK
    res.body = "success";
    res.headers["Content-Type"] = "text/plain";
    return res;
}
HttpResponse Handlers::getLogs(const HttpRequest& req) {
    HttpResponse res;

    // 1. 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.body = "Missing authentication cookie.";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 2. 验证是否为管理员
    if (!Auth::isRoot(cookie)) {
        res.status_code = 403; // Forbidden
        res.body = "权限不足。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 3. 获取数据库实例
    sqlite3* db = DatabaseManager::getInstance().getDB();
    if (!db) {
        res.status_code = 500; // Internal Server Error
        res.body = "数据库未初始化。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 4. 准备 SQL 查询
    const char* sql = "SELECT id, timestamp, log_level, user_id, message FROM logs ORDER BY id DESC;";

    // const char* sql = "SELECT id, timestamp, log_level, user_id, message FROM logs ORDER BY timestamp DESC;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        res.status_code = 500; // Internal Server Error
        res.body = "准备数据库查询失败。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 5. 执行查询并构建响应体
    std::ostringstream oss;
    bool firstRow = true;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!firstRow) {
            oss << "\n";
        } else {
            firstRow = false;
        }

        // 获取每一列的数据
        int id = sqlite3_column_int(stmt, 0);
        int timestamp = sqlite3_column_int(stmt, 1);
        const unsigned char* log_level_text = sqlite3_column_text(stmt, 2);
        int user_id = sqlite3_column_int(stmt, 3);
        const unsigned char* message_text = sqlite3_column_text(stmt, 4);

        // 转换为字符串，处理 NULL 值
        std::string log_level = log_level_text ? reinterpret_cast<const char*>(log_level_text) : "";
        std::string message = message_text ? reinterpret_cast<const char*>(message_text) : "";

        // 将 Unix 时间戳转换为可读格式（可选）
        // 如果前端需要 Unix 时间戳，可以直接使用 timestamp
        // 这里保持 Unix 时间戳格式
        // 如果需要可读时间，可以转换为字符串，例如使用 std::put_time

        // 格式化日志行
        oss << id << "," << timestamp << "," << log_level << "," << user_id << "," << message;
    }

    // 检查查询是否成功完成
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        res.status_code = 500; // Internal Server Error
        res.body = "执行数据库查询失败。";
        res.headers["Content-Type"] = "text/plain";
        return res;
    }

    // 6. 清理
    sqlite3_finalize(stmt);

    // 7. 设置响应
    res.status_code = 200; // OK
    res.body = oss.str();
    res.headers["Content-Type"] = "text/plain";

    return res;
}
//
// HttpResponse Handlers::getStudentInfo(const HttpRequest& req) {
//     HttpResponse res;
//
//     // 提取 cookie
//     std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
//     std::string cookie = cookies["cookie"];
//     if (cookie.empty()) {
//         res.status_code = 400; // Bad Request
//         res.body = "Missing authentication cookie.";
//         return res;
//     }
//
//     // TODO：针对不同cookie获取不同成绩，封装到响应体中
//     // TODO：对于 permission_level 为 1 的用户，仅展示他自己的各科信息；对于 permission_level 为 0 的用户，展示所有用户的信息
//
//     return res;
// }
// HttpResponse addStudentInfo(const HttpRequest& req) {
//     HttpResponse res;
//
//     // 提取 cookie
//     std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
//     std::string cookie = cookies["cookie"];
//     if (cookie.empty()) {
//         res.status_code = 400; // Bad Request
//         res.body = "Missing authentication cookie.";
//         return res;
//     }
//     // TODO: 判断是否是 permission_level 为 0 的用户，如果不是，不允许添加
//     // TODO: 如果 permission_level 为 0， 允许添加学生信息（默认添加密码 cdtu）
//
//     return res;
// }
// HttpResponse modifyStudentInfo(const HttpRequest& req) {
//     HttpResponse res;
//
//     // 提取 cookie
//     std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
//     std::string cookie = cookies["cookie"];
//     if (cookie.empty()) {
//         res.status_code = 400; // Bad Request
//         res.body = "Missing authentication cookie.";
//         return res;
//     }
//     // TODO: 判断是否是 permission_level 为 0 的用户，如果不是，不允许修改
//     // TODO: 如果 permission_level 为 0， 允许修改学生信息
//
//     return res;
// }
HttpResponse Handlers::getStudentInfo(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.body = "Missing authentication cookie.";
        return res;
    }

    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();
    if (!db) {
        res.status_code = 500;
        res.body = "Database not initialized.";
        return res;
    }

    // 获取用户信息
    const char* sql = "SELECT users.id, users.username, users.permission_level FROM auth "
                      "JOIN users ON auth.user_id = users.id WHERE auth.cookie = ? AND auth.expires_at > strftime('%s','now');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        res.status_code = 500;
        res.body = "Database error.";
        return res;
    }
    sqlite3_bind_text(stmt, 1, cookie.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.status_code = 403; // Forbidden
        res.body = "Invalid or expired cookie.";
        return res;
    }

    int user_id = sqlite3_column_int(stmt, 0);
    std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int permission_level = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);

    // 根据权限级别查询学生信息
    if (permission_level == 0) {
        // 管理员查看所有学生
        const char* sql_all = "SELECT users.id, users.username, student.algorithms_score, student.data_structures_score, "
                              "student.operating_systems_score, student.databases_score, student.computer_networks_score "
                              "FROM users JOIN student ON users.id = student.id;";
        sqlite3_stmt* stmt_all;
        if (sqlite3_prepare_v2(db, sql_all, -1, &stmt_all, nullptr) != SQLITE_OK) {
            res.status_code = 500;
            res.body = "Database error.";
            return res;
        }

        std::string response = "ADMIN\n";
        while ((rc = sqlite3_step(stmt_all)) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt_all, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_all, 1));
            double algorithms = sqlite3_column_double(stmt_all, 2);
            double data_structures = sqlite3_column_double(stmt_all, 3);
            double operating_systems = sqlite3_column_double(stmt_all, 4);
            double databases = sqlite3_column_double(stmt_all, 5);
            double computer_networks = sqlite3_column_double(stmt_all, 6);

            response += std::to_string(id) + "," + name + "," +
                        std::to_string(algorithms) + "," +
                        std::to_string(data_structures) + "," +
                        std::to_string(operating_systems) + "," +
                        std::to_string(databases) + "," +
                        std::to_string(computer_networks) + "\n";
        }
        sqlite3_finalize(stmt_all);
        res.status_code = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = response;
        return res;
    } else if (permission_level == 1) {
        // 学生查看自己的信息
        const char* sql_self = "SELECT student.algorithms_score, student.data_structures_score, "
                               "student.operating_systems_score, student.databases_score, "
                               "student.computer_networks_score "
                               "FROM student WHERE student.id = ?;";
        sqlite3_stmt* stmt_self;
        if (sqlite3_prepare_v2(db, sql_self, -1, &stmt_self, nullptr) != SQLITE_OK) {
            res.status_code = 500;
            res.body = "Database error.";
            return res;
        }
        sqlite3_bind_int(stmt_self, 1, user_id);
        rc = sqlite3_step(stmt_self);
        if (rc != SQLITE_ROW) {
            sqlite3_finalize(stmt_self);
            res.status_code = 404;
            res.body = "Student information not found.";
            return res;
        }

        double algorithms = sqlite3_column_double(stmt_self, 0);
        double data_structures = sqlite3_column_double(stmt_self, 1);
        double operating_systems = sqlite3_column_double(stmt_self, 2);
        double databases = sqlite3_column_double(stmt_self, 3);
        double computer_networks = sqlite3_column_double(stmt_self, 4);

        std::string response = "STUDENT\n" +
                               std::to_string(algorithms) + "," +
                               std::to_string(data_structures) + "," +
                               std::to_string(operating_systems) + "," +
                               std::to_string(databases) + "," +
                               std::to_string(computer_networks) + "\n";
        sqlite3_finalize(stmt_self);
        res.status_code = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = response;
        return res;
    } else {
        res.status_code = 403;
        res.body = "Invalid permission level.";
        return res;
    }
}
HttpResponse Handlers::addStudentInfo(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.body = "Missing authentication cookie.";
        return res;
    }

    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();
    if (!db) {
        res.status_code = 500;
        res.body = "Database not initialized.";
        return res;
    }

    // 获取用户信息
    const char* sql = "SELECT users.id, users.permission_level FROM auth "
                      "JOIN users ON auth.user_id = users.id WHERE auth.cookie = ? AND auth.expires_at > strftime('%s','now');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        res.status_code = 500;
        res.body = "Database error.";
        return res;
    }
    sqlite3_bind_text(stmt, 1, cookie.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.status_code = 403; // Forbidden
        res.body = "Invalid or expired cookie.";
        return res;
    }

    int user_id = sqlite3_column_int(stmt, 0);
    int permission_level = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);

    if (permission_level != 0) {
        res.status_code = 403; // Forbidden
        res.body = "Insufficient permissions to add student info.";
        return res;
    }

    // 解析表单数据
    std::unordered_map<std::string, std::string> form = Utils::parseForm(req.body);
    std::string id_str = form["id"];
    std::string name = form["name"];
    std::string algorithms_str = form["algorithms"];
    std::string data_structures_str = form["data_structures"];
    std::string operating_systems_str = form["operating_systems"];
    std::string databases_str = form["databases"];
    std::string computer_networks_str = form["computer_networks"];

    if (id_str.empty() || name.empty() || algorithms_str.empty() ||
        data_structures_str.empty() || operating_systems_str.empty() ||
        databases_str.empty() || computer_networks_str.empty()) {
        res.status_code = 400;
        res.body = "Missing required fields.";
        return res;
    }

    int id = std::stoi(id_str);
    double algorithms = std::stod(algorithms_str);
    double data_structures = std::stod(data_structures_str);
    double operating_systems = std::stod(operating_systems_str);
    double databases = std::stod(databases_str);
    double computer_networks = std::stod(computer_networks_str);

    // 开始事务
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        res.status_code = 500;
        res.body = "Database transaction error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        return res;
    }

    // 插入到 users 表
    const char* insert_user_sql = "INSERT INTO users (id, username, password_hash, permission_level) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* insert_user_stmt;
    if (sqlite3_prepare_v2(db, insert_user_sql, -1, &insert_user_stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database error.";
        return res;
    }

    std::string password = name;
    std::string password_hash = password; // 假设 Utils::hashPassword 存在

    sqlite3_bind_int(insert_user_stmt, 1, id);
    sqlite3_bind_text(insert_user_stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_user_stmt, 3, password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(insert_user_stmt, 4, 1); // permission_level 1 表示学生

    rc = sqlite3_step(insert_user_stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(insert_user_stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Error inserting user. Possible duplicate ID or username.";
        return res;
    }
    sqlite3_finalize(insert_user_stmt);

    // 插入到 student 表
    const char* insert_student_sql = "INSERT INTO student (id, algorithms_score, data_structures_score, operating_systems_score, "
                                     "databases_score, computer_networks_score) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* insert_student_stmt;
    if (sqlite3_prepare_v2(db, insert_student_sql, -1, &insert_student_stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Database error.";
        return res;
    }

    sqlite3_bind_int(insert_student_stmt, 1, id);
    sqlite3_bind_double(insert_student_stmt, 2, algorithms);
    sqlite3_bind_double(insert_student_stmt, 3, data_structures);
    sqlite3_bind_double(insert_student_stmt, 4, operating_systems);
    sqlite3_bind_double(insert_student_stmt, 5, databases);
    sqlite3_bind_double(insert_student_stmt, 6, computer_networks);

    rc = sqlite3_step(insert_student_stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(insert_student_stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.status_code = 500;
        res.body = "Error inserting student information.";
        return res;
    }
    sqlite3_finalize(insert_student_stmt);

    // 提交事务
    rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        res.status_code = 500;
        res.body = "Database commit error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        return res;
    }

    res.status_code = 200;
    res.headers["Content-Type"] = "text/plain";
    res.body = "Student added successfully.";
    return res;
}
HttpResponse Handlers::modifyStudentInfo(const HttpRequest& req) {
    HttpResponse res;

    // 提取 cookie
    std::unordered_map<std::string, std::string> cookies = Utils::parseCookies(req.cookie);
    std::string cookie = cookies["cookie"];
    if (cookie.empty()) {
        res.status_code = 400; // Bad Request
        res.headers["Content-Type"] = "text/plain";
        res.body = "Missing authentication cookie.";
        return res;
    }

    if (!Auth::isRoot(cookie)) {
        res.status_code = 400;
        res.headers["Content-Type"] = "text/plain";
        res.body = "Root authentication failed.";
        return res;
    }
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();
    if (!db) {
        res.status_code = 500;
        res.headers["Content-Type"] = "text/plain";
        res.body = "Database not initialized.";
        return res;
    }

    // 获取用户信息
    const char* sql = "SELECT users.id, users.permission_level FROM auth "
                      "JOIN users ON auth.user_id = users.id WHERE auth.cookie = ? AND auth.expires_at > strftime('%s','now');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        res.status_code = 500;
        res.headers["Content-Type"] = "text/plain";
        res.body = "Database error.";
        return res;
    }
    sqlite3_bind_text(stmt, 1, cookie.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.status_code = 403; // Forbidden
        res.headers["Content-Type"] = "text/plain";
        res.body = "Invalid or expired cookie.";
        return res;
    }

    int user_id = sqlite3_column_int(stmt, 0);
    int permission_level = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);

    if (permission_level != 0) {
        res.status_code = 403; // Forbidden
        res.headers["Content-Type"] = "text/plain";
        res.body = "Insufficient permissions to modify student info.";
        return res;
    }

    // 解析表单数据
    std::unordered_map<std::string, std::string> form = Utils::parseForm(req.body);
    std::string id_str = form["id"];
    std::string algorithms_str = form["algorithms"];
    std::string data_structures_str = form["data_structures"];
    std::string operating_systems_str = form["operating_systems"];
    std::string databases_str = form["databases"];
    std::string computer_networks_str = form["computer_networks"];

    if (id_str.empty() || algorithms_str.empty() ||
        data_structures_str.empty() || operating_systems_str.empty() ||
        databases_str.empty() || computer_networks_str.empty()) {
        res.status_code = 400;
        res.body = "Missing required fields.";
        return res;
    }

    int id = std::stoi(id_str);
    double algorithms = std::stod(algorithms_str);
    double data_structures = std::stod(data_structures_str);
    double operating_systems = std::stod(operating_systems_str);
    double databases = std::stod(databases_str);
    double computer_networks = std::stod(computer_networks_str);

    // 更新 student 表
    const char* update_sql = "UPDATE student SET algorithms_score = ?, data_structures_score = ?, "
                             "operating_systems_score = ?, databases_score = ?, computer_networks_score = ? "
                             "WHERE id = ?;";
    sqlite3_stmt* update_stmt;
    if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, nullptr) != SQLITE_OK) {
        res.status_code = 500;
        res.body = "Database error.";
        return res;
    }

    sqlite3_bind_double(update_stmt, 1, algorithms);
    sqlite3_bind_double(update_stmt, 2, data_structures);
    sqlite3_bind_double(update_stmt, 3, operating_systems);
    sqlite3_bind_double(update_stmt, 4, databases);
    sqlite3_bind_double(update_stmt, 5, computer_networks);
    sqlite3_bind_int(update_stmt, 6, id);

    rc = sqlite3_step(update_stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(update_stmt);
        res.status_code = 500;
        res.body = "Error updating student information.";
        return res;
    }
    sqlite3_finalize(update_stmt);

    res.status_code = 200;
    res.headers["Content-Type"] = "text/plain";
    res.body = "Student information updated successfully.";
    return res;
}