/**
 * @file    main.cpp
 * @brief   DeviceSimulator程序入口
 *
 * 启动流程：
 *     1. 创建QApplication
 *     2. 创建并显示SimMainWindow
 *     3. 进入事件循环
 */

#include <QApplication>
#include "SimMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SimMainWindow window;
    window.show();

    return app.exec();
}
