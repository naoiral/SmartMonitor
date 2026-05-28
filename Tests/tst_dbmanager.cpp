/**
 * @file    tst_dbmanager.cpp
 * @brief   DbManager 单元测试实现
 */

#include "tst_dbmanager.h"
#include "database/DbManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>

void TestDbManager::initTestCase()
{
    // 测试前删除旧数据库，确保干净环境
    QString dbPath = QCoreApplication::applicationDirPath() + "/smartmonitor.db";
    QFile::remove(dbPath);
}

void TestDbManager::testInit()
{
    bool ok = DbManager::instance().init();
    QVERIFY(ok);
}

void TestDbManager::testInsertAndQueryDeviceData()
{
    // 插入一条设备数据
    DeviceInfo info;
    info.id = 1;
    info.name = "测试设备";
    info.status = DeviceStatus::Online;
    info.temperature = 55.5;
    info.pressure = 0.9;
    info.speed = 3000.0;
    info.voltage = 220.0;
    info.updateTime = QDateTime::currentDateTime();

    DbManager::instance().insertDeviceData(info);

    // 查询刚插入的数据
    QDateTime now = QDateTime::currentDateTime();
    QList<DeviceInfo> result = DbManager::instance().queryDeviceData(
        "1", now.addSecs(-10), now.addSecs(10));

    QVERIFY(result.size() >= 1);
    QCOMPARE(result[0].id, 1);
    QCOMPARE(result[0].name, QString("测试设备"));
    QVERIFY(qAbs(result[0].temperature - 55.5) < 0.01);
}

void TestDbManager::testInsertAndQueryAlarms()
{
    // 插入一条报警记录
    AlarmRecord alarm;
    alarm.deviceId = 1;
    alarm.deviceName = "测试设备";
    alarm.alarmType = "高温报警";
    alarm.description = "温度超过阈值";
    alarm.alarmTime = QDateTime::currentDateTime();
    alarm.resolved = false;

    DbManager::instance().insertAlarm(alarm);

    // 查询报警记录
    QList<AlarmRecord> result = DbManager::instance().queryAlarms(1);

    QVERIFY(result.size() >= 1);
    QCOMPARE(result[0].deviceId, 1);
    QCOMPARE(result[0].alarmType, QString("高温报警"));
    QCOMPARE(result[0].resolved, false);
}

void TestDbManager::testUserAuth()
{
    // 默认admin用户应该存在（init时自动创建）
    bool ok = DbManager::instance().verifyUser("admin", "123456");
    QVERIFY(ok);

    // 错误密码应该失败
    bool fail = DbManager::instance().verifyUser("admin", "wrongpass");
    QVERIFY(!fail);

    // 不存在的用户应该失败
    bool noUser = DbManager::instance().verifyUser("nonexistent", "123456");
    QVERIFY(!noUser);

    // 添加新用户
    bool added = DbManager::instance().addUser("testuser", "testpass");
    QVERIFY(added);

    // 验证新用户
    bool newOk = DbManager::instance().verifyUser("testuser", "testpass");
    QVERIFY(newOk);
}

void TestDbManager::cleanupTestCase()
{
    DbManager::instance().close();

    // 测试后删除测试数据库
    QString dbPath = QCoreApplication::applicationDirPath() + "/smartmonitor.db";
    QFile::remove(dbPath);
}
