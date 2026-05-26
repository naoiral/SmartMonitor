/**
 * @file    SimMainWindow.cpp
 * @brief   设备模拟器主窗口实现
 */

#include "SimMainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDateTime>

SimMainWindow::SimMainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_server(new DeviceServer(this))
{
    setupUI();
    setFixedSize(500, 420);
    setWindowTitle("DeviceSimulator - 工业设备模拟器");

    connect(m_server, &DeviceServer::clientCountChanged,
            this, &SimMainWindow::onClientCountChanged);
    connect(m_server, &DeviceServer::logMessage,
            this, &SimMainWindow::onLogMessage);
}

void SimMainWindow::setupUI()
{
    QWidget* central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // 控制区
    QGroupBox* controlGroup = new QGroupBox("服务器控制");
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);

    m_startStopBtn = new QPushButton("启动服务器");
    m_startStopBtn->setMinimumHeight(40);
    m_startStopBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; font-size: 14px; font-weight: bold; border-radius: 4px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
    );
    controlLayout->addWidget(m_startStopBtn);

    QHBoxLayout* statusRow = new QHBoxLayout;
    statusRow->addWidget(new QLabel("服务器状态:"));
    m_statusLabel = new QLabel("已停止");
    m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
    statusRow->addWidget(m_statusLabel);
    statusRow->addStretch();
    statusRow->addWidget(new QLabel("端口: 1234"));
    statusRow->addStretch();
    statusRow->addWidget(new QLabel("已连接客户端:"));
    m_clientCountLabel = new QLabel("0");
    m_clientCountLabel->setStyleSheet("font-weight: bold; color: #2980b9; font-size: 16px;");
    statusRow->addWidget(m_clientCountLabel);
    controlLayout->addLayout(statusRow);

    mainLayout->addWidget(controlGroup);

    // 日志区
    QGroupBox* logGroup = new QGroupBox("运行日志");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet(
        "background-color: #2c3e50; color: #ecf0f1; "
        "font-family: Consolas, monospace; font-size: 12px;");
    logLayout->addWidget(m_logView);

    mainLayout->addWidget(logGroup);

    connect(m_startStopBtn, &QPushButton::clicked, this, &SimMainWindow::onStartStopClicked);
}

void SimMainWindow::onStartStopClicked()
{
    if (m_server->isRunning()) {
        m_server->stop();
        m_startStopBtn->setText("启动服务器");
        m_startStopBtn->setStyleSheet(
            "QPushButton { background-color: #27ae60; color: white; font-size: 14px; font-weight: bold; border-radius: 4px; }"
            "QPushButton:hover { background-color: #2ecc71; }"
        );
        m_statusLabel->setText("已停止");
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else {
        if (m_server->start(1234)) {
            m_startStopBtn->setText("停止服务器");
            m_startStopBtn->setStyleSheet(
                "QPushButton { background-color: #c0392b; color: white; font-size: 14px; font-weight: bold; border-radius: 4px; }"
                "QPushButton:hover { background-color: #e74c3c; }"
            );
            m_statusLabel->setText("运行中");
            m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
        }
    }
}

void SimMainWindow::onClientCountChanged(int count)
{
    m_clientCountLabel->setText(QString::number(count));
}

void SimMainWindow::onLogMessage(const QString& msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logView->append(QString("[%1] %2").arg(timestamp, msg));
}
