/**
 * @file    LoginWindow.cpp
 * @brief   登录窗口实现
 */

#include "LoginWindow.h"
#include "MainWindow.h"
#include "../database/DbManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setFixedSize(400, 300);
    setWindowTitle("SmartMonitor - 登录");
}

void LoginWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    // 标题
    QLabel* titleLabel = new QLabel("SmartMonitor");
    titleLabel->setObjectName("title");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 登录表单
    QGroupBox* formGroup = new QGroupBox("用户登录");
    QVBoxLayout* formLayout = new QVBoxLayout(formGroup);

    QHBoxLayout* userLayout = new QHBoxLayout;
    userLayout->addWidget(new QLabel("用户名:"));
    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setPlaceholderText("请输入用户名");
    userLayout->addWidget(m_usernameEdit);
    formLayout->addLayout(userLayout);

    QHBoxLayout* passLayout = new QHBoxLayout;
    passLayout->addWidget(new QLabel("密  码:"));
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    passLayout->addWidget(m_passwordEdit);
    formLayout->addLayout(passLayout);

    mainLayout->addWidget(formGroup);

    // 状态提示
    m_statusLabel = new QLabel;
    m_statusLabel->setObjectName("status");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    // 登录按钮
    m_loginBtn = new QPushButton("登 录");
    m_loginBtn->setObjectName("loginBtn");
    mainLayout->addWidget(m_loginBtn);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("用户名和密码不能为空");
        m_statusLabel->setStyleSheet("color: red;");
        return;
    }

    // 从数据库验证用户（密码用SHA256哈希存储）
    if (DbManager::instance().verifyUser(username, password)) {
        m_statusLabel->setText("登录成功，正在进入...");
        m_statusLabel->setStyleSheet("color: green;");
        MainWindow* mainWin = new MainWindow;
        mainWin->show();
        this->close();
    } else {
        m_statusLabel->setText("用户名或密码错误");
        m_statusLabel->setStyleSheet("color: red;");
    }
}
