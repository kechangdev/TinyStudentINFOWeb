//
// Created by KeChang on 5/1/2025.
//

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <mutex>
#include <string>

class DatabaseManager {
public:
    static DatabaseManager& getInstance();

    bool initialize(const std::string& dbPath);
    sqlite3* getDB();

    // 禁止通过拷贝构造函数和拷贝赋值操作符复制 DatabaseManager 实例，确保单例模式的唯一性，避免多个实例导致资源竞争或不一致。
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    std::mutex dbMutex;
private:
    DatabaseManager();
    ~DatabaseManager();

    sqlite3* db;
};


#endif //DATABASEMANAGER_H
