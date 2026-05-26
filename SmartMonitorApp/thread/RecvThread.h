/**
 * @file    RecvThread.h
 * @brief   数据接收线程
 *
 * 模块作用：
 *     在后台线程中持续接收TCP数据并解析，避免阻塞主线程（UI线程）。
 *     解析好的DeviceInfo通过信号传给主线程更新界面。
 *
 * 设计思路：
 *     继承QThread，重写run()函数。
 *     在run()中连接TcpClient的dataReceived信号，用DataParser解析数据，
 *     解析完成后发deviceDataReady信号，主线程收到后更新界面和数据库。
 *
 * 面试考点：
 *     1. 为什么网络收发要用单独的线程？
 *        - 网络IO可能阻塞（等待数据到达），如果在主线程做，界面会卡死
 *        - 单独线程处理IO，主线程只负责刷新界面，保证流畅
 *     2. Qt跨线程信号槽怎么工作？
 *        - 默认是Qt::AutoConnection：跨线程时自动变成Qt::QueuedConnection
 *        - QueuedConnection：信号发送后，槽函数在接收者线程的事件循环中执行
 *        - 这保证了UI操作只在主线程执行，线程安全
 *     3. QThread的正确用法？
 *        - 方式一：继承QThread，重写run()（本项目使用的方式）
 *        - 方式二：创建QThread对象，用moveToSlot把Worker移到子线程
 *        - 推荐方式二，但方式一更直观，适合面试讲解
 */

#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>
#include "../network/TcpClient.h"
#include "../network/DataParser.h"
#include "../model/DeviceInfo.h"

class RecvThread : public QThread
{
    Q_OBJECT

public:
    explicit RecvThread(TcpClient* client, QObject* parent = nullptr);
    ~RecvThread();

    void stop();

signals:
    void deviceDataReady(const DeviceInfo& info);

protected:
    void run() override;

private:
    TcpClient* m_client;
    bool       m_running;
};

#endif // RECVTHREAD_H
