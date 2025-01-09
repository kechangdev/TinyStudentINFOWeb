# TinyStudentINFOWeb

# Quick Start
```
docker build -t tinystudentinfoweb:latest .
docker run --rm -d -p 18899:10086 tinystudentinfoweb:latest
```
# 概述

`TinyStudentINFOWeb` 是一个使用 C++ 实现的基于面向对象编程的简单的学生信息展示站点项目，有如下特点：

- 页面实现（登录界面、注册界面、管理面板）
- 模块化项目结构，低耦合，易开发
- 线程池控制并发访问量
- 项目源码目录结构清晰
- 实现简单的鉴权，保障入门级安全性
- 通过设置 Cookie 实现简单的会话维护
- 采用轻量级数据库 SQLite
- 管理员面板，便于后期维护
- 容器化编排，便于运维管理
- 日志系统清晰，设置不同的日志等级，便于运维服务维护


**目录结构如下：**

```html
.
├── CMakeLists.txt
├── README.md
├── conf.h
├── db
│   └── autasker.db
├── include
│   ├── Auth.h
│   ├── DatabaseManager.h
│   ├── Handlers.h
│   ├── NetServer.h
│   ├── ThreadPool.h
│   └── Utils.h
├── main.cpp
├── src
│   ├── Auth.cpp
│   ├── DatabaseManager.cpp
│   ├── Handlers.cpp
│   ├── NetServer.cpp
│   └── Utils.cpp
└── www
    ├── 404.html
    ├── admin.html
    ├── index.html
    ├── login.html
    ├── logs.html
    ├── register.html
    └── student.html
```

- `db`: 数据库目录
- `www` : 静态页面目录
- `include` : 类头文件
- `src` : 类源代码
- `main.cpp` : 程序入口
- `conf.h` : 宏定义配置文件

---

# 主逻辑

## 后端

![image.png](TinyStudentINFOWeb%201734fa24e0328049a73acb39b9eefce1/image.png)

## 前端

---

# 核心模块

## Handlers 接口模块

**代码实现：**

- `src/Handlers.cpp`
- `include/Handlers.h`

该模块主要提供和实现 API 接口：

### **`/api/getstudentinfo`**

- **功能：**获取学生信息。根据用户类型（管理员或学生），返回相应的学生信息数据。

**请求：**

- **方法：** `GET`
- **请求头：** `Cookie`

**响应：**

- **响应格式:** `text/plain`
- **响应体**:
    - **第一行：**用户类型，`ADMIN` 或 `STUDENT`。
    - **后续行：**
        - **管理员用户**
            - 每行包含一个学生的信息，字段以逗号分隔。
            - 字段顺序：ID,姓名,算法成绩,数据结构成绩,操作系统成绩,数据库成绩,计算机网络成绩
        - **学生用户**
            - 每行包含一个课程及对应成绩，字段以逗号分隔。
            - 字段顺序：算法成绩,数据结构成绩,操作系统成绩,数据库成绩,计算机网络成绩

### **`/api/addstudentinfo`**

- **功能：**添加新的学生信息。

**请求：**

- **方法：** `POST`
- **请求头：** `Cookie`
- **请求格式：** `application/x-www-form-urlencoded`

**请求参数：**

- **`id`:** 学生ID（整数）
- **`name`:** 姓名（字符串）
- **`algorithms`:** 算法成绩（0-100）
- **`data_structures`:** 数据结构成绩（0-100）
- **`operating_systems`:** 操作系统成绩（0-100）
- **`databases`:** 数据库成绩（0-100）
- **`computer_networks`:** 计算机网络成绩（0-100）

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"学生信息添加成功。"`
    - **失败：** 返回错误信息描述，例如 `"添加失败：学生ID已存在。"`

### **`/api/modifystudentinfo`**

- **功能：**修改已有的学生信息。

**请求：**

- **方法：** `POST`
- **请求头：** `Cookie`
- **请求格式：** `application/x-www-form-urlencoded`

**请求参数：**

- **`id`:** 学生ID（整数）
- **`algorithms`:** 算法成绩（0-100）
- **`data_structures`:** 数据结构成绩（0-100）
- **`operating_systems`:** 操作系统成绩（0-100）
- **`databases`:** 数据库成绩（0-100）
- **`computer_networks`:** 计算机网络成绩（0-100）

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"学生信息修改成功。"`
    - **失败：** 返回错误信息描述，例如 `"修改失败：学生ID不存在。"`

### **`/api/islogin`**

- **功能：**检查用户是否已登录。

**请求：**

- **方法：** `GET`
- **请求头：** `Cookie`

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **已登录：** 返回状态码 `200 OK`，可选返回 `"用户已登录。"`
    - **未登录：** 返回状态码 `401 Unauthorized`，可选返回错误信息描述，例如 `"用户未登录。"`

### **`/api/login`**

- **功能：**用户登录。

**请求：**

- **方法：** `POST`
- **请求头：**
    - `Content-Type: application/x-www-form-urlencoded`
    - `Cookie`

**请求格式：** `application/x-www-form-urlencoded`

**请求参数：**

- **`username`:** 用户名（字符串，必填）
- **`password`:** 密码（字符串，必填）

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"登录成功。"` 并设置登录相关的Cookie。
    - **失败：** 返回错误信息描述，例如 `"登录失败：用户名或密码错误。"`

### **`/api/register`**

- **功能：**用户注册新账号。

**请求：**

- **方法：** `POST`
- **请求头：**
    - `Content-Type: application/x-www-form-urlencoded`

**请求格式：** `application/x-www-form-urlencoded`

**请求参数：**

- **`username`:** 用户名（字符串，必填）
- **`password`:** 密码（字符串，必填）
- **`invite_code`:** 确认密码（字符串，必填）

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"注册成功。"`
    - **失败：** 返回错误信息描述，例如 `"注册失败：用户名已存在。"`

### **`/api/logout`**

- **功能：**用户注销登录。

**请求：**

