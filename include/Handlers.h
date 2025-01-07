//
// Created by KeChang on 6/1/2025.
//

#ifndef HANDLERS_H
#define HANDLERS_H

#include "NetServer.h"
namespace Handlers {
    // HTML
    // student.html 页面
    HttpResponse studentHandler(const HttpRequest& req);
    // logs.html 页面
    HttpResponse logsHandler(const HttpRequest& req);
    // login.html 页面
    HttpResponse rootHandler(const HttpRequest& req);
    // index.html 页面
    HttpResponse indexHandler(const HttpRequest& req);
    // login.html 页面
    HttpResponse loginHandler(const HttpRequest& req);
    // register.html 页面
    HttpResponse registerHandler(const HttpRequest& req);
    // admin.html 页面
    HttpResponse adminHandler(const HttpRequest& req);

    // API
    // 修改学生信息
    HttpResponse modifyStudentInfo(const HttpRequest& req);
    // 添加学生信息
    HttpResponse addStudentInfo(const HttpRequest& req);
    // 获取学生信息
    HttpResponse getStudentInfo(const HttpRequest& req);
    // 获取日志
    HttpResponse getLogs(const HttpRequest& req);
    // 删除用户
    HttpResponse delUser(const HttpRequest& req);
    // 获取用户列表
    HttpResponse getUserlist(const HttpRequest& req);
    // 判断是否是 Root 用户
    HttpResponse isrootHandler(const HttpRequest& req);
    // 判断是否已经登录（通过 cookie）
    HttpResponse isloginHandler(const HttpRequest& req);
    // 判断登录使用的账号密码是否正确，并登录
    HttpResponse loginVHandler(const HttpRequest& req);
    // 判断是否能注册，并注册
    HttpResponse registerVHandler(const HttpRequest& req);
    // 通过 cookie 获取用户名
    HttpResponse getUsername(const HttpRequest& req);
    // 登出，删除 cookie
    HttpResponse logOut(const HttpRequest& req);
}


#endif //HANDLERS_H
