/**
 * @file    TcpClient.cpp
 * @brief   TCP客户端实现
 *
 * 关键实现细节：
 *     - 构造函数中把Socket的系统信号连接到我们的槽函数
 *     - connectToHost()用waitForConnected()做3秒超时等待
 *     - onReadyRead()中readAll()读取所有可用数据并转发
 */

#include "TcpClient.h"
#include <QDebug>

TcpClient::TcpClient(QObject* parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    // 将Socket的系统信号连接到我们的处理函数
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TcpClient::onError);
}

TcpClient::~TcpClient()
{
    disconnect();
}

bool TcpClient::connectToHost(const QString& ip, quint16 port)
{
    m_socket->connectToHost(ip, port);
    return m_socket->waitForConnected(3000);  // 阻塞等待3秒
}

void TcpClient::disconnect()
{
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

void TcpClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClient::onConnected()
{
    qDebug() << "已连接到设备";
    emit connectionStateChanged(true);
}

void TcpClient::onDisconnected()
{
    qDebug() << "已断开连接";
    emit connectionStateChanged(false);
}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString errMsg = m_socket->errorString();
    qDebug() << "连接错误:" << errMsg;
    emit errorOccurred(errMsg);
}
