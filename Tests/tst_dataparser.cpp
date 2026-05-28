/**
 * @file    tst_dataparser.cpp
 * @brief   DataParser 单元测试实现
 */

#include "tst_dataparser.h"
#include "network/DataParser.h"
#include <QtEndian>

void TestDataParser::testParseSingleFrame()
{
    // 构造一帧测试数据：设备ID=1, 温度=45.5, 压力=0.8, 转速=3000, 电压=220, 状态=0
    QByteArray frame = DataParser::buildTestData(1, "测试设备", 45.5f, 0.8f, 3000.0f, 220.0f, 0);

    QList<DeviceInfo> result = DataParser::parse(frame);

    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].id, 1);
    QVERIFY(qAbs(result[0].temperature - 45.5) < 0.01);
    QVERIFY(qAbs(result[0].pressure - 0.8) < 0.01);
    QVERIFY(qAbs(result[0].speed - 3000.0) < 1.0);
    QVERIFY(qAbs(result[0].voltage - 220.0) < 0.1);
    QCOMPARE(static_cast<int>(result[0].status), 0);
}

void TestDataParser::testParseMultipleFrames()
{
    // 构造3帧连续数据
    QByteArray multiFrame;
    multiFrame.append(DataParser::buildTestData(1, "设备1", 45.0f, 0.8f, 3000.0f, 220.0f, 0));
    multiFrame.append(DataParser::buildTestData(2, "设备2", 50.0f, 1.0f, 3200.0f, 220.0f, 0));
    multiFrame.append(DataParser::buildTestData(3, "设备3", 35.0f, 0.5f, 1500.0f, 380.0f, 1));

    QList<DeviceInfo> result = DataParser::parse(multiFrame);

    QCOMPARE(result.size(), 3);
    QCOMPARE(result[0].id, 1);
    QCOMPARE(result[1].id, 2);
    QCOMPARE(result[2].id, 3);
    QCOMPARE(static_cast<int>(result[2].status), 1);
}

void TestDataParser::testParseInvalidData()
{
    // 空数据
    QList<DeviceInfo> result1 = DataParser::parse(QByteArray());
    QCOMPARE(result1.size(), 0);

    // 随机垃圾数据
    QByteArray garbage("hello world this is garbage data");
    QList<DeviceInfo> result2 = DataParser::parse(garbage);
    QCOMPARE(result2.size(), 0);

    // 不完整的帧（只有帧头）
    QByteArray incomplete;
    incomplete.append(static_cast<char>(0xAA));
    incomplete.append(static_cast<char>(0x55));
    QList<DeviceInfo> result3 = DataParser::parse(incomplete);
    QCOMPARE(result3.size(), 0);
}

void TestDataParser::testBuildAndParseConsistency()
{
    // buildTestData 和 parse 应该是互逆操作
    float temp = 67.8f;
    float pressure = 1.23f;
    float speed = 2850.0f;
    float voltage = 378.5f;

    QByteArray frame = DataParser::buildTestData(42, "一致性测试", temp, pressure, speed, voltage, 2);
    QList<DeviceInfo> result = DataParser::parse(frame);

    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].id, 42);
    QVERIFY(qAbs(result[0].temperature - static_cast<double>(temp)) < 0.01);
    QVERIFY(qAbs(result[0].pressure - static_cast<double>(pressure)) < 0.01);
    QVERIFY(qAbs(result[0].speed - static_cast<double>(speed)) < 1.0);
    QVERIFY(qAbs(result[0].voltage - static_cast<double>(voltage)) < 0.1);
    QCOMPARE(static_cast<int>(result[0].status), 2);
}
