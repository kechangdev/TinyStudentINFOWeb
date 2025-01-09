//
// Created by KeChang on 6/1/2025.
//

#include "../include/Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "../include/DatabaseManager.h"
#include <random>
#include "../conf.h"

std::string Utils::readHTML(std::string filePath) {
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (!fileStream) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return "";
    }

    std::ostringstream oss;
    oss << fileStream.rdbuf();
    return oss.str();
}

std::string Utils::getCurrentTime() {
    // 获取当前时间
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // 格式化时间为 YYYY-MM-DD : HH-MM-SS
    char buffer[30]; // 足够存储格式化的时间字符串
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d:%H-%M-%S", localTime);

    return std::string(buffer);
}

// std::string Utils::getTimeAndMessage(int user_id, std::string level, std::string msg) {
//     // 获取当前时间
//     std::time_t now = std::time(nullptr);
//     std::tm* localTime = std::localtime(&now);
//
//     // 格式化时间为 YYYY-MM-DD : HH-MM-SS
//     char buffer[30]; // 足够存储格式化的时间字符串
//     std::strftime(buffer, sizeof(buffer), "%Y-%m-%d:%H-%M-%S", localTime);
//     std::string time = std::string(buffer);
//
//     std::string res = "[" + time + "] " + "[" + level + "] " + msg;
//
//     DatabaseManager& dbManager = DatabaseManager::getInstance();
//     sqlite3* db = dbManager.getDB();
//     const char* sql = "INSERT INTO logs (user_id, log_level, message, timestamp) VALUES (?, ?, ?, ?);";
//     sqlite3_stmt* stmt;
//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
//         // std::cerr << "Failed to prepare log insert statement: " << sqlite3_errmsg(db) << std::endl;
//         return res + " & " + "Failed to prepare log insert statement: " + sqlite3_errmsg(db);
//     }
//     sqlite3_bind_int(stmt, 1, user_id);
//     sqlite3_bind_text(stmt, 2, level.c_str(), -1, SQLITE_TRANSIENT);
//     sqlite3_bind_text(stmt, 3, msg.c_str(), -1, SQLITE_TRANSIENT);
//     sqlite3_bind_int64(stmt, 4, now);
//     // 执行插入操作
//     if (sqlite3_step(stmt) != SQLITE_DONE) {
//         // std::cerr << "Failed to execute log insert statement: " << sqlite3_errmsg(db) << std::endl;
//         return res + " & " + "Failed to execute log insert statement: " + sqlite3_errmsg(db);
//     }
//
//     // 释放语句对象
//     sqlite3_finalize(stmt);
//
//     return res;
// }
std::string Utils::getTimeAndMessage(int user_id, std::string level, std::string msg) {
    // 1. 获取本地时间、格式化为字符串
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d:%H-%M-%S", localTime);
    std::string time = std::string(buffer);

    // 2. 构建返回日志字符串（此处与数据库无关）
    std::string res = "[" + time + "] " + "[" + level + "] " + msg;

    // 3. 在执行数据库操作前，先加锁
    {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().dbMutex);

        DatabaseManager& dbManager = DatabaseManager::getInstance();
        sqlite3* db = dbManager.getDB();
        if (!db) {
            // 如果 db 尚未初始化，可以直接返回
            return res + " & " + "Database not initialized.";
        }

        const char* sql = "INSERT INTO logs (user_id, log_level, message, timestamp) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;

        // 准备语句
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::string err = sqlite3_errmsg(db);
            return res + " & " + "Failed to prepare log insert statement: " + err;
        }

        // 绑定参数
        sqlite3_bind_int(stmt, 1, user_id);
        sqlite3_bind_text(stmt, 2, level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, msg.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 4, now);

        // 执行插入
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            return res + " & " + "Failed to execute log insert statement: " + err;
        }

        // 释放语句对象
        sqlite3_finalize(stmt);
    }

    // 4. 返回最终日志字符串（若数据库插入成功，则不附带错误信息）
    return res;
}

std::string Utils::generateCookie() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t max_index = sizeof(charset) - 1;
    std::string cookie;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(0, max_index - 1);
    for (int i = 0; i < 32; ++i) {
        cookie += charset[dist(rng)];
    }
    return cookie;
}

bool Utils::verifyInviteCode(const std::string& inviteCode) {
    return inviteCode == "INVITECODE";
}

std::string Utils::url_decode(const std::string& str) {
    std::string decoded;
    char ch;
    int i, ii;
    for (i = 0; i < str.length(); ++i) {
        if (str[i] == '%') {
            if (i + 2 < str.length()) {
                std::istringstream iss(str.substr(i + 1, 2));
                iss >> std::hex >> ii;
                ch = static_cast<char>(ii);
                decoded += ch;
                i += 2;
            }
        }
        else if (str[i] == '+') {
            decoded += ' ';
        }
        else {
            decoded += str[i];
        }
    }
    return decoded;
}


std::string Utils::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}


std::unordered_map<std::string, std::string> Utils::parseCookies(const std::string& cookieHeader) {
    std::unordered_map<std::string, std::string> cookies;
    std::stringstream ss(cookieHeader);
    std::string item;

    while (std::getline(ss, item, ';')) {
        size_t pos = item.find('=');
        if (pos != std::string::npos) {
            std::string key = Utils::trim(item.substr(0, pos));
            std::string value = Utils::trim(item.substr(pos + 1));
            cookies[key] = value;
        }
    }

    return cookies;
}

// URL 解码函数
std::string Utils::urlDecode(const std::string& str) {
    std::string decoded;
    decoded.reserve(str.size());

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '+') {
            // '+' 转换为空格
            decoded += ' ';
        }
        else if (str[i] == '%' && i + 2 < str.size()) {
            // 处理百分号编码
            int high = hexCharToInt(str[i + 1]);
            int low = hexCharToInt(str[i + 2]);

            if (high == -1 || low == -1) {
                // 无效的百分号编码，保留原字符
                decoded += '%';
            }
            else {
                char byte = static_cast<char>((high << 4) | low);
                decoded += byte;
                i += 2; // 跳过已经处理的两个字符
            }
        }
        else {
            // 其他字符保持不变
            decoded += str[i];
        }
    }

    return decoded;
}
// 解析表单数据函数
std::unordered_map<std::string, std::string> Utils::parseForm(const std::string& body) {
    std::unordered_map<std::string, std::string> formData;
    std::stringstream ss(body);
    std::string pair;

    while (std::getline(ss, pair, '&')) {
        if (pair.empty()) continue; // 跳过空字符串

        size_t pos = pair.find('=');
        if (pos == std::string::npos) {
            // 没有 '=', 只包含键，值为空
            std::string key = urlDecode(pair);
            formData[key] = "";
        }
        else {
            // 分割键和值
            std::string key = urlDecode(pair.substr(0, pos));
            std::string value = urlDecode(pair.substr(pos + 1));
            formData[key] = value;
        }
    }

    return formData;
}