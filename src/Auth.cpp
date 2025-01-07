//
// Created by KeChang on 5/1/2025.
//
// src/Auth.cpp

#include "../include/Auth.h"
#include "../include/DatabaseManager.h"
#include "../include/Utils.h"
#include <ctime>
#include <iostream>

std::string Auth::loginUser(const std::string& username, const std::string& password) {
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();
    if (!db) {
        std::cerr << "Database connection is not initialized." << std::endl;
        return "null";
    }

    // 开启事务以确保操作的原子性
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return "null";
    }

    std::string cookie;

    try {
        // 1. 查询用户ID和密码哈希
        const char* sql_select = "SELECT id, password_hash FROM users WHERE username = ?;";
        sqlite3_stmt* stmt_select = nullptr;
        rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << std::endl;
            throw std::runtime_error("Prepare SELECT failed");
        }

        // 绑定用户名参数
        sqlite3_bind_text(stmt_select, 1, username.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt_select);
        if (rc != SQLITE_ROW) {
            // 用户不存在
            std::cerr << "Login failed: User not found for username: " << username << std::endl;
            sqlite3_finalize(stmt_select);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return "null";
        }

        int user_id = sqlite3_column_int(stmt_select, 0);
        const unsigned char* db_password_raw = sqlite3_column_text(stmt_select, 1);
        std::string db_password = db_password_raw ? reinterpret_cast<const char*>(db_password_raw) : "";
        sqlite3_finalize(stmt_select);

        // 2. 验证密码
        if (password != db_password) {
            std::cerr << "Login failed: Invalid password for user: " << username << std::endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return "null";
        }

        // 3. 检查是否已有Cookie
        const char* sql_check_cookie = "SELECT id FROM auth WHERE user_id = ?;";
        sqlite3_stmt* stmt_check = nullptr;
        rc = sqlite3_prepare_v2(db, sql_check_cookie, -1, &stmt_check, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare SELECT auth statement: " << sqlite3_errmsg(db) << std::endl;
            throw std::runtime_error("Prepare SELECT auth failed");
        }

        sqlite3_bind_int(stmt_check, 1, user_id);

        bool has_existing_cookie = false;
        int auth_id = 0;
        rc = sqlite3_step(stmt_check);
        if (rc == SQLITE_ROW) {
            has_existing_cookie = true;
            auth_id = sqlite3_column_int(stmt_check, 0);
        }
        sqlite3_finalize(stmt_check);

        // 4. 如果存在旧Cookie，删除它
        if (has_existing_cookie) {
            const char* sql_delete_cookie = "DELETE FROM auth WHERE id = ?;";
            sqlite3_stmt* stmt_delete = nullptr;
            rc = sqlite3_prepare_v2(db, sql_delete_cookie, -1, &stmt_delete, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "Failed to prepare DELETE auth statement: " << sqlite3_errmsg(db) << std::endl;
                throw std::runtime_error("Prepare DELETE auth failed");
            }

            sqlite3_bind_int(stmt_delete, 1, auth_id);

            rc = sqlite3_step(stmt_delete);
            if (rc != SQLITE_DONE) {
                std::cerr << "Failed to delete existing auth: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt_delete);
                throw std::runtime_error("Delete auth failed");
            }
            sqlite3_finalize(stmt_delete);
        }

        // 5. 生成新的Cookie并插入auth表
        cookie = Utils::generateCookie();
        long expires_at = std::time(nullptr) + 3600; // 1小时后过期

        const char* sql_insert_auth = "INSERT INTO auth (user_id, cookie, expires_at) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt_insert = nullptr;
        rc = sqlite3_prepare_v2(db, sql_insert_auth, -1, &stmt_insert, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare INSERT auth statement: " << sqlite3_errmsg(db) << std::endl;
            throw std::runtime_error("Prepare INSERT auth failed");
        }

        sqlite3_bind_int(stmt_insert, 1, user_id);
        sqlite3_bind_text(stmt_insert, 2, cookie.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt_insert, 3, expires_at);

        rc = sqlite3_step(stmt_insert);
        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to execute INSERT auth statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt_insert);
            throw std::runtime_error("Execute INSERT auth failed");
        }

        sqlite3_finalize(stmt_insert);

        // 提交事务
        rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to commit transaction: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            throw std::runtime_error("Commit transaction failed");
        }

        // 日志记录（假设Logger已经实现）
        // Logger::getInstance().log(user_id, LogLevel::LOGIN, "User logged in: " + username);

    } catch (const std::exception& e) {
        // 出现异常时回滚事务
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        std::cerr << "Exception during loginUser: " << e.what() << std::endl;
        return "null";
    }

    return cookie;
}