- **方法：** `POST`
- **请求头：** `Cookie`

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"注销成功。"`
    - **失败：** 返回错误信息描述，例如 `"注销失败：用户未登录。"`

### **`/api/getlogs`**

- **功能：**获取系统的日志信息。

**请求：**

- **方法：** `GET`
- **请求头：** `Cookie`

**响应：**

- **响应格式:** `text/plain`

- **响应体:**

    - **成功：** 返回纯文本格式的日志数据，每行一个日志，字段以逗号分隔，格式如下：**示例：**

      ```
      id,timestamp,log_level,user_id,message
      ```

    - **失败：** 返回错误信息描述，例如：

        - `"获取日志失败：权限不足。"`
        - `"获取日志失败：服务器内部错误。"`

### **`/api/getuserlist`**

- **功能：**获取系统中的用户列表。

**请求：**

- **方法：** `GET`
- **请求头：** `Cookie`

**响应：**

- **响应格式:** `text/plain`

- **响应体:**

    - **成功：** 返回纯文本格式的用户数据，每行一个用户，字段以逗号分隔，格式如下：**示例：**

      ```bash
      id,username
      ```

      ```yaml
      1001,john_doe
      1002,jane_smith
      ```

    - **失败：** 返回错误信息描述，例如：

        - `"获取用户列表失败：权限不足。"`
        - `"获取用户列表失败：服务器内部错误。"`

### **`/api/deluser`**

- **功能：**删除指定的用户。

**请求：**

- **方法：** `POST`
- **请求头：**
    - `Content-Type: text/plain`
    - `Cookie`

**请求格式：** `text/plain`

**请求参数：**

- **请求体：** 仅包含用户ID作为纯文本（字符串），例如 `"1001"`

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回字符串 `"success"`
    - **失败：** 返回错误信息描述，例如：
        - `"删除失败：用户ID不存在。"`
        - `"删除失败：权限不足。"`

### **`/api/isroot`**

- **功能：**检查当前用户是否具有root权限。

**请求：**

- **方法：** `GET`
- **请求头：**
    - `Cookie` （用于身份验证）

**响应：**

- **响应格式:** 无特定格式（依赖于HTTP状态码）
- **响应体:**
    - **成功（拥有root权限）：** 返回HTTP状态码 `200 OK`
    - **失败（权限不足）：** 返回HTTP状态码 `403 Forbidden`
    - **其他错误：** 返回相应的HTTP错误状态码，例如 `500 Internal Server Error`

### **`/api/username`**

- **功能：**获取当前登录用户的用户名。

**请求：**

- **方法：** `GET`
- **请求头：**
    - `Cookie` （用于身份验证）

**响应：**

- **响应格式:** `text/plain`
- **响应体:**
    - **成功：** 返回用户名字符串，例如 `"张三"`
    - **失败：** 返回错误信息描述，例如：
        - `"未登录"`（当用户未登录时）
        - `"网络响应不正常"`（当网络请求失败时）

## NetServer 网络模块

### 概述

网络模块 (`NetServer`) 是项目中的核心组件，负责处理网络通信、管理客户端连接、解析HTTP请求并生成相应的HTTP响应。该模块基于C++实现，利用POSIX套接字编程，支持多线程处理以提高并发性能。通过灵活的请求处理函数注册机制，`NetServer` 能够根据不同的请求路径执行相应的业务逻辑。

`NetServer` 类负责创建和管理服务器端套接字，监听指定端口的客户端连接，接收和解析HTTP请求，并通过注册的处理函数生成相应的HTTP响应。该类封装了底层的网络操作，提供简洁的接口以便在项目中集成和使用。

### 构造函数

```cpp
NetServer::NetServer(int port)
```

**参数：**

- `port`：服务器监听的端口号。

**功能：**

- 初始化服务器的端口号。
- 设置初始的套接字描述符为无效值 `1`。
- 设置服务器的运行状态为未运行 (`false`)。

### 析构函数

```cpp
NetServer::~NetServer()
```

**功能：**

- 确保在对象销毁时，如果服务器正在运行，则关闭服务器套接字，释放相关资源。

### `setHandler`

```cpp
void NetServer::setHandler(const std::string& path, RequestHandler handler)
```

**参数：**

- `path`：HTTP请求的路径，例如 `/api/data`。
- `handler`：处理该路径请求的回调函数，类型为 `RequestHandler`。

**功能：**

- 注册特定路径的请求处理函数，将路径与处理函数绑定，存储在 `handlers` 映射中。

### `run`

```cpp
void NetServer::run()
```

**功能：**

- 启动并运行服务器，阻塞调用线程。
- 创建服务器套接字，设置套接字选项（如 `SO_REUSEADDR`）。
- 绑定套接字到指定的IP地址和端口。
- 开始监听客户端连接，设定最大等待连接数。
- 进入主循环，持续接受并处理客户端连接，调用 `handleClient` 函数处理每个连接。
- 在服务器停止运行时，关闭服务器套接字。

### `handleClient`

```cpp
void NetServer::handleClient(int clientSocket, const struct sockaddr_in& clientAddress)
```

**参数：**

- `clientSocket`：客户端的套接字描述符。
- `clientAddress`：客户端的网络地址结构。

**功能：**

- 获取并记录客户端的IP地址。
- 从客户端套接字读取数据，解析HTTP请求，包括请求方法、路径、头部和请求体。
- 解析Cookie信息。
- 记录请求日志，包含IP地址、请求路径和Cookie。
- 根据请求路径查找对应的处理函数，生成HTTP响应。如果未找到处理函数，返回404 Not Found。
- 生成并发送HTTP响应字符串给客户端。
- 记录响应日志，包含IP地址、请求路径、响应状态码和Cookie。
- 关闭客户端套接字。

### 数据成员

- `int serverPort`：服务器监听的端口号。
- `int serverSocket`：服务器的套接字描述符。
- `bool isRunning`：服务器的运行状态标志。
- `std::unordered_map<std::string, RequestHandler> handlers`：存储请求路径与处理函数的映射关系。

### 使用示例

以下示例展示如何使用 `NetServer` 类创建一个简单的HTTP服务器，并注册处理函数：

```cpp
#include "NetServer.h"
#include <iostream>

// 示例处理函数
HttpResponse helloHandler(const HttpRequest& request) {
    HttpResponse response;
    response.status_code = 200;
    response.headers["Content-Type"] = "text/plain";
    response.body = "Hello, World!";
    return response;
}

int main() {
    int port = 8080;
    NetServer server(port);

    // 注册处理函数
    server.setHandler("/hello", helloHandler);

    std::cout << "Starting server on port " << port << "..." << std::endl;
    server.run();

    return 0;
}
```

## DatabaseManager 数据库管理模块

### 概述

**DatabaseManager** 模块负责管理项目中的数据库连接和操作。它采用单例模式确保全局只有一个数据库管理实例，并提供线程安全的接口用于初始化数据库、执行SQL语句以及获取数据库连接。该模块使用 SQLite 作为数据库引擎，并预设了多个表以支持用户管理、身份验证、日志记录以及学生成绩管理等功能。

### 表项设计

- **users 表**

  ```sql
  CREATE TABLE IF NOT EXISTS users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT UNIQUE NOT NULL,
      password_hash TEXT NOT NULL,
      permission_level INTEGER NOT NULL
  );
  ```

    - **字段说明**
        - `id`: 用户唯一标识，自增主键。
        - `username`: 用户名，唯一且不能为空。
        - `password_hash`: 密码哈希值，不能为空。
        - `permission_level`: 权限等级，不能为空。

- **auth 表**

  ```sql
  CREATE TABLE IF NOT EXISTS auth (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      cookie TEXT UNIQUE NOT NULL,
      expires_at INTEGER NOT NULL,
      FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE ON UPDATE CASCADE
  );
  ```

    - **字段说明**
        - `id`: 认证记录唯一标识，自增主键。
        - `user_id`: 关联的用户 ID，不能为空。
        - `cookie`: 认证 cookie，唯一且不能为空。
        - `expires_at`: 认证过期时间（时间戳），不能为空。

- **logs 表**

  ```sql
  CREATE TABLE IF NOT EXISTS logs (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER,
      log_level TEXT NOT NULL,
      message TEXT NOT NULL,
      timestamp INTEGER NOT NULL,
      FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE SET NULL ON UPDATE CASCADE
  );
  ```

    - **字段说明**
        - `id`: 日志记录唯一标识，自增主键。
        - `user_id`: 关联的用户 ID，可为空。
        - `log_level`: 日志级别，不能为空。
        - `message`: 日志信息，不能为空。
        - `timestamp`: 日志时间戳，不能为空。

- **student 表**

  ```sql
  CREATE TABLE IF NOT EXISTS student (
      id INTEGER PRIMARY KEY,
      algorithms_score REAL NOT NULL CHECK (algorithms_score BETWEEN 0 AND 100),
      data_structures_score REAL NOT NULL CHECK (data_structures_score BETWEEN 0 AND 100),
      operating_systems_score REAL NOT NULL CHECK (operating_systems_score BETWEEN 0 AND 100),
      databases_score REAL NOT NULL CHECK (databases_score BETWEEN 0 AND 100),
      computer_networks_score REAL NOT NULL CHECK (computer_networks_score BETWEEN 0 AND 100),
      FOREIGN KEY(id) REFERENCES users(id) ON DELETE CASCADE ON UPDATE CASCADE
  );
  ```

    - **字段说明**
        - `id`: 学生唯一标识，对应 `users` 表中的 `id`，作为主键。
        - 各科目分数 (`algorithms_score`, `data_structures_score`, `operating_systems_score`, `databases_score`, `computer_networks_score`): 分数为 0 至 100 之间的实数，不能为空。

### 成员变量

- `sqlite3* db`

  数据库连接指针，初始化为 `nullptr`。

- `std::mutex dbMutex`

  互斥锁，确保数据库操作的线程安全。

### 构造函数与析构函数

- **DatabaseManager()**

  构造函数，将 `db` 初始化为 `nullptr`。

- **~DatabaseManager()**

  析构函数，关闭数据库连接（如果已打开）。

### 成员函数

### `getInstance`

- **描述：**获取 `DatabaseManager` 的单例实例。

- **返回值：**返回 `DatabaseManager` 的引用。

- **示例**

  ```cpp
  DatabaseManager& dbManager = DatabaseManager::getInstance();
  ```

### `initialize`

- **描述**

  初始化数据库连接并设置数据库模式。此函数会尝试打开指定路径的数据库文件，如果文件不存在则创建新数据库。随后，它会执行预定义的 SQL 语句以创建所需的表。

- **参数**

    - `dbPath`: 数据库文件的路径。

- **返回值：**如果初始化成功，返回 `true`；否则返回 `false`。

- **线程安全：**通过 `std::lock_guard<std::mutex>` 确保线程安全。

- **示例**

  ```cpp
  bool success = dbManager.initialize("path/to/database.db");
  if (!success) {
      // 处理初始化失败
  }
  ```

### `getDB`

- **描述**

  获取当前的数据库连接指针。

- **返回值**

  返回指向 `sqlite3` 结构的指针。如果数据库未初始化，返回 `nullptr`。

- **示例**

  ```cpp
  sqlite3* db = dbManager.getDB();
  if (db) {
      // 执行数据库操作
  }
  ```

### 使用示例

```cpp
#include "../include/DatabaseManager.h"
#include <iostream>

