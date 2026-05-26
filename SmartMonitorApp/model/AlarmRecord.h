/**
 * @file    AlarmRecord.h
 * @brief   报警记录数据模型
 *
 * 模块作用：
 *     定义设备报警记录的数据结构。
 *     当设备参数超过阈值时，系统会生成一条AlarmRecord，存入数据库。
 *     工程师可以通过"设备详情"窗口查看历史报警记录。
 *
 * 设计思路：
 *     报警记录和设备信息分开存储，因为：
 *     1. 设备数据量大（每秒更新），报警数据量小（偶尔触发）
 *     2. 查询逻辑不同：设备数据按时间范围查，报警按设备ID查
 *     3. 报警需要"是否已处理"字段，设备数据不需要
 *
 * 面试考点：
 *     1. 数据库表设计：为什么把报警和设备数据分两张表？答：数据量级不同，查询模式不同。
 *     2. resolved字段用bool还是int？SQLite不支持bool，用int(0/1)存储，代码里用bool操作。
 */

#ifndef ALARMRECORD_H
#define ALARMRECORD_H

#include <QString>
#include <QDateTime>

/**
 * @brief 报警记录结构体
 *     记录一次设备报警的完整信息，包括哪台设备、什么类型、什么时候、是否已处理。
 */
struct AlarmRecord {
    int       id;           // 记录编号（数据库自增主键）
    int       deviceId;     // 报警设备的编号
    QString   deviceName;   // 设备名称（冗余存储，方便查询时不用联表）
    QString   alarmType;    // 报警类型："高温报警"、"高压报警"等
    QString   description;  // 报警详情，如"温度92.5℃超过阈值80℃"
    QDateTime alarmTime;    // 报警触发时间
    bool      resolved;     // 是否已处理（true=已处理，false=未处理）

    AlarmRecord()
        : id(0), deviceId(0), resolved(false) {}
};

#endif // ALARMRECORD_H
