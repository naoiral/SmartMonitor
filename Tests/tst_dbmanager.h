/**
 * @file    tst_dbmanager.h
 * @brief   DbManager 单元测试
 *
 * 测试内容：
 *     1. 数据库初始化
 *     2. 设备数据插入和查询
 *     3. 报警记录插入和查询
 *     4. 用户认证
 */

#ifndef TST_DBMANAGER_H
#define TST_DBMANAGER_H

#include <QObject>
#include <QtTest>

class TestDbManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testInit();
    void testInsertAndQueryDeviceData();
    void testInsertAndQueryAlarms();
    void testUserAuth();
    void cleanupTestCase();
};

#endif // TST_DBMANAGER_H