int main() {
    // 获取单例实例
    DatabaseManager& dbManager = DatabaseManager::getInstance();

    // 初始化数据库
    if (!dbManager.initialize("data/my_database.db")) {
        std::cerr << "数据库初始化失败。" << std::endl;
        return -1;
    }

    // 获取数据库连接
    sqlite3* db = dbManager.getDB();
    if (db) {
        // 执行数据库操作
        const char* sql = "SELECT * FROM users;";
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, [](void*, int, char**, char**) -> int {
            // 回调函数处理查询结果
            return 0;
        }, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL执行错误: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    } else {
        std::cerr << "未能获取数据库连接。" << std::endl;
    }

    return 0;
}
```

### 依赖

- **SQLite3**

  本模块依赖 SQLite3 库，请确保在项目中正确链接 SQLite3。

- **C++11 或更高版本**

  使用了 `std::mutex` 和 `std::lock_guard`，需要支持 C++11 及以上标准。

## Auth 鉴权模块

- `src/Auth.cpp`
- `include/Auth.h`

### 概述

本鉴权模块（`Auth` 类）负责用户的登录、注册、会话验证、登出以及权限检查等功能。模块通过与 SQLite 数据库交互，管理用户信息和会话数据，确保用户身份的合法性和权限的正确性。

### `loginUser`

```cpp
static std::string loginUser(const std::string& username, const std::string& password);
```

**描述**
用户登录函数，通过用户名和密码验证用户身份，并生成会话 Cookie。

**参数**

- `username`：用户的用户名。
- `password`：用户的密码。

**返回值**

- 成功时返回生成的 Cookie 字符串。
- 失败时返回 `"null"`。

**流程**

1. 开启数据库事务。
2. 查询用户的 `id` 和 `password_hash`。
3. 验证密码是否匹配。
4. 检查用户是否已有存在的 Cookie，会删除旧的 Cookie。
5. 生成新的 Cookie，并插入 `auth` 表。
6. 提交事务。
7. 返回新的 Cookie。

**异常处理**

- 数据库操作失败时回滚事务，并返回 `"null"`。

### `registerUser`

```cpp
static std::string registerUser(const std::string& username, const std::string& password, const std::string& inviteCode);
```

**描述**
用户注册函数，通过用户名、密码和邀请码创建新用户，并自动登录。

**参数**

- `username`：新用户的用户名。
- `password`：新用户的密码。
- `inviteCode`：注册邀请码。

**返回值**

- 成功时返回生成的 Cookie 字符串。
- 失败时返回 `"null"`。

**流程**

1. 验证邀请码的有效性。
2. 插入新用户到 `users` 表，默认权限级别为 `1`。
3. 调用 `loginUser` 函数自动登录新用户。
4. 返回生成的 Cookie。

**注意**

- 当前实现中，密码以明文形式存储，建议改为哈希存储以增强安全性。

### `validateCookie`

```cpp
static bool validateCookie(const std::string& cookie);
```

**描述**
验证给定的 Cookie 是否有效，即检查 Cookie 是否存在且未过期。

**参数**

- `cookie`：要验证的 Cookie 字符串。

**返回值**

- 有效时返回 `true`。
- 无效时返回 `false`。

**流程**

1. 查询 `auth` 表中与 Cookie 对应的 `expires_at` 时间。
2. 比较当前时间与 `expires_at`，判断 Cookie 是否过期。
3. 返回验证结果。

### `logoutUser`

```cpp
static bool logoutUser(const std::string& cookie);
```

**描述**
用户登出函数，通过删除对应的 Cookie 记录来终止会话。

**参数**

- `cookie`：要注销的 Cookie 字符串。

**返回值**

- 成功时返回 `true`。
- 失败时返回 `false`。

**流程**

1. 删除 `auth` 表中对应的 Cookie 记录。
2. 返回操作结果。

### `isRoot`

```cpp
static bool isRoot(const std::string& cookie);
```

**描述**
检查给定 Cookie 所对应的用户是否为根用户（权限级别为 `0`）。

**参数**

- `cookie`：要检查的 Cookie 字符串。

**返回值**

- 是根用户时返回 `true`。
- 否则返回 `false`。

**流程**

1. 查询 `auth` 表，获取与 Cookie 对应的 `user_id`，并检查 Cookie 是否未过期。
2. 使用 `user_id` 查询 `users` 表，获取 `permission_level`。
3. 判断 `permission_level` 是否为 `0`，以确定是否为根用户。

### 使用示例

以下是如何使用 `Auth` 类中的各个函数的示例：

- 用户注册

```cpp
#include "Auth.h"

int main() {
    std::string username = "john_doe";
    std::string password = "securepassword123";
    std::string inviteCode = "INVITE2025";

    std::string cookie = Auth::registerUser(username, password, inviteCode);
    if (cookie != "null") {
        std::cout << "注册并登录成功，Cookie: " << cookie << std::endl;
    } else {
        std::cout << "注册失败。" << std::endl;
    }

    return 0;
}

```

- 用户登录

```cpp
#include "Auth.h"

int main() {
    std::string username = "john_doe";
    std::string password = "securepassword123";

    std::string cookie = Auth::loginUser(username, password);
    if (cookie != "null") {
        std::cout << "登录成功，Cookie: " << cookie << std::endl;
    } else {
        std::cout << "登录失败。" << std::endl;
    }

    return 0;
}
```

- 验证会话 Cookie

```cpp
#include "Auth.h"

int main() {
    std::string cookie = "user_session_cookie_here";

    if (Auth::validateCookie(cookie)) {
        std::cout << "Cookie 有效。" << std::endl;
    } else {
        std::cout << "Cookie 无效或已过期。" << std::endl;
    }

    return 0;
}
```

- 用户登出

```cpp
#include "Auth.h"

int main() {
    std::string cookie = "user_session_cookie_here";

    if (Auth::logoutUser(cookie)) {
        std::cout << "登出成功。" << std::endl;
    } else {
        std::cout << "登出失败。" << std::endl;
    }

    return 0;
}
```

- 检查是否为根用户

```cpp
#include "Auth.h"

