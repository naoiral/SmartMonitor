/**
 * @file    DbManager.h
 * @brief   数据库管理器
 *
 * 模块作用：
 *     封装所有数据库操作（建表、插入、查询），是系统数据持久化的核心。
 *     使用SQLite数据库，数据文件保存在程序同目录下。
 *
 * 设计思路：
 *     采用单例模式（Singleton），整个程序只有一个DbManager实例。
 *     原因：数据库连接是全局资源，多个实例会重复打开连接，浪费资源还可能锁表。
 *
 * 面试考点：
 *     1. 单例模式怎么实现？
 *        - 构造函数private，禁止外部创建
 *        - 静态instance()方法返回唯一实例
 *        - 删除拷贝构造和赋值运算符，防止复制
 *     2. 为什么用QSqlQuery的prepare/bind而不是拼接SQL字符串？
 *        - 防止SQL注入攻击（安全性）
 *        - 避免类型转换问题（健壮性）
 *     3. SQLite和MySQL的区别？
 *        - SQLite：轻量级、无需安装服务器、单文件存储，适合嵌入式和桌面应用
 *        - MySQL：需要独立服务器、支持高并发、适合Web后端
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
#include "../model/DeviceInfo.h"
#include "../model/AlarmRecord.h"

class DbManager
{
public:
    static DbManager& instance();

    bool init();
    void close();

    // 设备数据的增删改查（CRUD）
    void insertDeviceData(const DeviceInfo& info);
    QList<DeviceInfo> queryDeviceData(const QString& deviceId,
                                      const QDateTime& start,
                                      const QDateTime& end);

    // 报警记录的增删改查
    void insertAlarm(const AlarmRecord& record);
    QList<AlarmRecord> queryAlarms(int deviceId);

private:
    DbManager();
    ~DbManager();
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
