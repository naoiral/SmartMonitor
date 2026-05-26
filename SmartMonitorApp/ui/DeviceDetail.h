/**
 * @file    DeviceDetail.h
 * @brief   设备详情窗口
 *
 * 模块作用：
 *     按设备编号查询并展示该设备的历史报警记录。
 *     从工具栏的"设备详情"按钮打开。
 *
 * 面试考点：
 *     1. QDialog的exec()和show()的区别？
 *        - exec()：模态对话框，阻塞父窗口，返回后才能操作父窗口
 *        - show()：非模态对话框，不阻塞父窗口
 *     2. 为什么继承QDialog而不是QWidget？
 *        - QDialog自带exec()/accept()/reject()，适合做弹窗
 */

#ifndef DEVICEDETAIL_H
#define DEVICEDETAIL_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class DeviceDetail : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDetail(QWidget* parent = nullptr);

private slots:
    void onQueryClicked();

private:
    void setupUI();

    QLineEdit*     m_deviceIdEdit;
    QTableWidget*  m_alarmTable;
    QLabel*        m_infoLabel;
};

#endif // DEVICEDETAIL_H