int main() {
    std::string cookie = "user_session_cookie_here";

    if (Auth::isRoot(cookie)) {
        std::cout << "当前用户为根用户。" << std::endl;
    } else {
        std::cout << "当前用户非根用户。" << std::endl;
    }

    return 0;
}
```

## Utils 工具模块 & 日志模块

### 概述

`Utils` 模块提供了一组通用的实用函数，涵盖文件读取、时间获取、日志记录、Cookie 生成与解析、URL 解码、字符串修剪及表单数据解析等功能。该模块旨在简化常见的编程任务，提高开发效率。

### **`readHTML`**

```cpp
std::string Utils::readHTML(std::string filePath);
```

**描述**：读取指定路径的 HTML 文件内容并返回为字符串。

**参数**：

- `filePath`：要读取的 HTML 文件的路径。

**返回值**：文件内容的字符串。如果文件无法打开，则返回空字符串，并在标准错误输出中打印错误信息。

**示例**：

```cpp
std::string htmlContent = Utils::readHTML("path/to/file.html");
if (htmlContent.empty()) {
    // 处理错误
}
```

### `getCurrentTime`

```cpp
std::string Utils::getCurrentTime();
```

**描述**：获取当前系统时间，并格式化为 `YYYY-MM-DD:HH-MM-SS` 的字符串格式。

**参数**：无。

**返回值**：格式化后的时间字符串。

**示例**：

```cpp
std::string currentTime = Utils::getCurrentTime();
std::cout << "当前时间: " << currentTime << std::endl;
```

### `getTimeAndMessage`

```cpp
std::string Utils::getTimeAndMessage(int user_id, std::string level, std::string msg);
```

**描述**：生成带有时间戳和日志级别的消息，并将日志信息插入数据库中的 `logs` 表。

**参数**：

- `user_id`：用户的唯一标识符。
- `level`：日志级别（例如，INFO、WARN、ERROR）。
- `msg`：日志消息内容。

**返回值**：格式化后的日志字符串。如果在准备或执行数据库插入语句时发生错误，会在返回字符串中附加错误信息。

**示例**：

```cpp
std::string logEntry = Utils::getTimeAndMessage(123, "INFO", "用户登录成功");
std::cout << logEntry << std::endl;
```

### `generateCookie`

```cpp
std::string Utils::generateCookie();
```

**描述**：生成一个32位的随机Cookie字符串，由字母和数字组成。

**参数**：无。

**返回值**：生成的Cookie字符串。

**示例**：

```cpp
std::string cookie = Utils::generateCookie();
std::cout << "生成的Cookie: " << cookie << std::endl;
```

### `verifyInviteCode`

```cpp
bool Utils::verifyInviteCode(const std::string& inviteCode);
```

**描述**：验证邀请码是否正确。目前仅支持固定的 `INVITECODE`。

**参数**：

- `inviteCode`：要验证的邀请码字符串。

**返回值**：如果邀请码正确，返回 `true`；否则返回 `false`。

**示例**：

```cpp
bool isValid = Utils::verifyInviteCode("INVITECODE");
if (isValid) {
    // 验证通过
} else {
    // 验证失败
}
```

### `url_decode`

```cpp
std::string Utils::url_decode(const std::string& str);
```

**描述**：对URL编码的字符串进行解码，将 `%xx` 转换为对应的字符，`+` 转换为空格。

**参数**：

- `str`：要解码的URL编码字符串。

**返回值**：解码后的字符串。

**示例**：

```cpp
std::string decoded = Utils::url_decode("Hello%20World%21");
std::cout << decoded; // 输出: Hello World!
```

### `trim`

```cpp
std::string Utils::trim(const std::string& s);
```

**描述**：去除字符串开头和结尾的空白字符（包括空格、制表符、回车和换行符）。

**参数**：

- `s`：要修剪的字符串。

**返回值**：修剪后的字符串。如果字符串全为空白字符，则返回空字符串。

**示例**：

```cpp
std::string trimmed = Utils::trim("  Hello World!  ");
std::cout << "'" << trimmed << "'"; // 输出: 'Hello World!'
```

### `parseCookies`

```cpp
std::unordered_map<std::string, std::string> Utils::parseCookies(const std::string& cookieHeader);
```

**描述**：解析HTTP请求中的Cookie头，将其转换为键值对的无序映射。

**参数**：

- `cookieHeader`：包含Cookie的HTTP头字符串。

**返回值**：一个包含所有Cookie键值对的 `unordered_map`。

**示例**：

```cpp
std::string cookieHeader = "user=JohnDoe; session=abc123; theme=dark";
auto cookies = Utils::parseCookies(cookieHeader);
std::cout << "用户: " << cookies["user"] << std::endl;
```

### `urlDecode`

```cpp
std::string Utils::urlDecode(const std::string& str);
```

**描述**：与 `url_decode` 类似，对URL编码的字符串进行解码。此函数更为健壮，处理无效的百分号编码时保留原字符。

**参数**：

- `str`：要解码的URL编码字符串。

**返回值**：解码后的字符串。

**示例**：

```cpp
std::string decoded = Utils::urlDecode("Hello%20World%21%ZZ");
std::cout << decoded; // 输出: Hello World!%ZZ
```

### `parseForm`

```cpp
std::unordered_map<std::string, std::string> Utils::parseForm(const std::string& body);
```

**描述**：解析HTTP请求中的表单数据（`application/x-www-form-urlencoded`），将其转换为键值对的无序映射。

**参数**：

- `body`：包含表单数据的请求体字符串。

**返回值**：一个包含所有表单字段键值对的 `unordered_map`。

**示例**：

```cpp
std::string formData = "username=JohnDoe&password=123456&remember=true";
auto form = Utils::parseForm(formData);
std::cout << "用户名: " << form["username"] << std::endl;
```

## ThreadPool 线程池模块

- `ThreadPool.h`

### 概述

`ThreadPool` 是一个用于管理和执行多线程任务的 C++ 模块。它允许用户创建一个固定数量的工作线程，并将任务分派给这些线程以并行执行，从而提高程序的性能和响应能力。该模块采用现代 C++ 特性（如模板、`std::thread`、`std::mutex`、`std::condition_variable` 和 `std::future`）实现，确保高效、安全的多线程操作。

### 构造函数

```cpp
ThreadPool(size_t);
```

- **参数**:
    - `size_t threads`: 指定线程池中工作线程的数量。
- **描述**:
  初始化线程池，创建指定数量的工作线程。这些线程将在后台等待并执行分配给它们的任务。

### `enqueue`

```cpp
template<class F, class... Args>
auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type>;
```

- **模板参数**:
    - `F`: 可调用对象的类型（函数、lambda、绑定表达式等）。
    - `Args`: 可调用对象的参数类型。
- **参数**:
    - `F&& f`: 可调用对象。
    - `Args&&... args`: 可调用对象的参数。
- **返回值**:
    - `std::future<return_type>`: 一个 `std::future` 对象，用于获取任务的返回值。
- **描述**:
  向线程池提交一个任务。任务由可调用对象 `f` 及其参数 `args` 组成。该方法会将任务封装为一个 `std::packaged_task`，存入任务队列，并返回一个 `std::future` 对象，允许调用者在未来某个时间点获取任务的结果。
- **异常**:
    - 如果线程池已经停止（即析构函数已经开始执行），则抛出 `std::runtime_error` 异常。

### 析构函数

```cpp
~ThreadPool();
```

- **描述**:
  关闭线程池。具体步骤包括：
  1. 设置停止标志，防止新任务被添加。
  2. 通知所有工作线程，使其退出等待状态。
  3. 等待所有工作线程完成当前任务并终止。

### 私有成员

- `std::vector<std::thread> workers;`
    - 存储所有工作线程的容器。
- `std::queue<std::function<void()>> tasks;`
    - 任务队列，存储待执行的任务。
- `std::mutex queue_mutex;`
    - 保护任务队列的互斥锁，确保线程安全。
- `std::condition_variable condition;`
    - 条件变量，用于通知工作线程有新任务可执行或线程池正在停止。
- `bool stop;`
    - 标志位，指示线程池是否正在停止。

### 使用示例

以下是一个使用 `ThreadPool` 的简单示例，展示如何提交任务并获取结果。

```cpp
#include "ThreadPool.h"
#include <iostream>
#include <vector>
#include <chrono>

// 示例任务：计算两个整数的和
int add(int a, int b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟耗时操作
    return a + b;
}

int main() {
    // 创建一个包含 4 个工作线程的线程池
    ThreadPool pool(4);

    // 存储未来对象
    std::vector<std::future<int>> results;

    // 提交多个加法任务
    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.enqueue(add, i, i*2)
        );
    }

    // 获取并打印结果
    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;
}
```

**输出示例**:

```
0 3 6 9 12 15 18 21
```

**说明**:

1. 创建一个包含 4 个工作线程的线程池 `pool`。
2. 使用 `enqueue` 方法提交 8 个加法任务，每个任务计算 `i + (i * 2)`。
3. 将每个任务的 `std::future` 对象存储在 `results` 向量中。
4. 遍历 `results` 向量，调用 `get()` 方法获取每个任务的结果并打印。

# 前端设计

## Login 登录界面

## 概述

本页面为学生成绩管理系统的用户登录界面，旨在为用户提供一个简洁、安全的登录入口。用户可以通过输入用户名和密码进行登录，若尚未注册账户，则可通过页面提供的注册链接进行注册。

## 页面结构

页面采用标准的HTML5结构，主要分为以下几个部分：

- **DOCTYPE声明及HTML标签**：定义文档类型及语言为中文（简体）。
- **`<head>`部分**：包含页面的元数据、标题及内嵌的CSS样式。
- **`<body>`部分**：包含登录容器、登录表单、注册链接及错误信息显示区域。
- **内嵌的JavaScript**：处理页面加载时的登录状态检查及表单提交的异步请求。

## 主要HTML元素

```html
<div class="login-container">
    <h2>用户登录</h2>
    <form id="login-form">
        <label for="username">用户名：</label>
        <input type="text" id="username" name="username" required>

        <label for="password">密码：</label>
        <input type="password" id="password" name="password" required>

        <button type="submit">登录</button>
    </form>
    <div class="register-link">
        没有账号？<a href="/register">立即注册</a>
    </div>
    <div id="error-message" class="error-message" style="display: none;"></div>
