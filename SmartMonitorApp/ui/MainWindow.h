/**
 * @file    MainWindow.h
 * @brief   主窗口（核心界面）
 *
 * 模块作用：
 *     系统的核心界面，显示所有设备的实时运行状态。
 *     工程师通过这个界面监控设备、查看报警、导出数据。
 *
 * 设计思路：
 *     采用MVC的思想：
 *     - Model层（DeviceInfo）提供数据
 *     - View层（QTableWidget）显示数据
 *     - Controller层（MainWindow的槽函数）处理用户交互
 *     TcpClient和RecvThread负责从设备获取数据，MainWindow负责展示。
 *
 * 面试考点：
 *     1. Qt的MainWindow架构？
 *        - QMainWindow包含菜单栏、工具栏、状态栏、中央窗口
 *        - 中央窗口用setCentralWidget设置，通常放布局管理器
 *     2. 信号槽连接的几种方式？
 *        - 直接连接：同线程，信号发出立即调用槽
 *        - 队列连接：跨线程，槽函数在接收者线程事件循环中调用
 *        - 自动连接（默认）：Qt自动判断同线程还是跨线程
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include "../network/TcpClient.h"
#include "../thread/RecvThread.h"
#include "../model/DeviceInfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onDeviceDataReceived(const DeviceInfo& info);
    void onConnectionStateChanged(bool connected);
    void onSimulateData();
    void onExportClicked();
    void onHistoryClicked();
    void onDeviceDetailClicked();

private:
    void setupUI();
    void setupToolBar();
    void setupStatusBar();
    void updateDeviceTable(const DeviceInfo& info);
    void checkAlarm(const DeviceInfo& info);
    void startRecvWorker();
    void stopRecvWorker();

    // 界面控件
    QTableWidget*  m_deviceTable;
    QLabel*        m_connectionStatus;
    QLabel*        m_deviceCountLabel;
    QPushButton*   m_connectBtn;
    QPushButton*   m_disconnectBtn;

    // 业务对象
    TcpClient*     m_tcpClient;
    RecvWorker*    m_recvWorker;
    QThread*       m_recvThread;
    QTimer*        m_simulateTimer;
    bool           m_connected;

    // 设备数据映射（设备ID -> 表格行号，用于更新而非重复插入）
    QMap<int, int> m_deviceRowMap;
    int            m_deviceCount;
};

#endif // MAINWINDOW_H
