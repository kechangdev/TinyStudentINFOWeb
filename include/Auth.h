//
// Created by KeChang on 5/1/2025.
//
// include/Auth.h

#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <optional>

struct User {
    int id;
    std::string username;
    int permission_level;
};
namespace Auth {
    // 用户登录，返回可选的 cookie 字符串。如果登录失败，返回 std::nullopt
    std::string loginUser(const std::string& username, const std::string& password);
    // 注册新用户，接受用户名、密码和邀请码
    std::string registerUser(const std::string& username, const std::string& password, const std::string& inviteCode);
    // 验证用户的 cookie，若有效，填充 User 结构体
    bool validateCookie(const std::string& cookie);
    // 用户登出，删除对应的 cookie 记录
    bool logoutUser(const std::string& cookie);
    // cookie 是否是 root 用户持有
    bool isRoot(const std::string& cookie);
}

#endif //AUTH_H