</div>

```

## 样式说明（CSS）

页面使用内嵌的CSS样式，确保样式仅作用于本页面。主要样式包括：

- **整体布局**：
    - 背景色为浅灰色 (`#f2f2f2`)，使用`Arial, sans-serif`字体。
    - 登录容器设置固定宽度（350px），居中显示，带有圆角和阴影效果，提升视觉层次感。
- **登录表单**：
    - 标签 (`label`) 和输入框 (`input`) 的样式统一，确保表单元素整齐对齐。
    - 输入框具有适当的填充和边框，圆角设计提升美观度。
- **按钮**：
    - 登录按钮使用蓝色 (`#4285F4`) 背景，白色文字，悬停时颜色加深（`#357ae8`），增加交互反馈。
- **链接及错误信息**：
    - 注册链接与按钮颜色一致，悬停时增加下划线效果。
    - 错误信息使用红色字体，居中显示，初始隐藏，便于动态显示错误提示。

## 功能说明（JavaScript）

页面内嵌的JavaScript主要负责以下功能：

### 登录状态检查

```jsx
window.addEventListener('DOMContentLoaded', async () => {
    try {
        const response = await fetch('/api/islogin', {
            method: 'GET',
            credentials: 'include'
        });

        if (response.ok) {
            window.location.href = '/index';
        }
    } catch (error) {
        console.error('检查登录状态时出错:', error);
    }
});

```

- **功能**：页面加载完成后，自动向服务器发送`/api/islogin`的GET请求，检查用户是否已登录。
- **处理逻辑**：
    - 若响应状态为200（`response.ok`为`true`），则认为用户已登录，自动跳转到主页 (`/index`)。
    - 若未登录或请求失败，则保持在登录页面，允许用户进行登录操作。

### 表单提交处理

```jsx
document.getElementById('login-form').addEventListener('submit', async function(event) {
    event.preventDefault();

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    const errorMessageElement = document.getElementById('error-message');
    errorMessageElement.style.display = 'none';

    try {
        const params = new URLSearchParams();
        params.append('username', username);
        params.append('password', password);

        const response = await fetch('/api/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: params.toString(),
            credentials: 'include'
        });

        const resultText = await response.text();

        if (response.ok) {
            window.location.href = '/index';
        } else {
            errorMessageElement.textContent = resultText || '登录失败，请重试';
            errorMessageElement.style.display = 'block';
        }
    } catch (error) {
        errorMessageElement.textContent = '请求失败，请稍后重试';
        errorMessageElement.style.display = 'block';
    }
});

```

- **功能**：监听登录表单的提交事件，阻止默认提交行为，改为通过JavaScript进行异步请求处理。
- **处理逻辑**：
    - 获取用户输入的用户名和密码。
    - 使用`URLSearchParams`构建`application/x-www-form-urlencoded`格式的请求体。
    - 发送POST请求至`/api/login`，并携带必要的凭证（如Cookies）。
    - 根据服务器响应：
        - 若登录成功（响应状态为200），跳转到主页。
        - 若登录失败，显示服务器返回的错误信息或默认错误提示。
    - 若请求过程中发生网络错误或其他异常，显示通用的错误提示信息。

## Register 注册界面

### 概述

本页面为学生成绩管理系统的用户注册界面，旨在为新用户提供一个简洁、安全的注册入口。用户可以通过输入用户名、密码及邀请码完成注册。注册成功后，用户将被引导至登录页面或主页。

### 页面结构

页面采用标准的HTML5结构，主要分为以下几个部分：

- **DOCTYPE声明及HTML标签**：定义文档类型及语言为中文（简体）。
- **`<head>`部分**：包含页面的元数据、标题及内嵌的CSS样式。
- **`<body>`部分**：包含注册容器、注册表单及错误信息显示区域。
- **内嵌的JavaScript**：处理表单提交的异步请求。

### 主要HTML元素

```html
<div class="register-container">
  <h2>用户注册</h2>
  <form id="register-form">
    <label for="username">用户名：</label>
    <input type="text" id="username" name="username" required>

    <label for="password">密码：</label>
    <input type="password" id="password" name="password" required>

    <label for="invite_code">邀请码：</label>
    <input type="password" id="invite_code" name="invite_code" required>

    <button type="submit">注册</button>
  </form>
  <div id="error-message" class="error-message" style="display: none;"></div>
</div>

```

### 样式说明（CSS）

页面使用内嵌的CSS样式，确保样式仅作用于本页面。主要样式包括：

- **整体布局**：
    - 背景色为浅灰色 (`#f2f2f2`)，使用`Arial, sans-serif`字体。
    - 注册容器设置固定宽度（350px），居中显示，带有圆角和阴影效果，提升视觉层次感。
- **注册表单**：
    - 标签 (`label`) 和输入框 (`input`) 的样式统一，确保表单元素整齐对齐。
    - 输入框具有适当的填充和边框，圆角设计提升美观度。
- **按钮**：
    - 注册按钮使用蓝色 (`#4285F4`) 背景，白色文字，悬停时颜色加深（`#357ae8`），增加交互反馈。
- **错误信息**：
    - 错误信息使用红色字体，居中显示，初始隐藏，便于动态显示错误提示。

### 功能说明（JavaScript）

页面内嵌的JavaScript主要负责以下功能：

### 表单提交处理

```jsx
document.getElementById('register-form').addEventListener('submit', async function(event) {
  event.preventDefault();  // 阻止默认表单提交

  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;
  const invite_code = document.getElementById('invite_code').value;

  const errorMessageElement = document.getElementById('error-message');
  errorMessageElement.style.display = 'none';  // 清除上次的错误信息

  try {
    // 创建 URL 编码的表单数据
    const params = new URLSearchParams();
    params.append('username', username);
    params.append('password', password);
    params.append('invite_code', invite_code);

    // 发送异步 POST 请求
    const response = await fetch('/api/register', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
      },
      body: params.toString()
    });

    // 假设服务器返回纯文本响应
    const resultText = await response.text();

    // 检查注册是否成功
    if (response.ok) {
      // 注册成功的逻辑（比如跳转到主页）
      alert('注册成功!');
      window.location.href = '/';  // 注册成功后跳转
    } else {
      // 显示错误信息
      errorMessageElement.textContent = resultText || '注册失败，请重试';
      errorMessageElement.style.display = 'block';
    }
  } catch (error) {
    // 网络错误或其他异常
    errorMessageElement.textContent = '请求失败，请稍后重试';
    errorMessageElement.style.display = 'block';
  }
});

```

- **功能**：监听注册表单的提交事件，阻止默认提交行为，改为通过JavaScript进行异步请求处理。
- **处理逻辑**：
    - 获取用户输入的用户名、密码及邀请码。
    - 使用`URLSearchParams`构建`application/x-www-form-urlencoded`格式的请求体。
    - 发送POST请求至`/api/register`。
    - 根据服务器响应：
        - 若注册成功（响应状态为200），显示成功提示并跳转至主页或登录页面。
        - 若注册失败，显示服务器返回的错误信息或默认错误提示。
    - 若请求过程中发生网络错误或其他异常，显示通用的错误提示信息。

## Logs 日志查看界面

### 概述

