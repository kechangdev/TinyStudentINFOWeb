//
// Created by KeChang on 6/1/2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>

static int hexCharToInt(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return -1;
}

namespace Utils {
    // 获取 HTML 文本，以 String 返回
    std::string readHTML(std::string filePath);
    std::string getCurrentTime();

    // 输出 [TIME] [LEVEL] MSG 并记录到数据库
    std::string getTimeAndMessage(int user_id, std::string level, std::string msg);

    // 创建 32 位 Cookie
    std::string generateCookie();

    // URL 解码
    std::string url_decode(const std::string& str);
    std::string urlDecode(const std::string& str);

    // 检查邀请码是否有效
    bool verifyInviteCode(const std::string& inviteCode);

    std::unordered_map<std::string, std::string> parseForm(const std::string& body);

    // 解析 Cookie
    std::unordered_map<std::string, std::string> parseCookies(const std::string& cookieHeader);
    // 辅助函数：修剪字符串两端的空白字符
    std::string trim(const std::string& s);
}

#endif //UTILS_H