std::string Auth::registerUser(const std::string& username, const std::string& password, const std::string& inviteCode) {
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();

    // 验证邀请码
    if (!Utils::verifyInviteCode(inviteCode)) {
        // Logger::getInstance().log(0, LogLevel::INFO, "Invalid invite code used for registration by " + username);
        return "null";
    }

    // 插入新用户 用户名及密码
    const char* sql = "INSERT INTO users (username, password_hash, permission_level) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // Logger::getInstance().log(0, LogLevel::ERROR, "Failed to prepare user insert statement.");
        return "null";
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT); // 存储明文密码
    sqlite3_bind_int(stmt, 3, 1); // 默认权限级别为 1

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // Logger::getInstance().log(0, LogLevel::ERROR, "Failed to execute user insert statement. Possible duplicate username.");
        sqlite3_finalize(stmt);
        return "null";
    }

    sqlite3_finalize(stmt);
    // Logger::getInstance().log(0, LogLevel::INFO, "Registered new user: " + username);
    return Auth::loginUser(username, password);
}

bool Auth::validateCookie(const std::string& cookie) {
    // 获取数据库实例
    sqlite3* db = DatabaseManager::getInstance().getDB();
    if (!db) {
        std::cerr << "数据库未初始化。" << std::endl;
        return false;
    }

    // 准备 SQL 查询语句
    const char* sql = "SELECT expires_at FROM auth WHERE cookie = ?;";
    sqlite3_stmt* stmt = nullptr;

    // 编译 SQL 语句
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "无法准备 SQL 语句: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 绑定 cookie 参数
    rc = sqlite3_bind_text(stmt, 1, cookie.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "无法绑定参数: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool isValid = false;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // 获取 expires_at 字段
        int expires_at = sqlite3_column_int(stmt, 0);

        // 获取当前时间
        std::time_t current_time = std::time(nullptr);

        // 检查 cookie 是否未过期
        if (expires_at > static_cast<int>(current_time)) {
            isValid = true;
        }
    } else if (rc == SQLITE_DONE) {
        // 没有找到对应的 cookie
        isValid = false;
    } else {
        std::cerr << "执行 SQL 语句时出错: " << sqlite3_errmsg(db) << std::endl;
    }

    // 清理资源
    sqlite3_finalize(stmt);
    return isValid;
}

bool Auth::logoutUser(const std::string& cookie) {
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();

    const char* sql = "DELETE FROM auth WHERE cookie = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // Logger::getInstance().log(0, LogLevel::ERROR, "Failed to prepare logout statement.");
        return false;
    }

    sqlite3_bind_text(stmt, 1, cookie.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // Logger::getInstance().log(0, LogLevel::ERROR, "Failed to execute logout statement.");
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}
bool Auth::isRoot(const std::string& cookie) {
    // 获取数据库管理器的实例
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager.getDB();

    if (!db) {
        std::cerr << "Database connection is not initialized." << std::endl;
        return false;
    }

    // 准备查询 auth 表，获取与 cookie 对应的 user_id，且 cookie 未过期
    const char* authQuery = R"(
        SELECT user_id
        FROM auth
        WHERE cookie = ?
          AND expires_at > strftime('%s', 'now')
        LIMIT 1;
    )";

    sqlite3_stmt* authStmt;
    int rc = sqlite3_prepare_v2(db, authQuery, -1, &authStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare auth query: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 绑定 cookie 参数
    rc = sqlite3_bind_text(authStmt, 1, cookie.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind cookie parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(authStmt);
        return false;
    }

    // 执行查询
    rc = sqlite3_step(authStmt);
    if (rc != SQLITE_ROW) {
        // 没有找到对应的记录或 cookie 已过期
        sqlite3_finalize(authStmt);
        return false;
    }

    // 获取 user_id
    int user_id = sqlite3_column_int(authStmt, 0);
    sqlite3_finalize(authStmt);

    // 准备查询 users 表，获取 permission_level
    const char* userQuery = R"(
        SELECT permission_level
        FROM users
        WHERE id = ?
        LIMIT 1;
    )";

    sqlite3_stmt* userStmt;
    rc = sqlite3_prepare_v2(db, userQuery, -1, &userStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare user query: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 绑定 user_id 参数
    rc = sqlite3_bind_int(userStmt, 1, user_id);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind user_id parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(userStmt);
        return false;
    }

    // 执行查询
    rc = sqlite3_step(userStmt);
    if (rc != SQLITE_ROW) {
        // 没有找到对应的用户
        sqlite3_finalize(userStmt);
        return false;
    }

    // 获取 permission_level
    int permission_level = sqlite3_column_int(userStmt, 0);
    sqlite3_finalize(userStmt);

    // 判断是否为根用户
    return (permission_level == 0);
}