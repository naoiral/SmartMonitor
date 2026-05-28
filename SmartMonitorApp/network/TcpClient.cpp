/**
 * @file    TcpClient.cpp
 * @brief   TCP客户端实现
 *
 * 关键改动：
 *     - connectToHost()异步连接，不阻塞UI线程
 *     - 加5秒连接超时定时器，超时自动断开并通知
 *     - 支持自动重连：断开后每隔指定时间尝试重连
 */

#include "TcpClient.h"
#include <QDebug>

TcpClient::TcpClient(QObject* parent)
    : QObject(parent),
      m_socket(new QTcpSocket(this)),
      m_connectTimer(new QTimer(this)),
      m_reconnectTimer(new QTimer(this)),
      m_port(0),
      m_autoReconnect(false),
      m_reconnectInterval(3000)
{
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TcpClient::onError);

    m_connectTimer->setSingleShot(true);
    connect(m_connectTimer, &QTimer::timeout, this, &TcpClient::onConnectTimeout);

    connect(m_reconnectTimer, &QTimer::timeout, this, &TcpClient::tryReconnect);
}

TcpClient::~TcpClient()
{
    disconnect();
}

void TcpClient::connectToHost(const QString& ip, quint16 port)
{
    m_host = ip;
    m_port = port;

    // 异步连接，不阻塞
    m_socket->connectToHost(ip, port);
    // 启动5秒超时定时器
    m_connectTimer->start(5000);
}

void TcpClient::disconnect()
{
    m_connectTimer->stop();
    m_reconnectTimer->stop();

    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool TcpClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

bool TcpClient::sendData(const QByteArray& data)
{
    if (!isConnected()) return false;
    qint64 written = m_socket->write(data);
    return written == data.size();
}

void TcpClient::setAutoReconnect(bool enable)
{
    m_autoReconnect = enable;
}

bool TcpClient::autoReconnect() const
{
    return m_autoReconnect;
}

void TcpClient::setReconnectInterval(int msec)
{
    m_reconnectInterval = msec;
}

void TcpClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClient::onConnected()
{
    m_connectTimer->stop();
    m_reconnectTimer->stop();
    qDebug() << "已连接到设备";
    emit connectionStateChanged(true);
}

void TcpClient::onDisconnected()
{
    m_connectTimer->stop();
    qDebug() << "已断开连接";
    emit connectionStateChanged(false);

    // 断开后如果开启了自动重连，启动重连定时器
    if (m_autoReconnect && !m_reconnectTimer->isActive()) {
        m_reconnectTimer->start(m_reconnectInterval);
    }
}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString errMsg = m_socket->errorString();
    qDebug() << "连接错误:" << errMsg;
    emit errorOccurred(errMsg);
}

void TcpClient::onConnectTimeout()
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "连接超时";
        m_socket->abort();
        emit errorOccurred("连接超时");

        // 超时后如果开启了自动重连，启动重连定时器
        if (m_autoReconnect && !m_reconnectTimer->isActive()) {
            m_reconnectTimer->start(m_reconnectInterval);
        }
    }
}

void TcpClient::tryReconnect()
{
    if (isConnected()) {
        m_reconnectTimer->stop();
        return;
    }

    qDebug() << "尝试重连" << m_host << ":" << m_port;
    m_socket->connectToHost(m_host, m_port);
    m_connectTimer->start(5000);
}
