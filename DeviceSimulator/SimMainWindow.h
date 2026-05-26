/**
 * @file    SimMainWindow.h
 * @brief   设备模拟器主窗口
 *
 * 模块作用：
 *     DeviceSimulator的用户界面，提供服务器启动/停止控制，
 *     显示当前连接的客户端数量和运行日志。
 *
 * 设计思路：
 *     界面分为两部分：上半部分是控制区（按钮+状态），下半部分是日志区。
 *     日志用QTextEdit以追加方式显示，黑色背景绿色文字模拟终端风格。
 */

#ifndef SIMMAINWINDOW_H
#define SIMMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "DeviceServer.h"

class SimMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimMainWindow(QWidget* parent = nullptr);

private slots:
    void onStartStopClicked();
    void onClientCountChanged(int count);
    void onLogMessage(const QString& msg);

private:
    void setupUI();

    DeviceServer*  m_server;
    QPushButton*   m_startStopBtn;
    QLabel*        m_statusLabel;
    QLabel*        m_clientCountLabel;
    QTextEdit*     m_logView;
};

#endif // SIMMAINWINDOW_H
