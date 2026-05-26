/**
 * @file    LoginWindow.h
 * @brief   登录窗口
 *
 * 模块作用：
 *     程序启动后的第一个界面，负责用户身份验证。
 *     登录成功后跳转到主窗口（MainWindow）。
 *
 * 设计思路：
 *     用QWidget而非QDialog实现，因为登录窗口是程序的入口窗口，
 *     登录成功后需要创建并显示MainWindow，然后关闭自身。
 *     界面采用纯代码布局（不用.ui文件），便于理解和维护。
 *
 * 面试考点：
 *     1. QWidget和QDialog的区别？
 *        - QWidget：通用窗口基类，可以独立显示
 *        - QDialog：对话框窗口，有exec()/accept()/reject()等模态对话框支持
 *     2. 界面布局方式？
 *        - 代码布局：QVBoxLayout/QHBoxLayout，灵活可控
 *        - Qt Designer：拖拽生成.ui文件，适合复杂界面
 *        - QML：声明式UI，适合动态界面和移动端
 */

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);

private slots:
    void onLoginClicked();

private:
    void setupUI();

    QLineEdit*   m_usernameEdit;
    QLineEdit*   m_passwordEdit;
    QPushButton* m_loginBtn;
    QLabel*      m_statusLabel;
};

#endif // LOGINWINDOW_H
