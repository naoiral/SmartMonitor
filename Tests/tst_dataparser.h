/**
 * @file    tst_dataparser.h
 * @brief   DataParser 单元测试
 *
 * 测试内容：
 *     1. 正常帧解析
 *     2. 多帧连续解析
 *     3. 无效数据容错
 *     4. buildTestData 与 parse 的一致性
 */

#ifndef TST_DATAPARSER_H
#define TST_DATAPARSER_H

#include <QObject>
#include <QtTest>

class TestDataParser : public QObject
{
    Q_OBJECT

private slots:
    void testParseSingleFrame();
    void testParseMultipleFrames();
    void testParseInvalidData();
    void testBuildAndParseConsistency();
};

#endif // TST_DATAPARSER_H
