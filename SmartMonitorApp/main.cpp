/**
 * @file    main.cpp
 * @brief   程序入口
 *
 * 启动流程：
 *     1. 创建QApplication（Qt事件循环管理器）
 *     2. 初始化数据库（建表）
 *     3. 加载QSS样式表（美化界面）
 *     4. 显示登录窗口
 *     5. 进入事件循环（a.exec()）
 */

#include <QApplication>
#include <QFile>
#include "ui/LoginWindow.h"
#include "database/DbManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 初始化数据库（建表，如果已存在则跳过）
    DbManager::instance().init();

    // 加载QSS样式表
    QFile styleFile(":/qss/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    LoginWindow loginWin;
    loginWin.show();

    return app.exec();
}
