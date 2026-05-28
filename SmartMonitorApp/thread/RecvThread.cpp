/**
 * @file    RecvThread.cpp
 * @brief   数据接收处理器实现
 *
 * 关键实现细节：
 *     RecvWorker被moveToThread移到子线程后：
 *     - startProcessing()在子线程中连接TcpClient的dataReceived信号
 *     - onDataReceived()在子线程中执行数据解析
 *     - deviceDataReady信号跨线程传回主线程更新UI
 *
 * 线程模型：
 *     主线程：TcpClient（socket读写） → dataReceived信号
 *     子线程：RecvWorker::onDataReceived（数据解析） → deviceDataReady信号
 *     主线程：MainWindow::onDeviceDataReceived（更新UI + 存数据库）
 */

#include "RecvThread.h"
#include <QDebug>

RecvWorker::RecvWorker(TcpClient* client, QObject* parent)
    : QObject(parent), m_client(client)
{
}

void RecvWorker::startProcessing()
{
    // 在子线程中连接信号，收到数据后在子线程解析
    connect(m_client, &TcpClient::dataReceived,
            this, &RecvWorker::onDataReceived);
    qDebug() << "RecvWorker: 数据处理已启动（线程:" << QThread::currentThreadId() << ")";
}

void RecvWorker::stopProcessing()
{
    disconnect(m_client, &TcpClient::dataReceived,
               this, &RecvWorker::onDataReceived);
    qDebug() << "RecvWorker: 数据处理已停止";
}

void RecvWorker::onDataReceived(const QByteArray& data)
{
    // 这个函数在子线程中执行，解析不阻塞UI
    QList<DeviceInfo> devices = DataParser::parse(data);
    for (const auto& info : devices) {
        emit deviceDataReady(info);
    }
}
