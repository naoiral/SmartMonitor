/**
 * @file    main.cpp
 * @brief   单元测试入口
 *
 * 运行方式：
 *     1. 用Qt Creator打开SmartMonitor.pro（包含Tests子项目）
 *     2. 构建并运行Tests
 *     3. 或者命令行：cd Tests && qmake && make && ./SmartMonitorTests
 */

#include <QApplication>
#include "tst_dataparser.h"
#include "tst_dbmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int status = 0;

    {
        TestDataParser test1;
        status |= QTest::qExec(&test1, argc, argv);
    }

    {
        TestDbManager test2;
        status |= QTest::qExec(&test2, argc, argv);
    }

    return status;
}
