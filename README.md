# SmartMonitor - 工业设备监控系统

基于 Qt 开发的工业设备实时监控系统，支持设备数据采集、实时显示、报警管理和数据导出。

## 功能特性

- **实时监控**：通过 TCP 连接采集设备数据，实时显示温度、压力、转速、电压等参数
- **设备管理**：设备列表展示、设备详情查看
- **报警管理**：参数超限自动报警，报警记录查询
- **数据导出**：支持将历史数据导出为 CSV 文件
- **用户登录**：基础的用户认证功能
- **设备模拟器**：内置设备模拟工具，方便开发和测试

## 项目结构

```
SmartMonitor/
├── SmartMonitor.pro          # 项目主文件
├── SmartMonitorApp/          # 主应用程序
│   ├── main.cpp              # 程序入口
│   ├── ui/                   # 界面模块
│   │   ├── MainWindow        # 主窗口
│   │   ├── LoginWindow       # 登录窗口
│   │   ├── DeviceDetail      # 设备详情
│   │   └── HistoryQuery      # 历史查询
│   ├── network/              # 网络通信模块
│   │   ├── TcpClient         # TCP客户端
│   │   └── DataParser        # 数据解析器
│   ├── database/             # 数据库模块
│   │   ├── DbManager         # 数据库管理
│   │   └── DataExporter      # 数据导出
│   ├── model/                # 数据模型
│   │   ├── DeviceInfo        # 设备信息
│   │   └── AlarmRecord       # 报警记录
│   ├── thread/               # 线程模块
│   │   └── RecvThread        # 数据接收线程
│   └── resource/             # 资源文件
│       ├── qss/              # 样式表
│       └── resource.qrc      # 资源文件
└── DeviceSimulator/          # 设备模拟器
    ├── main.cpp              # 程序入口
    ├── DeviceServer          # 模拟服务器
    └── SimMainWindow         # 模拟器界面
```

## 技术栈

- **开发语言**：C++
- **UI 框架**：Qt 5/6
- **网络通信**：QTcpSocket / QTcpServer
- **数据库**：SQLite (QSqlDatabase)
- **多线程**：QThread
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

编译完成后会生成两个可执行文件：

1. **SmartMonitorApp** - 主监控程序
2. **DeviceSimulator** - 设备模拟器

建议先启动 DeviceSimulator，再启动 SmartMonitorApp 进行连接测试。

## 使用说明

1. 启动 DeviceSimulator，点击"启动服务器"开始模拟设备数据
2. 启动 SmartMonitorApp，输入服务器地址（默认 127.0.0.1:1234）
3. 点击"连接"按钮开始接收设备数据
4. 在主界面查看设备实时状态
5. 使用工具栏按钮查看设备详情、历史数据或导出报表

## 数据协议

系统使用自定义的二进制数据协议，帧长度 23 字节：

| 字段 | 长度 | 说明 |
|------|------|------|
| 帧头 | 2字节 | 固定 0xAA 0x55 |
| 设备ID | 2字节 | 设备编号 |
| 温度 | 4字节 | float 类型 |
| 压力 | 4字节 | float 类型 |
| 转速 | 4字节 | float 类型 |
| 电压 | 4字节 | float 类型 |
| 状态 | 1字节 | 设备状态 |

## 许可证

本项目仅供学习交流使用。