本页面为学生成绩管理系统的管理员面板中的日志查看界面，旨在为系统管理员提供一个高效、直观的工具，以便查看、监控和管理系统日志。通过此界面，管理员可以浏览系统运行中的各种日志信息，帮助识别和排查潜在问题，确保系统的稳定性和安全性。

### 页面结构

页面采用标准的HTML5结构，主要分为以下几个部分：

- **DOCTYPE声明及HTML标签**：定义文档类型及语言为中文（简体）。
- **`<head>`部分**：包含页面的元数据、标题及内嵌的CSS样式。
- **`<body>`部分**：包含日志查看的主容器、控制面板（筛选和分页）、日志表格、消息显示区域及加载指示器。
- **内嵌的JavaScript**：负责获取日志数据、解析数据、填充表格、处理分页及显示消息。

### 主要HTML元素

```html
<div class="container">
    <h1>日志查看</h1>

    <div class="controls">
        <div class="filter">
            <!-- 日志级别筛选已取消 -->
            <label for="logLevel">日志级别:</label>
            <select id="logLevel">
                <option value="">全部</option>
                <option value="DEBUG">DEBUG</option>
                <option value="INFO">INFO</option>
                <option value="WARNING">WARNING</option>
                <option value="ERROR">ERROR</option>
                <option value="CRITICAL">CRITICAL</option>
            </select>
        </div>
        <div class="pagination-info">
            <!-- 可添加额外控制元素 -->
        </div>
    </div>

    <table>
        <thead>
        <tr>
            <th>日志ID</th>
            <th>时间</th>
            <th>日志级别</th>
            <th>用户ID</th>
            <th>日志</th>
        </tr>
        </thead>
        <tbody id="logs-table-body">
        <!-- 日志列表将通过JavaScript动态填充 -->
        </tbody>
    </table>

    <div class="pagination">
        <button id="prevPage">上一页</button>
        <span id="pageInfo">第 1 页</span>
        <button id="nextPage">下一页</button>
    </div>

    <div id="message" class="message"></div>
    <div id="error" class="error"></div>
    <div class="loader" id="loader"></div>
</div>

```

### 样式说明（CSS）

页面使用内嵌的CSS样式，确保样式仅作用于本页面。主要样式包括：

- **整体布局**：
    - 背景色为浅灰色 (`#f4f6f8`)，使用`Arial, sans-serif`字体。
    - 主容器设置宽度为90%，最大宽度1200px，居中显示，带有圆角和阴影效果，提升视觉层次感。
- **标题**：
    - 页面标题居中显示，颜色为深灰色 (`#333333`)，增强可读性。
- **控制面板**：
    - 包含筛选和分页信息区域，使用Flexbox布局实现响应式排列。
    - **日志级别筛选**：
        - 虽然筛选功能已被隐藏，但相关样式保留，方便未来功能扩展。
- **表格**：
    - 宽度100%，边框折叠，确保表格内容整齐对齐。
    - 表头背景色为蓝色 (`#007BFF`)，文字颜色为白色，突出显示。
    - 表格行在鼠标悬停时背景色变为浅灰色 (`#f1f1f1`)，增加交互反馈。
- **分页控制**：
    - 分页按钮使用蓝色背景，白色文字，悬停时颜色加深。
    - 禁用状态下按钮颜色变为浅灰色，且鼠标指针变为不可用状态。
- **消息显示**：
    - 成功消息使用绿色字体 (`#28a745`)，错误消息使用红色字体 (`#dc3545`)。
    - 消息居中显示，增强可见性。
- **加载指示器**：
    - 使用CSS动画创建旋转的加载动画，位于页面中央，初始隐藏。
- **响应式设计**：
    - 针对屏幕宽度小于768px的设备，调整表格单元格的填充和字体大小，控制面板采用垂直排列，加载指示器缩小尺寸，确保移动设备上的良好显示效果。

### 功能说明（JavaScript）

页面内嵌的JavaScript主要负责以下功能：

### 日志数据获取与显示

```jsx
document.addEventListener('DOMContentLoaded', function() {
    const logsTableBody = document.getElementById('logs-table-body');
    const prevPageBtn = document.getElementById('prevPage');
    const nextPageBtn = document.getElementById('nextPage');
    const pageInfo = document.getElementById('pageInfo');
    const messageDiv = document.getElementById('message');
    const errorDiv = document.getElementById('error');
    const loader = document.getElementById('loader');

    let currentPage = 1;
    const pageSize = 50;
    let totalLogs = 0;
    let allLogs = []; // 存储所有日志数据

    // 显示加载指示器
    function showLoader() {
        loader.style.display = 'block';
    }

    // 隐藏加载指示器
    function hideLoader() {
        loader.style.display = 'none';
    }

    // 显示错误消息
    function showError(message) {
        errorDiv.textContent = message;
        setTimeout(() => {
            errorDiv.textContent = '';
        }, 5000);
    }

    // 显示成功消息
    function showMessage(message) {
        messageDiv.textContent = message;
        setTimeout(() => {
            messageDiv.textContent = '';
        }, 3000);
    }

    // 获取并显示日志
    async function fetchLogs() {
        showLoader();
        try {
            const response = await fetch('/api/getlogs');
            if (!response.ok) {
                throw new Error('网络响应不正常');
            }

            const data = await response.text();
            allLogs = parsePlainData(data);
            totalLogs = allLogs.length;
            currentPage = 1; // 重置到第一页
            populateLogsTable();
            updatePagination();
            hideLoader();
        } catch (error) {
            console.error('获取日志时出错:', error);
            showError('无法加载日志数据。请稍后再试。');
            hideLoader();
        }
    }

    // 解析纯文本数据（每行一个日志，字段以逗号分隔：id,timestamp,log_level,user_id,message）
    function parsePlainData(data) {
        const logs = [];
        const lines = data.trim().split('\n');
        lines.forEach(line => {
            // 使用正则匹配，考虑到日志消息中可能包含逗号
            const regex = /^(\d+),(\d+),([A-Z]+),(\d+),(.*)$/;
            const match = line.match(regex);
            if (match) {
                const [, id, timestamp, log_level, user_id, message] = match;
                logs.push({
                    id: id.trim(),
                    timestamp: parseInt(timestamp.trim(), 10),
                    log_level: log_level.trim(),
                    user_id: user_id.trim(),
                    message: message.trim()
                });
            }
        });
        return logs;
    }

    // 填充日志表格
    function populateLogsTable() {
        logsTableBody.innerHTML = ''; // 清空现有内容
        if (allLogs.length === 0) {
            const tr = document.createElement('tr');
            const td = document.createElement('td');
            td.colSpan = 5; // 现在有5列：日志ID, 时间, 日志级别, 用户ID, 日志
            td.textContent = '暂无日志数据。';
            td.style.textAlign = 'center';
            tr.appendChild(td);
            logsTableBody.appendChild(tr);
            return;
        }

        const startIndex = (currentPage - 1) * pageSize;
        const endIndex = Math.min(startIndex + pageSize, totalLogs);
        const logsToDisplay = allLogs.slice(startIndex, endIndex);

        logsToDisplay.forEach(log => {
            const tr = document.createElement('tr');

            const tdId = document.createElement('td');
            tdId.textContent = log.id;
            tr.appendChild(tdId);

            const tdTime = document.createElement('td');
            const date = new Date(log.timestamp * 1000); // 假设timestamp是Unix时间戳
            tdTime.textContent = date.toLocaleString();
            tr.appendChild(tdTime);

            const tdLevel = document.createElement('td');
            tdLevel.textContent = log.log_level;
            tr.appendChild(tdLevel);

            const tdUserId = document.createElement('td');
            tdUserId.textContent = log.user_id;
            tr.appendChild(tdUserId);

            const tdMessage = document.createElement('td');
            tdMessage.textContent = log.message;
            tr.appendChild(tdMessage);

            logsTableBody.appendChild(tr);
        });
    }

    // 更新分页信息和按钮状态
    function updatePagination() {
        const totalPages = Math.ceil(totalLogs / pageSize);
        pageInfo.textContent = `第 ${currentPage} 页，共 ${totalPages} 页`;

        prevPageBtn.disabled = currentPage === 1;
        nextPageBtn.disabled = currentPage >= totalPages;
    }

    // 事件监听器
    prevPageBtn.addEventListener('click', () => {
        if (currentPage > 1) {
            currentPage--;
            populateLogsTable();
            updatePagination();
        }
    });

    nextPageBtn.addEventListener('click', () => {
        const totalPages = Math.ceil(totalLogs / pageSize);
        if (currentPage < totalPages) {
            currentPage++;
            populateLogsTable();
            updatePagination();
        }
    });

    // 初始加载
    fetchLogs();
});

```

