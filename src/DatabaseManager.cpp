//
// Created by KeChang on 5/1/2025.
//
// src/DatabaseManager.cpp

#include "../include/DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager() : db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(dbMutex);
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 初始化数据库表
    const char* schema = R"(
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT UNIQUE NOT NULL,
        password_hash TEXT NOT NULL,
        permission_level INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS auth (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id INTEGER NOT NULL,
        cookie TEXT UNIQUE NOT NULL,
        expires_at INTEGER NOT NULL,
        FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE ON UPDATE CASCADE
    );

    CREATE TABLE IF NOT EXISTS logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id INTEGER,
        log_level TEXT NOT NULL,
        message TEXT NOT NULL,
        timestamp INTEGER NOT NULL,
        FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE SET NULL ON UPDATE CASCADE
    );

    CREATE TABLE IF NOT EXISTS student (
        id INTEGER PRIMARY KEY,
        algorithms_score REAL NOT NULL CHECK (algorithms_score BETWEEN 0 AND 100),
        data_structures_score REAL NOT NULL CHECK (data_structures_score BETWEEN 0 AND 100),
        operating_systems_score REAL NOT NULL CHECK (operating_systems_score BETWEEN 0 AND 100),
        databases_score REAL NOT NULL CHECK (databases_score BETWEEN 0 AND 100),
        computer_networks_score REAL NOT NULL CHECK (computer_networks_score BETWEEN 0 AND 100),
        FOREIGN KEY(id) REFERENCES users(id) ON DELETE CASCADE ON UPDATE CASCADE
    );
)";


    char* errMsg = nullptr;
    rc = sqlite3_exec(db, schema, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error during schema initialization: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

sqlite3* DatabaseManager::getDB() {
    return db;
}