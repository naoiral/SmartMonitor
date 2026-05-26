/**
 * @file    RecvThread.cpp
 * @brief   数据接收线程实现
 *
 * 关键实现细节：
 *     run()中用Qt::DirectConnection连接信号，确保数据解析在子线程执行。
 *     exec()启动子线程的事件循环，让信号槽在子线程中正常工作。
 *     stop()设置标志位并调用quit()退出事件循环。
 */

#include "RecvThread.h"
#include <QDebug>

RecvThread::RecvThread(TcpClient* client, QObject* parent)
    : QThread(parent), m_client(client), m_running(true)
{
}

RecvThread::~RecvThread()
{
    stop();
    wait();
}

void RecvThread::stop()
{
    m_running = false;
    quit();  // 退出事件循环
}

void RecvThread::run()
{
    // DirectConnection：确保dataReceived的处理在子线程中执行
    connect(m_client, &TcpClient::dataReceived, this, [this](const QByteArray& data) {
        QList<DeviceInfo> devices = DataParser::parse(data);
        for (const auto& info : devices) {
            emit deviceDataReady(info);
        }
    }, Qt::DirectConnection);

    // 启动子线程事件循环（阻塞在这里，直到quit()被调用）
    exec();
}