### 主要功能解释

1. **页面加载时获取日志数据**：
    - 使用`DOMContentLoaded`事件监听器，确保DOM元素加载完成后再执行脚本。
    - 调用`fetchLogs`函数从服务器获取日志数据。
2. **日志数据获取 (`fetchLogs`)**：
    - 显示加载指示器以提示用户数据正在加载。
    - 通过`fetch` API发送GET请求至`/api/getlogs`接口获取日志数据。
    - 若响应成功，调用`parsePlainData`解析数据，并将其存储在`allLogs`数组中。
    - 更新总日志数`totalLogs`，重置当前页为第一页，填充日志表格并更新分页信息。
    - 隐藏加载指示器。
    - 若请求失败，显示错误消息并隐藏加载指示器。
3. **日志数据解析 (`parsePlainData`)**：
    - 假设服务器返回纯文本格式，每行一个日志，字段以逗号分隔：`id,timestamp,log_level,user_id,message`。
    - 使用正则表达式解析每行日志，考虑到日志消息中可能包含逗号。
    - 将解析后的日志对象存储在`logs`数组中并返回。
4. **填充日志表格 (`populateLogsTable`)**：
    - 清空现有表格内容。
    - 若无日志数据，显示“暂无日志数据。”的提示。
    - 根据当前页码和每页显示数量（`pageSize`），计算要显示的日志范围。
    - 遍历要显示的日志数据，动态创建表格行和单元格，填充日志信息。
5. **分页更新 (`updatePagination`)**：
    - 计算总页数，并更新分页信息显示。
    - 根据当前页码，启用或禁用“上一页”和“下一页”按钮。
6. **分页按钮事件监听**：
    - “上一页”按钮点击时，若当前页大于1，减少页码并更新表格和分页信息。
    - “下一页”按钮点击时，若当前页小于总页数，增加页码并更新表格和分页信息。
7. **消息显示**：
    - 成功消息在操作成功后显示，5秒后自动隐藏。
    - 错误消息在操作失败后显示，5秒后自动隐藏。
8. **加载指示器控制**：
    - 在数据请求期间显示加载指示器，数据加载完成或失败后隐藏。

## Admin 用户管理界面

### 概述

本页面为学生成绩管理系统的管理员面板中的用户管理界面，旨在为系统管理员提供一个高效、直观的工具，以便管理系统中的用户。通过此界面，管理员可以查看所有注册用户的详细信息，并执行用户删除操作，以维护系统的安全性和秩序。

### 页面结构

页面采用标准的HTML5结构，主要分为以下几个部分：

- **DOCTYPE声明及HTML标签**：定义文档类型及语言为中文（简体）。
- **`<head>`部分**：包含页面的元数据、标题及内嵌的CSS样式。
- **`<body>`部分**：包含用户管理的主容器、用户表格、消息显示区域及加载指示器。
- **内嵌的JavaScript**：负责获取用户数据、填充表格、处理用户删除操作及自动刷新用户列表。

### 主要HTML元素

```html
<div class="container">
  <h1>用户管理</h1>
  <table>
    <thead>
      <tr>
        <th>用户ID</th>
        <th>用户名</th>
        <th>操作</th>
      </tr>
    </thead>
    <tbody id="user-table-body">
      <!-- 用户列表将通过JavaScript动态填充 -->
    </tbody>
  </table>
  <div id="message" class="message"></div>
  <div id="error" class="error"></div>
  <div class="loader" id="loader"></div>
</div>

```

### 样式说明（CSS）

页面使用内嵌的CSS样式，确保样式仅作用于本页面。主要样式包括：

- **整体布局**：
    - 背景色为浅灰色 (`#f4f6f8`)，使用`Arial, sans-serif`字体。
    - 主容器设置宽度为90%，最大宽度1200px，居中显示，带有圆角和阴影效果，提升视觉层次感。
- **标题**：
    - 页面标题居中显示，颜色为深灰色 (`#333333`)，增强可读性。
- **表格**：
    - 宽度100%，边框折叠，确保表格内容整齐对齐。
    - 表头背景色为蓝色 (`#007BFF`)，文字颜色为白色，突出显示。
    - 表格行在鼠标悬停时背景色变为浅灰色 (`#f1f1f1`)，增加交互反馈。
- **操作按钮**：
    - 删除按钮使用红色 (`#dc3545`) 背景，白色文字，悬停时颜色加深 (`#c82333`)，增加交互反馈。
    - 按钮具有圆角设计，提升美观度。
- **消息显示**：
    - 成功消息使用绿色字体 (`#28a745`)，错误消息使用红色字体 (`#dc3545`)。
    - 消息居中显示，增强可见性。
- **加载指示器**：
    - 使用CSS动画创建旋转的加载动画，位于页面中央，初始隐藏。
- **响应式设计**：
    - 针对屏幕宽度小于768px的设备，调整表格单元格的填充和按钮尺寸，确保移动设备上的良好显示效果。

### 功能说明（JavaScript）

页面内嵌的JavaScript主要负责以下功能：

### 用户数据获取与显示

```jsx
document.addEventListener('DOMContentLoaded', function() {
  const userTableBody = document.getElementById('user-table-body');
  const messageDiv = document.getElementById('message');
  const errorDiv = document.getElementById('error');
  const loader = document.getElementById('loader');
  const REFRESH_INTERVAL = 30000; // 30秒

  let refreshTimer;

  // 显示加载指示器
  function showLoader() {
    loader.style.display = 'block';
  }

  // 隐藏加载指示器
  function hideLoader() {
    loader.style.display = 'none';
  }

  // 获取用户列表
  function fetchUserList() {
    showLoader();
    fetch('/api/getuserlist')
      .then(response => {
        if (!response.ok) {
          throw new Error('网络响应不正常');
        }
        return response.text(); // 获取纯文本数据
      })
      .then(data => {
        const users = parsePlainData(data);
        populateUserTable(users);
        hideLoader();
      })
      .catch(error => {
        console.error('获取用户列表时出错:', error);
        errorDiv.textContent = '无法加载用户列表。请稍后再试。';
        hideLoader();
        setTimeout(() => {
          errorDiv.textContent = '';
        }, 5000);
      });
  }

  // 解析纯文本数据（每行一个用户，字段以逗号分隔：id,username）
  function parsePlainData(data) {
    const users = [];
    const lines = data.trim().split('\n');
    lines.forEach(line => {
      const [id, username] = line.split(',');
      if (id && username) {
        users.push({ id: id.trim(), username: username.trim() });
      }
    });
    return users;
  }

  // 填充用户表格
  function populateUserTable(users) {
    userTableBody.innerHTML = ''; // 清空现有内容
    if (users.length === 0) {
      const tr = document.createElement('tr');
      const td = document.createElement('td');
      td.colSpan = 3; // 现在有3列：用户ID, 用户名, 操作
      td.textContent = '暂无用户数据。';
      td.style.textAlign = 'center';
      tr.appendChild(td);
      userTableBody.appendChild(tr);
      return;
    }

    users.forEach(user => {
      const tr = document.createElement('tr');

      const tdId = document.createElement('td');
      tdId.textContent = user.id;
      tr.appendChild(tdId);

      const tdUsername = document.createElement('td');
      tdUsername.textContent = user.username;
      tr.appendChild(tdUsername);

      const tdAction = document.createElement('td');
      const deleteButton = document.createElement('button');
      deleteButton.textContent = '删除';
      deleteButton.classList.add('delete-btn');
      deleteButton.addEventListener('click', () => deleteUser(user.id));
      tdAction.appendChild(deleteButton);
      tr.appendChild(tdAction);

      userTableBody.appendChild(tr);
    });
  }

  // 删除用户
  function deleteUser(userId) {
    if (!confirm('确定要删除此用户吗？')) {
      return;
    }

    showLoader();
    fetch('/api/deluser', {
      method: 'POST',
      headers: {
        'Content-Type': 'text/plain' // 保持为纯文本格式
      },
      body: String(userId) // 仅包含用户ID作为纯文本
    })
      .then(response => {
        if (!response.ok) {
          throw new Error('删除请求失败');
        }
        return response.text(); // 处理为纯文本
      })
      .then(data => {
        // 假设服务器返回 "success" 表示删除成功
        if (data.trim().toLowerCase() === 'success') {
          messageDiv.textContent = '用户已成功删除。';
          fetchUserList(); // 重新加载用户列表
          setTimeout(() => {
            messageDiv.textContent = '';
          }, 3000);
        } else {
          // 如果不是 "success"，则认为是错误消息
          throw new Error(data);
        }
      })
      .catch(error => {
        console.error('删除用户时出错:', error);
        errorDiv.textContent = '无法删除用户。' + (error.message ? `原因：${error.message}` : '请稍后再试。');
        hideLoader();
        setTimeout(() => {
          errorDiv.textContent = '';
        }, 5000);
      });
  }

  // 自动定时刷新用户列表
  function startAutoRefresh() {
    refreshTimer = setInterval(fetchUserList, REFRESH_INTERVAL);
  }

  // 停止自动刷新（如果需要）
  function stopAutoRefresh() {
    if (refreshTimer) {
      clearInterval(refreshTimer);
    }
  }

  // 初始加载用户列表
  fetchUserList();
  startAutoRefresh();

  // 可选：在页面卸载时停止自动刷新
  window.addEventListener('beforeunload', stopAutoRefresh);
});

```

