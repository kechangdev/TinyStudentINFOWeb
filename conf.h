//
// Created by KeChang on 6/1/2025.
//

#ifndef CONF_H
#define CONF_H

#define PROJECT_NAME "TinyStudentINFOWeb"
#define AUTHOR "Kechang"
#define GITHUB_URL "https://github.com/Kechangdev"

#define SERVER_PORT 10086
#define MAX_CONNECT_NUM 10
#define THREAD_POOL_SIZE 30

// Mac
// #define WWW_ADDR "/Users/kechang/Codes/TinyStudentINFOWeb/www"
// #define DB_ADDR "/Users/kechang/Codes/TinyStudentINFOWeb/db/autasker.db"

// Laptop
// #define WWW_ADDR "/home/kechang/Codes/TinyStudentINFOWeb/www"
// #define DB_ADDR "/home/kechang/Codes/TinyStudentINFOWeb/db/autasker.db"

// Docker
#define WWW_ADDR "/app/www"
#define DB_ADDR "/app/db/autasker.db"

// API
#define ROOT_PATH "/"
#define _404_PATH "/404"
#define LOGS_PATH "/logs"
#define STUDENT_PATH "/student"
#define INDEX_PATH "/index"
#define LOGIN_PATH "/login"
#define ADMIN_PATH "/admin"
#define REGISTER_PATH "/register"


#endif //CONF_H
