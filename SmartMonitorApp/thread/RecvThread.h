/**
 * @file    RecvThread.h
 * @brief   数据接收处理器
 *
 * 模块作用：
 *     在后台线程中接收TCP数据并解析，避免阻塞主线程（UI线程）。
 *     解析好的DeviceInfo通过信号传给主线程更新界面。
 *
 * 设计思路：
 *     使用Qt推荐的moveToThread模式（而非继承QThread重写run）：
 *     1. RecvWorker继承QObject，包含实际的数据处理逻辑
 *     2. MainWindow创建QThread，把RecvWorker移到子线程
 *     3. 通过信号槽跨线程通信，Qt自动处理线程安全
 *
 * 面试考点：
 *     1. QThread的两种用法？
 *        - 方式一：继承QThread，重写run()（早期做法，不推荐）
 *        - 方式二：QObject + moveToThread（Qt推荐，更灵活）
 *     2. 为什么推荐moveToThread？
 *        - Worker的所有槽函数都在子线程执行，不用手动管理线程
 *        - 信号槽自动跨线程，Qt负责线程安全
 *        - Worker可以接收来自任何线程的信号
 *     3. 跨线程信号槽怎么工作？
 *        - 默认Qt::AutoConnection：跨线程时自动变成Qt::QueuedConnection
 *        - QueuedConnection：信号发送后，槽函数在接收者线程的事件循环中执行
 */

#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QObject>
#include <QThread>
#include "../network/TcpClient.h"
#include "../network/DataParser.h"
#include "../model/DeviceInfo.h"

/**
 * @brief 数据接收Worker（运行在子线程中）
 *
 * 所有槽函数都在子线程执行，通过信号把结果传回主线程。
 */
class RecvWorker : public QObject
{
    Q_OBJECT

public:
    explicit RecvWorker(TcpClient* client, QObject* parent = nullptr);

public slots:
    void startProcessing();
    void stopProcessing();

signals:
    void deviceDataReady(const DeviceInfo& info);

private slots:
    void onDataReceived(const QByteArray& data);

private:
    TcpClient* m_client;
};

#endif // RECVTHREAD_H