### 主要功能解释

1. **页面加载时获取用户数据**：
    - 使用`DOMContentLoaded`事件监听器，确保DOM元素加载完成后再执行脚本。
    - 调用`fetchUserList`函数从服务器获取用户数据。
2. **用户数据获取 (`fetchUserList`)**：
    - 显示加载指示器以提示用户数据正在加载。
    - 通过`fetch` API发送GET请求至`/api/getuserlist`接口获取用户数据。
    - 若响应成功，调用`parsePlainData`解析数据，并将其存储在`users`数组中。
    - 填充用户表格并隐藏加载指示器。
    - 若请求失败，显示错误消息并隐藏加载指示器。
3. **用户数据解析 (`parsePlainData`)**：
    - 假设服务器返回纯文本格式，每行一个用户，字段以逗号分隔：`id,username`。
    - 将解析后的用户对象存储在`users`数组中并返回。
4. **填充用户表格 (`populateUserTable`)**：
    - 清空现有表格内容。
    - 若无用户数据，显示“暂无用户数据。”的提示。
    - 遍历用户数据，动态创建表格行和单元格，填充用户信息及操作按钮。
5. **用户删除操作 (`deleteUser`)**：
    - 在点击“删除”按钮时，弹出确认对话框确认操作。
    - 若确认，显示加载指示器并发送POST请求至`/api/deluser`接口，携带用户ID作为纯文本。
    - 根据服务器响应：
        - 若返回`"success"`，显示成功消息并重新加载用户列表。
        - 若返回其他内容，认为是错误消息并显示相应提示。
    - 若请求过程中发生网络错误或其他异常，显示通用的错误提示信息。
6. **自动刷新用户列表 (`startAutoRefresh`)**：
    - 设置定时器，每30秒自动调用`fetchUserList`函数刷新用户列表，确保数据的实时性。
7. **消息与错误提示显示**：
    - 成功消息在操作成功后显示，3秒后自动隐藏。
    - 错误消息在操作失败后显示，5秒后自动隐藏。
8. **加载指示器控制**：
    - 在数据请求期间显示加载指示器，数据加载完成或失败后隐藏。

## Student 学生成绩界面

### 概述

本页面支持以下主要功能：

- **管理员**：查看所有学生的信息，添加新学生，修改现有学生的成绩。
- **学生**：查看自身的成绩信息。

系统采用HTML、CSS和JavaScript开发，前端通过AJAX与后端API进行数据交互。

### HTML 结构

页面采用标准的HTML5结构，包含以下主要部分：

- **头部（`<head>`）**：
    - 设置字符编码为UTF-8。
    - 页面标题为“学生信息管理系统”。
    - 内嵌CSS样式用于页面美化和响应式设计。
    - 内嵌JavaScript用于实现页面的动态功能。
- **主体（`<body>`）**：
    - **容器（`.container`）**：用于包裹主要内容，设置了宽度、背景色、阴影等样式。
    - **标题（`<h1>`）**：显示“学生信息管理系统”。
    - **学生信息展示区域（`<div id="studentInfo">`）**：用于动态显示学生信息。
    - **添加学生信息表单（`<div id="addStudentForm">`）**：管理员用于添加新学生的表单，默认隐藏。
    - **修改学生信息表单（`<div id="modifyStudentForm">`）**：管理员用于修改学生成绩的表单，默认隐藏。
    - **加载指示器（`.loader`）**：用于在数据加载时显示的动画，默认隐藏。

### CSS 样式

CSS主要负责页面的布局、颜色、字体、响应式设计等：

- **基础样式**：
    - 统一字体为Arial或无衬线字体。
    - 设置背景色为浅灰色，容器背景为白色。
    - 添加盒子阴影和圆角以提升视觉效果。
- **表格样式**：
    - 设置表格宽度为100%，去除边框间隙。
    - 表头背景色为蓝色，字体为白色。
    - 鼠标悬停行时改变背景色以增强交互体验。
- **按钮样式**：
    - “修改”按钮为绿色，带有圆角和悬停效果。
    - 提交按钮为蓝色，带有悬停变色效果。
- **表单样式**：
    - 表单容器带有边框、圆角和背景色。
    - 表单元素间距合理，输入框和标签对齐。
- **加载指示器**：
    - 圆形旋转动画，用于提示用户正在加载数据。
    - 通过绝对定位居中显示。
- **响应式设计**：
    - 针对屏幕宽度小于768px的设备，调整表格和按钮的内边距，加载指示器大小缩小。

### JavaScript 功能

JavaScript主要实现以下功能：

1. **获取并显示学生信息**：
    - 在页面加载时，通过AJAX向`/api/getstudentinfo`发送GET请求，获取学生信息。
    - 根据用户类型（ADMIN或STUDENT）动态生成表格显示数据。
    - 管理员视图显示所有学生的信息，并提供“修改”按钮。
    - 学生视图仅显示个人的课程成绩。
2. **添加学生信息**：
    - 管理员可以通过“添加学生信息”表单输入新学生的详细信息。
    - 表单提交时，通过AJAX向`/api/addstudentinfo`发送POST请求，添加新学生。
3. **修改学生信息**：
    - 管理员点击“修改”按钮后，显示修改表单，预填学生ID。
    - 修改表单提交时，通过AJAX向`/api/modifystudentinfo`发送POST请求，更新学生成绩。
4. **加载指示器**：
    - 在发送AJAX请求前显示加载指示器，数据加载完成后隐藏。
5. **表单显示与隐藏**：
    - 通过切换CSS类`hidden`来控制添加和修改表单的显示与隐藏。

### 主要功能

- 查看学生信息
    - **管理员视图**：
        - 显示所有学生的详细信息，包括ID、姓名及各科成绩。
        - 提供“修改”按钮，允许管理员更新学生成绩。
        - 显示添加学生信息的表单。
    - **学生视图**：
        - 仅显示当前学生的各科成绩。
        - 隐藏添加和修改表单，确保学生只能查看自己的信息。
- 添加学生信息
    - 管理员通过“添加学生信息”表单输入新学生的ID、姓名及各科成绩。
    - 表单验证确保所有字段均已填写且成绩在0-100之间。
    - 提交表单后，系统发送数据至后端API，成功后刷新学生信息展示区域并隐藏表单。
- 修改学生信息
    - 管理员点击某一学生的“修改”按钮，弹出修改表单。
    - 修改表单仅允许更新各科成绩，学生ID为只读（隐藏域）。
    - 提交修改后，系统发送更新数据至后端API，成功后刷新学生信息展示区域并隐藏表单。
- 加载指示器
    - 在发送AJAX请求期间，显示加载指示器提示用户等待。
    - 请求完成后隐藏加载指示器，无论成功与否。
