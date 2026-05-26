/**
 * @file    HistoryQuery.cpp
 * @brief   历史数据查询窗口实现
 */

#include "HistoryQuery.h"
#include "../database/DbManager.h"
#include "../model/DeviceInfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

HistoryQuery::HistoryQuery(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setFixedSize(750, 500);
    setWindowTitle("历史数据查询");
}

void HistoryQuery::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 查询条件区
    QGroupBox* queryGroup = new QGroupBox("查询条件");
    QVBoxLayout* queryLayout = new QVBoxLayout(queryGroup);

    // 设备编号
    QHBoxLayout* row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("设备编号（留空查全部）:"));
    m_deviceIdEdit = new QLineEdit;
    m_deviceIdEdit->setPlaceholderText("输入设备编号");
    row1->addWidget(m_deviceIdEdit);
    queryLayout->addLayout(row1);

    // 时间范围
    QHBoxLayout* row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("开始时间:"));
    m_startTime = new QDateTimeEdit(QDateTime::currentDateTime().addDays(-7));
    m_startTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    row2->addWidget(m_startTime);

    row2->addWidget(new QLabel("结束时间:"));
    m_endTime = new QDateTimeEdit(QDateTime::currentDateTime());
    m_endTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    row2->addWidget(m_endTime);
    queryLayout->addLayout(row2);

    m_queryBtn = new QPushButton("查 询");
    queryLayout->addWidget(m_queryBtn);
    mainLayout->addWidget(queryGroup);

    // 数据表格
    m_dataTable = new QTableWidget;
    m_dataTable->setColumnCount(8);
    QStringList headers;
    headers << "设备编号" << "设备名称" << "状态" << "温度(℃)"
            << "压力(MPa)" << "转速(RPM)" << "电压(V)" << "更新时间";
    m_dataTable->setHorizontalHeaderLabels(headers);
    m_dataTable->horizontalHeader()->setStretchLastSection(true);
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_dataTable);

    connect(m_queryBtn, &QPushButton::clicked, this, &HistoryQuery::onQueryClicked);
}

void HistoryQuery::onQueryClicked()
{
    QString deviceId = m_deviceIdEdit->text().trimmed();
    QDateTime start  = m_startTime->dateTime();
    QDateTime end    = m_endTime->dateTime();

    QList<DeviceInfo> data = DbManager::instance().queryDeviceData(deviceId, start, end);

    m_dataTable->setRowCount(0);
    for (const auto& info : data) {
        int row = m_dataTable->rowCount();
        m_dataTable->insertRow(row);
        m_dataTable->setItem(row, 0, new QTableWidgetItem(QString::number(info.id)));
        m_dataTable->setItem(row, 1, new QTableWidgetItem(info.name));
        m_dataTable->setItem(row, 2, new QTableWidgetItem(DeviceInfo::statusToString(info.status)));
        m_dataTable->setItem(row, 3, new QTableWidgetItem(QString::number(info.temperature, 'f', 1)));
        m_dataTable->setItem(row, 4, new QTableWidgetItem(QString::number(info.pressure, 'f', 2)));
        m_dataTable->setItem(row, 5, new QTableWidgetItem(QString::number(info.speed, 'f', 0)));
        m_dataTable->setItem(row, 6, new QTableWidgetItem(QString::number(info.voltage, 'f', 1)));
        m_dataTable->setItem(row, 7, new QTableWidgetItem(info.updateTime.toString("yyyy-MM-dd HH:mm:ss")));
    }
}
