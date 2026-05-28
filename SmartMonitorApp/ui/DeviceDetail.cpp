/**
 * @file    DeviceDetail.cpp
 * @brief   设备详情窗口实现
 */

#include "DeviceDetail.h"
#include "../database/DbManager.h"
#include "../model/AlarmRecord.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

DeviceDetail::DeviceDetail(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setFixedSize(600, 450);
    setWindowTitle("设备详情");
}

void DeviceDetail::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 查询区
    QGroupBox* queryGroup = new QGroupBox("查询设备报警记录");
    QHBoxLayout* queryLayout = new QHBoxLayout(queryGroup);

    queryLayout->addWidget(new QLabel("设备编号:"));
    m_deviceIdEdit = new QLineEdit;
    m_deviceIdEdit->setPlaceholderText("输入设备编号，如 1");
    queryLayout->addWidget(m_deviceIdEdit);

    QPushButton* queryBtn = new QPushButton("查询");
    queryLayout->addWidget(queryBtn);
    mainLayout->addWidget(queryGroup);

    // 信息提示
    m_infoLabel = new QLabel("请输入设备编号后点击查询");
    mainLayout->addWidget(m_infoLabel);

    // 报警记录表格
    m_alarmTable = new QTableWidget;
    m_alarmTable->setColumnCount(5);
    QStringList headers;
    headers << "报警类型" << "报警描述" << "报警时间" << "设备名称" << "是否处理";
    m_alarmTable->setHorizontalHeaderLabels(headers);
    m_alarmTable->horizontalHeader()->setStretchLastSection(true);
    m_alarmTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_alarmTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_alarmTable);

    connect(queryBtn, &QPushButton::clicked, this, &DeviceDetail::onQueryClicked);
    connect(m_deviceIdEdit, &QLineEdit::returnPressed, this, &DeviceDetail::onQueryClicked);
}

void DeviceDetail::onQueryClicked()
{
    QString idText = m_deviceIdEdit->text().trimmed();
    if (idText.isEmpty()) {
        m_infoLabel->setText("请输入设备编号");
        return;
    }

    int deviceId = idText.toInt();
    QList<AlarmRecord> alarms = DbManager::instance().queryAlarms(deviceId);

    m_infoLabel->setText(QString("设备 %1 共有 %2 条报警记录").arg(deviceId).arg(alarms.size()));

    m_alarmTable->setRowCount(0);
    for (const auto& alarm : alarms) {
        int row = m_alarmTable->rowCount();
        m_alarmTable->insertRow(row);
        m_alarmTable->setItem(row, 0, new QTableWidgetItem(alarm.alarmType));
        m_alarmTable->setItem(row, 1, new QTableWidgetItem(alarm.description));
        m_alarmTable->setItem(row, 2, new QTableWidgetItem(alarm.alarmTime.toString("yyyy-MM-dd HH:mm:ss")));
        m_alarmTable->setItem(row, 3, new QTableWidgetItem(alarm.deviceName));
        m_alarmTable->setItem(row, 4, new QTableWidgetItem(alarm.resolved ? "已处理" : "未处理"));
    }
}
