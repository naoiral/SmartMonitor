/**
 * @file    TcpClient.h
 * @brief   TCP客户端
 *
 * 模块作用：
 *     封装TCP Socket通信，负责连接设备服务器、发送指令、接收原始字节数据。
 *     是系统网络通信的基础层，上层的RecvThread依赖它来收数据。
 *
 * 设计思路：
 *     对QTcpSocket做薄封装，用信号（signal）把网络事件暴露给外部：
 *     - dataReceived：收到数据时触发，外部连这个信号就能拿到数据
 *     - connectionStateChanged：连接状态变化时触发
 *     - errorOccurred：出错时触发
 *     这样调用方不需要直接操作Socket，只需要连信号就行。
 *
 * 面试考点：
 *     1. TCP和UDP的区别？
 *        - TCP：可靠传输、有连接、有序、适合文件传输/命令控制
 *        - UDP：不可靠、无连接、速度快、适合视频流/游戏
 *     2. Qt的信号槽机制是什么？
 *        - 观察者模式的实现，发送者发信号，接收者的槽函数自动被调用
 *        - 解耦了发送者和接收者，发送者不需要知道谁在接收
 *     3. 阻塞和非阻塞Socket的区别？
 *        - 阻塞：调用read()时如果没有数据，程序会卡住等待
 *        - 非阻塞：调用read()时如果没有数据，立即返回，不卡住
 *        - Qt的QTcpSocket是基于事件循环的非阻塞模式
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>

/**
 * @brief TCP客户端
 *
 * 异步连接：调用connectToHost()后立即返回，通过connectionStateChanged信号通知结果。
 * 自动重连：断开后如果m_autoReconnect为true，自动每隔m_reconnectInterval毫秒重连。
 */
class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);
    ~TcpClient();

    void connectToHost(const QString& ip, quint16 port);
    void disconnect();
    bool isConnected() const;
    bool sendData(const QByteArray& data);

    void setAutoReconnect(bool enable);
    bool autoReconnect() const;
    void setReconnectInterval(int msec);

signals:
    void dataReceived(const QByteArray& data);
    void connectionStateChanged(bool connected);
    void errorOccurred(const QString& errorMsg);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onConnectTimeout();
    void tryReconnect();

private:
    QTcpSocket* m_socket;
    QTimer*     m_connectTimer;    // 连接超时定时器
    QTimer*     m_reconnectTimer;  // 重连定时器
    QString     m_host;
    quint16     m_port;
    bool        m_autoReconnect;
    int         m_reconnectInterval;
};

#endif // TCPCLIENT_H
