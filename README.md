# SmartMonitor - 工业设备监控系统

基于 Qt/C++ 开发的工业设备实时监控系统，支持设备数据采集、实时显示、报警管理和数据导出。

## 功能特性

- **实时监控**：通过 TCP 连接采集设备数据，实时显示温度、压力、转速、电压等参数
- **设备管理**：设备列表展示、设备详情查看、报警记录查询
- **报警管理**：参数超限自动报警（温度>80℃、压力>1.5MPa），报警记录持久化存储
- **数据导出**：支持将历史数据导出为 CSV 文件
- **用户登录**：基于数据库的用户认证，密码 SHA256 哈希存储
- **自动重连**：TCP 连接断开后自动重连，适应工业场景网络不稳定环境
- **异步通信**：TCP 连接采用异步模式，不阻塞 UI 线程
- **多线程架构**：数据接收和解析在独立线程执行，保证界面流畅
- **设备模拟器**：内置设备模拟工具，方便开发和测试
- **单元测试**：包含 DataParser 和 DbManager 的单元测试

## 项目结构

```
SmartMonitor/
├── SmartMonitor.pro              # 顶层项目文件
├── SmartMonitorApp/              # 主应用程序
│   ├── main.cpp                  # 程序入口
│   ├── ui/                       # 界面模块
│   │   ├── MainWindow.cpp/h      # 主窗口（实时监控）
│   │   ├── LoginWindow.cpp/h     # 登录窗口
│   │   ├── DeviceDetail.cpp/h    # 设备详情（报警查询）
│   │   └── HistoryQuery.cpp/h    # 历史数据查询
│   ├── network/                  # 网络通信模块
│   │   ├── TcpClient.cpp/h       # TCP客户端（异步连接+自动重连）
│   │   └── DataParser.cpp/h      # 二进制协议解析器
│   ├── database/                 # 数据库模块
│   │   ├── DbManager.cpp/h       # SQLite数据库管理（单例模式）
│   │   └── DataExporter.cpp/h    # CSV数据导出
│   ├── model/                    # 数据模型
│   │   ├── DeviceInfo.h          # 设备信息结构体
│   │   └── AlarmRecord.h         # 报警记录结构体
│   ├── thread/                   # 线程模块
│   │   └── RecvThread.cpp/h      # 数据接收Worker（moveToThread模式）
│   └── resource/                 # 资源文件
│       ├── resource.qrc
│       └── qss/style.qss
├── DeviceSimulator/              # 设备模拟器
│   ├── main.cpp
│   ├── DeviceServer.cpp/h        # 模拟TCP服务器
│   └── SimMainWindow.cpp/h       # 模拟器界面
└── Tests/                        # 单元测试
    ├── main.cpp                  # 测试入口
    ├── tst_dataparser.cpp/h      # DataParser测试
    └── tst_dbmanager.cpp/h       # DbManager测试
```

## 技术栈

- **开发语言**：C++17
- **UI 框架**：Qt 5/6
- **网络通信**：QTcpSocket / QTcpServer（异步非阻塞）
- **数据库**：SQLite（QSqlDatabase + 参数化查询防SQL注入）
- **多线程**：QThread + moveToThread 模式
- **安全**：密码 SHA256 哈希存储
- **测试**：QTest 单元测试框架
- **构建工具**：qmake

## 编译运行

### 环境要求

- Qt 5.12 或更高版本
- Qt Creator（推荐）
- MinGW 或 MSVC 编译器

### 编译步骤

1. 使用 Qt Creator 打开 `SmartMonitor.pro`
2. 选择编译套件（Kit）
3. 点击构建（Build）按钮

### 运行

编译完成后会生成三个可执行文件：

1. **SmartMonitorApp** - 主监控程序
2. **DeviceSimulator** - 设备模拟器
3. **SmartMonitorTests** - 单元测试

建议先启动 DeviceSimulator，再启动 SmartMonitorApp 进行连接测试。

### 运行测试

```bash
cd Tests
qmake
make
./SmartMonitorTests
```

## 数据协议

系统使用自定义的二进制数据协议，帧长度 23 字节：

| 字段 | 长度 | 说明 |
|------|------|------|
| 帧头 | 2字节 | 固定 0xAA 0x55 |
| 设备ID | 2字节 | 设备编号（大端序） |
| 温度 | 4字节 | float 类型（大端序） |
| 压力 | 4字节 | float 类型（大端序） |
| 转速 | 4字节 | float 类型（大端序） |
| 电压 | 4字节 | float 类型（大端序） |
| 状态 | 1字节 | 设备状态（0=正常, 1=报警） |
| 帧尾 | 2字节 | 固定 0x55 0xAA |

## 设计亮点（面试加分项）

| 亮点 | 说明 |
|------|------|
| 异步TCP连接 | 不阻塞UI线程，5秒超时自动断开 |
| 自动重连 | 工业场景网络不稳定，断开后自动重连 |
| moveToThread | Qt推荐的多线程模式，数据解析在子线程执行 |
| 单例模式 | DbManager全局唯一实例，删除拷贝构造防复制 |
| 参数化SQL | prepare+addBindValue防SQL注入 |
| 哈希密码 | SHA256哈希存储，不保存明文密码 |
| 二进制协议 | 自定义23字节帧协议，大端序处理 |
| 信号槽解耦 | 网络事件通过信号暴露，调用方不直接操作Socket |
| 单元测试 | DataParser和DbManager的完整测试用例 |

## 默认账户

| 用户名 | 密码 |
|--------|------|
| admin  | 123456 |

密码在数据库中以 SHA256 哈希存储。

## 许可证

本项目仅供学习交流使用。
