/**
 * @file    DeviceServer.cpp
 * @brief   工业设备模拟服务器实现
 *
 * 关键实现细节：
 *     - onNewConnection()：用while循环处理所有待处理连接（可能同时来多个）
 *     - onSendData()：模拟4台设备，每台设备的参数在基准值附近随机波动
 *     - buildFrame()：按大端序组装23字节二进制帧，与DataParser协议一致
 */

#include "DeviceServer.h"
#include <QtEndian>

DeviceServer::DeviceServer(QObject* parent)
    : QObject(parent),
      m_server(new QTcpServer(this)),
      m_sendTimer(new QTimer(this)),
      m_tick(0)
{
    connect(m_server, &QTcpServer::newConnection, this, &DeviceServer::onNewConnection);
    connect(m_sendTimer, &QTimer::timeout, this, &DeviceServer::onSendData);
}

DeviceServer::~DeviceServer()
{
    stop();
}

bool DeviceServer::start(quint16 port)
{
    if (m_server->isListening()) {
        return true;
    }

    if (!m_server->listen(QHostAddress::Any, port)) {
        emit logMessage(QString("监听端口 %1 失败: %2").arg(port).arg(m_server->errorString()));
        return false;
    }

    m_sendTimer->start(1000);  // 每1秒发送一轮数据
    emit logMessage(QString("服务器已启动，监听端口 %1").arg(port));
    return true;
}

void DeviceServer::stop()
{
    m_sendTimer->stop();
    m_server->close();

    for (QTcpSocket* client : m_clients) {
        client->disconnectFromHost();
    }
    m_clients.clear();
    m_tick = 0;

    emit clientCountChanged(0);
    emit logMessage("服务器已停止");
}

bool DeviceServer::isRunning() const
{
    return m_server->isListening();
}

int DeviceServer::clientCount() const
{
    return m_clients.size();
}

void DeviceServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket* client = m_server->nextPendingConnection();
        m_clients.append(client);

        connect(client, &QTcpSocket::disconnected, this, &DeviceServer::onClientDisconnected);

        QString addr = QString("%1:%2")
            .arg(client->peerAddress().toString())
            .arg(client->peerPort());
        emit logMessage(QString("新客户端连接: %1").arg(addr));
        emit clientCountChanged(m_clients.size());
    }
}

void DeviceServer::onClientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    m_clients.removeOne(client);
    client->deleteLater();  // 延迟删除，避免回调中崩溃

    emit logMessage("客户端断开");
    emit clientCountChanged(m_clients.size());
}

void DeviceServer::onSendData()
{
    if (m_clients.isEmpty()) return;

    m_tick++;

    // 模拟4台设备的基准参数
    struct SimDevice {
        int id;
        float baseTemp, basePressure, baseSpeed, baseVoltage;
    };

    SimDevice devices[] = {
        {1, 45.0f, 0.8f, 3000.0f, 220.0f},
        {2, 50.0f, 1.0f, 3200.0f, 220.0f},
        {3, 35.0f, 0.5f, 1500.0f, 380.0f},
        {4, 38.0f, 0.6f, 1600.0f, 380.0f}
    };

    QRandomGenerator* rng = QRandomGenerator::global();

    for (const auto& dev : devices) {
        // 在基准值附近随机波动
        float temp     = dev.baseTemp + rng->bounded(20) - 10;
        float pressure = dev.basePressure + (rng->bounded(10) - 5) * 0.1f;
        float speed    = dev.baseSpeed + rng->bounded(200) - 100;
        float voltage  = dev.baseVoltage + rng->bounded(10) - 5;

        // 设备1每隔15轮触发一次报警；温度超80也报警
        int status = (m_tick % 15 == 0 && dev.id == 1) ? 1 : 0;
        if (temp > 80.0f) status = 1;

        QByteArray frame = buildFrame(dev.id, temp, pressure, speed, voltage, status);

        // 广播给所有已连接的客户端
        for (QTcpSocket* client : m_clients) {
            if (client->state() == QAbstractSocket::ConnectedState) {
                client->write(frame);
            }
        }
    }

    emit logMessage(QString("已发送第 %1 轮数据（%2 台设备，%3 个客户端）")
                    .arg(m_tick).arg(4).arg(m_clients.size()));
}

QByteArray DeviceServer::buildFrame(int deviceId, float temp, float pressure,
                                      float speed, float voltage, int status)
{
    // 协议格式（23字节）：帧头(2) + 设备ID(2) + 温度(4) + 压力(4) + 转速(4) + 电压(4) + 状态(1) + 帧尾(2)
    QByteArray frame(23, 0);

    frame[0] = static_cast<char>(0xAA);  // 帧头
    frame[1] = static_cast<char>(0x55);

    qToBigEndian(static_cast<quint16>(deviceId), reinterpret_cast<uchar*>(frame.data() + 2));
    qToBigEndian(temp,     reinterpret_cast<uchar*>(frame.data() + 4));
    qToBigEndian(pressure, reinterpret_cast<uchar*>(frame.data() + 8));
    qToBigEndian(speed,    reinterpret_cast<uchar*>(frame.data() + 12));
    qToBigEndian(voltage,  reinterpret_cast<uchar*>(frame.data() + 16));

    frame[20] = static_cast<char>(status);

    frame[21] = static_cast<char>(0x55);  // 帧尾
    frame[22] = static_cast<char>(0xAA);

    return frame;
}
