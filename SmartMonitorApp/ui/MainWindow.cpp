/**
 * @file    MainWindow.cpp
 * @brief   主窗口实现
 *
 * 关键实现细节：
 *     - updateDeviceTable()：用m_deviceRowMap避免重复插入，同一设备只更新行
 *     - checkAlarm()：温度>80或压力>1.5时触发报警，写入数据库并在状态栏提示
 *     - onSimulateData()：无真实设备时用随机数据模拟，方便开发调试
 */

#include "MainWindow.h"
#include "DeviceDetail.h"
#include "HistoryQuery.h"
#include "../database/DbManager.h"
#include "../database/DataExporter.h"
#include "../network/DataParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QGroupBox>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_tcpClient(new TcpClient(this)),
      m_recvThread(nullptr),
      m_simulateTimer(new QTimer(this)),
      m_connected(false),
      m_deviceCount(0)
{
    setupUI();
    setupToolBar();
    setupStatusBar();
    setFixedSize(1000, 650);
    setWindowTitle("SmartMonitor - 工业设备监控系统");

    connect(m_tcpClient, &TcpClient::connectionStateChanged,
            this, &MainWindow::onConnectionStateChanged);
    connect(m_simulateTimer, &QTimer::timeout,
            this, &MainWindow::onSimulateData);
}

MainWindow::~MainWindow()
{
    if (m_recvThread) {
        m_recvThread->stop();
        m_recvThread->quit();
        m_recvThread->wait();
    }
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 顶部：连接控制区
    QGroupBox* connGroup = new QGroupBox("设备连接");
    QHBoxLayout* connLayout = new QHBoxLayout(connGroup);

    connLayout->addWidget(new QLabel("状态:"));
    m_connectionStatus = new QLabel("未连接");
    m_connectionStatus->setObjectName("connStatus");
    connLayout->addWidget(m_connectionStatus);
    connLayout->addStretch();

    m_connectBtn = new QPushButton("连接设备");
    m_disconnectBtn = new QPushButton("断开连接");
    m_disconnectBtn->setEnabled(false);
    QPushButton* simulateBtn = new QPushButton("模拟数据");
    connLayout->addWidget(m_connectBtn);
    connLayout->addWidget(m_disconnectBtn);
    connLayout->addWidget(simulateBtn);
    mainLayout->addWidget(connGroup);

    // 中部：设备统计
    QHBoxLayout* statsLayout = new QHBoxLayout;
    m_deviceCountLabel = new QLabel("在线设备: 0");
    m_deviceCountLabel->setObjectName("stats");
    statsLayout->addWidget(m_deviceCountLabel);
    statsLayout->addStretch();
    mainLayout->addLayout(statsLayout);

    // 底部：设备数据表格
    m_deviceTable = new QTableWidget;
    m_deviceTable->setColumnCount(8);
    QStringList headers;
    headers << "设备编号" << "设备名称" << "状态" << "温度(℃)"
            << "压力(MPa)" << "转速(RPM)" << "电压(V)" << "更新时间";
    m_deviceTable->setHorizontalHeaderLabels(headers);
    m_deviceTable->horizontalHeader()->setStretchLastSection(true);
    m_deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_deviceTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_deviceTable);

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(simulateBtn, &QPushButton::clicked, this, [this]() {
        m_simulateTimer->start(2000);
    });
}

void MainWindow::setupToolBar()
{
    QToolBar* toolBar = addToolBar("工具栏");
    QAction* exportAction  = toolBar->addAction("导出数据");
    QAction* historyAction = toolBar->addAction("历史查询");
    QAction* detailAction  = toolBar->addAction("设备详情");

    connect(exportAction,  &QAction::triggered, this, &MainWindow::onExportClicked);
    connect(historyAction, &QAction::triggered, this, &MainWindow::onHistoryClicked);
    connect(detailAction,  &QAction::triggered, this, &MainWindow::onDeviceDetailClicked);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("就绪 | SmartMonitor v1.0");
}

void MainWindow::onConnectClicked()
{
    QString ip = "127.0.0.1";
    quint16 port = 1234;

    statusBar()->showMessage(QString("正在连接 %1:%2 ...").arg(ip).arg(port));

    if (m_tcpClient->connectToHost(ip, port)) {
        m_connected = true;
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);

        // 启动接收线程
        m_recvThread = new RecvThread(m_tcpClient, this);
        connect(m_recvThread, &RecvThread::deviceDataReady,
                this, &MainWindow::onDeviceDataReceived);
        m_recvThread->start();
    } else {
        statusBar()->showMessage("连接失败，请检查设备网络", 5000);
    }
}

void MainWindow::onDisconnectClicked()
{
    if (m_recvThread) {
        m_recvThread->stop();
        m_recvThread->quit();
        m_recvThread->wait();
        delete m_recvThread;
        m_recvThread = nullptr;
    }
    m_tcpClient->disconnect();
    m_simulateTimer->stop();
    m_connected = false;
    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    statusBar()->showMessage("已断开连接");
}

