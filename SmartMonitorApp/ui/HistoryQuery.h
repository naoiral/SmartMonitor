/**
 * @file    HistoryQuery.h
 * @brief   历史数据查询窗口
 *
 * 模块作用：
 *     按设备编号和时间范围查询历史设备数据，结果以表格展示。
 *     从工具栏的"历史查询"按钮打开。
 *
 * 设计思路：
 *     查询条件：设备编号（可选）+ 时间范围（默认最近7天）。
 *     设备编号留空则查所有设备，方便做全局数据回顾。
 */

#ifndef HISTORYQUERY_H
#define HISTORYQUERY_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTimeEdit>

class HistoryQuery : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryQuery(QWidget* parent = nullptr);

private slots:
    void onQueryClicked();

private:
    void setupUI();

    QLineEdit*      m_deviceIdEdit;
    QDateTimeEdit*  m_startTime;
    QDateTimeEdit*  m_endTime;
    QTableWidget*   m_dataTable;
    QPushButton*    m_queryBtn;
};

#endif // HISTORYQUERY_H
