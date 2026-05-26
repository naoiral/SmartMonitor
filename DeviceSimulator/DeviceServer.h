/**
 * @file    DeviceServer.h
 * @brief   工业设备模拟服务器
 *
 * 模块作用：
 *     模拟工业设备的TCP服务器，用于开发和测试阶段。
 *     监听指定端口，接受多个SmartMonitor客户端连接，
 *     每秒向所有客户端广播一帧设备数据。
 *
 * 设计思路：
 *     使用QTcpServer处理连接管理，QTimer定时触发数据发送。
 *     采用广播模式：一份数据同时发给所有已连接的客户端。
 *     数据帧格式与SmartMonitor的DataParser完全一致（23字节二进制协议）。
 *
 * 面试考点：
 *     1. QTcpServer和QTcpSocket的关系？
 *        - QTcpServer：监听端口，接受新连接，每个连接产生一个QTcpSocket
 *        - QTcpSocket：代表一条具体的TCP连接，用于读写数据
 *     2. 如何实现广播？
 *        - 维护客户端列表m_clients，发数据时遍历列表逐个发送
 *     3. deleteLater()的作用？
 *        - 延迟删除，等当前事件处理完再删除对象，避免野指针崩溃
 */

#ifndef DEVICESERVER_H
#define DEVICESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QList>
#include <QRandomGenerator>

class DeviceServer : public QObject
{
    Q_OBJECT

public:
    explicit DeviceServer(QObject* parent = nullptr);
    ~DeviceServer();

    bool start(quint16 port = 1234);
    void stop();
    bool isRunning() const;
    int clientCount() const;

signals:
    void clientCountChanged(int count);
    void logMessage(const QString& msg);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onSendData();

private:
    QByteArray buildFrame(int deviceId, float temp, float pressure,
                           float speed, float voltage, int status);

    QTcpServer*         m_server;      // TCP服务器
    QList<QTcpSocket*>  m_clients;     // 已连接的客户端列表
    QTimer*             m_sendTimer;   // 定时发送触发器
    int                 m_tick;        // 发送轮次计数
};

#endif // DEVICESERVER_H