void MainWindow::onConnectionStateChanged(bool connected)
{
    if (connected) {
        m_connectionStatus->setText("已连接");
        m_connectionStatus->setStyleSheet("color: green; font-weight: bold;");
    } else {
        m_connectionStatus->setText("未连接");
        m_connectionStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MainWindow::onDeviceDataReceived(const DeviceInfo& info)
{
    updateDeviceTable(info);
    checkAlarm(info);
    DbManager::instance().insertDeviceData(info);
}

void MainWindow::updateDeviceTable(const DeviceInfo& info)
{
    int row;
    if (m_deviceRowMap.contains(info.id)) {
        row = m_deviceRowMap[info.id];
    } else {
        row = m_deviceTable->rowCount();
        m_deviceTable->insertRow(row);
        m_deviceRowMap[info.id] = row;
        m_deviceCount++;
        m_deviceCountLabel->setText(QString("在线设备: %1").arg(m_deviceCount));
    }

    m_deviceTable->setItem(row, 0, new QTableWidgetItem(QString::number(info.id)));
    m_deviceTable->setItem(row, 1, new QTableWidgetItem(info.name));
    m_deviceTable->setItem(row, 2, new QTableWidgetItem(DeviceInfo::statusToString(info.status)));
    m_deviceTable->setItem(row, 3, new QTableWidgetItem(QString::number(info.temperature, 'f', 1)));
    m_deviceTable->setItem(row, 4, new QTableWidgetItem(QString::number(info.pressure, 'f', 2)));
    m_deviceTable->setItem(row, 5, new QTableWidgetItem(QString::number(info.speed, 'f', 0)));
    m_deviceTable->setItem(row, 6, new QTableWidgetItem(QString::number(info.voltage, 'f', 1)));
    m_deviceTable->setItem(row, 7, new QTableWidgetItem(info.updateTime.toString("HH:mm:ss")));

    // 报警状态整行标红
    if (info.status == DeviceStatus::Alarm) {
        for (int col = 0; col < 8; col++) {
            m_deviceTable->item(row, col)->setBackground(QColor(255, 200, 200));
        }
    }
}

void MainWindow::checkAlarm(const DeviceInfo& info)
{
    if (info.temperature > 80.0) {
        AlarmRecord alarm;
        alarm.deviceId   = info.id;
        alarm.deviceName = info.name;
        alarm.alarmType  = "高温报警";
        alarm.description = QString("温度 %1℃ 超过阈值 80℃").arg(info.temperature, 0, 'f', 1);
        alarm.alarmTime  = QDateTime::currentDateTime();
        alarm.resolved   = false;
        DbManager::instance().insertAlarm(alarm);
        statusBar()->showMessage(QString("[报警] %1: %2").arg(info.name, alarm.description), 10000);
    }
    if (info.pressure > 1.5) {
        AlarmRecord alarm;
        alarm.deviceId   = info.id;
        alarm.deviceName = info.name;
        alarm.alarmType  = "高压报警";
        alarm.description = QString("压力 %1MPa 超过阈值 1.5MPa").arg(info.pressure, 0, 'f', 2);
        alarm.alarmTime  = QDateTime::currentDateTime();
        alarm.resolved   = false;
        DbManager::instance().insertAlarm(alarm);
        statusBar()->showMessage(QString("[报警] %1: %2").arg(info.name, alarm.description), 10000);
    }
}

void MainWindow::onSimulateData()
{
    static int tick = 0;
    tick++;

    struct SimDevice {
        int id;
        QString name;
        double baseTemp, basePressure, baseSpeed, baseVoltage;
    };

    QVector<SimDevice> devices = {
        {1, "1号压缩机", 45.0, 0.8, 3000, 220},
        {2, "2号压缩机", 50.0, 1.0, 3200, 220},
        {3, "1号水泵",   35.0, 0.5, 1500, 380},
        {4, "2号水泵",   38.0, 0.6, 1600, 380}
    };

    for (const auto& dev : devices) {
        float temp     = dev.baseTemp + QRandomGenerator::global()->bounded(20) - 10;
        float pressure = dev.basePressure + (QRandomGenerator::global()->bounded(10) - 5) * 0.1f;
        float speed    = dev.baseSpeed + QRandomGenerator::global()->bounded(200) - 100;
        float voltage  = dev.baseVoltage + QRandomGenerator::global()->bounded(10) - 5;
        int status = (tick % 15 == 0 && dev.id == 1) ? 2 : 0;

        QByteArray frame = DataParser::buildTestData(dev.id, dev.name, temp, pressure, speed, voltage, status);
        QList<DeviceInfo> parsed = DataParser::parse(frame);

        for (const auto& info : parsed) {
            DeviceInfo mutableInfo = info;
            mutableInfo.name = dev.name;
            onDeviceDataReceived(mutableInfo);
        }
    }

    statusBar()->showMessage(QString("模拟数据更新 #%1").arg(tick), 3000);
}

void MainWindow::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出数据", "export.csv", "CSV文件 (*.csv)");
    if (filePath.isEmpty()) return;

    QDateTime now = QDateTime::currentDateTime();
    QList<DeviceInfo> data = DbManager::instance().queryDeviceData("", now.addDays(-1), now);

    if (DataExporter::exportToCsv(filePath, data)) {
        QMessageBox::information(this, "导出成功",
            QString("已导出 %1 条数据到:\n%2").arg(data.size()).arg(filePath));
    } else {
        QMessageBox::warning(this, "导出失败", "无法写入文件，请检查路径权限");
    }
}

void MainWindow::onHistoryClicked()
{
    HistoryQuery* historyWin = new HistoryQuery(this);
    historyWin->exec();
}

void MainWindow::onDeviceDetailClicked()
{
    DeviceDetail* detailWin = new DeviceDetail(this);
    detailWin->exec();
}
