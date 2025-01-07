cmake_minimum_required(VERSION 3.30)
project(TinyStudentINFOWeb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找SQLite3库
find_package(SQLite3 REQUIRED)

# 包含SQLite3头文件和源代码目录
include_directories(${SQLite3_INCLUDE_DIRS} src)

add_executable(
        TinyStudentINFOWeb
        main.cpp
        conf.h
        include/Auth.h
        include/DatabaseManager.h
        include/Handlers.h
        include/NetServer.h
        include/ThreadPool.h
        include/Utils.h
        src/Auth.cpp
        src/DatabaseManager.cpp
        src/Handlers.cpp
        src/NetServer.cpp
        src/Utils.cpp
)

# 链接SQLite3库
target_link_libraries(TinyStudentINFOWeb ${SQLite3_LIBRARIES})