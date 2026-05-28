/**
 * @file    DbManager.cpp
 * @brief   数据库管理器实现
 *
 * 关键实现细节：
 *     - init()中用CREATE TABLE IF NOT EXISTS，重复调用不会报错
 *     - 所有SQL用prepare+addBindValue，防止SQL注入
 *     - 查询结果逐行映射到DeviceInfo/AlarmRecord结构体
 */

#include "DbManager.h"
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QCryptographicHash>

DbManager::DbManager() {}

DbManager::~DbManager()
{
    close();
}

DbManager& DbManager::instance()
{
    static DbManager inst;  // C++11保证线程安全的局部静态变量
    return inst;
}

bool DbManager::init()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QCoreApplication::applicationDirPath() + "/smartmonitor.db";
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "数据库打开失败:" << m_db.lastError().text();
        return false;
    }

    // 建设备数据表（IF NOT EXISTS保证重复调用不报错）
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS device_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "device_id INTEGER,"
               "device_name TEXT,"
               "status INTEGER,"
               "temperature REAL,"
               "pressure REAL,"
               "speed REAL,"
               "voltage REAL,"
               "update_time TEXT"
               ")");

    // 建报警记录表
    query.exec("CREATE TABLE IF NOT EXISTS alarm_record ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "device_id INTEGER,"
               "device_name TEXT,"
               "alarm_type TEXT,"
               "description TEXT,"
               "alarm_time TEXT,"
               "resolved INTEGER"
               ")");

    // 初始化用户表（含默认admin账户）
    initUserTable();

    return true;
}

void DbManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void DbManager::insertDeviceData(const DeviceInfo& info)
{
    QSqlQuery query;
    query.prepare("INSERT INTO device_data "
                  "(device_id, device_name, status, temperature, pressure, speed, voltage, update_time) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(info.id);
    query.addBindValue(info.name);
    query.addBindValue(static_cast<int>(info.status));
    query.addBindValue(info.temperature);
    query.addBindValue(info.pressure);
    query.addBindValue(info.speed);
    query.addBindValue(info.voltage);
    query.addBindValue(info.updateTime.toString("yyyy-MM-dd HH:mm:ss"));
    query.exec();
}

QList<DeviceInfo> DbManager::queryDeviceData(const QString& deviceId,
                                              const QDateTime& start,
                                              const QDateTime& end)
{
    QList<DeviceInfo> result;
    QSqlQuery query;

    // deviceId为空时查所有设备，否则按设备ID过滤
    if (deviceId.isEmpty()) {
        query.prepare("SELECT * FROM device_data "
                      "WHERE update_time BETWEEN ? AND ? ORDER BY update_time DESC");
        query.addBindValue(start.toString("yyyy-MM-dd HH:mm:ss"));
        query.addBindValue(end.toString("yyyy-MM-dd HH:mm:ss"));
    } else {
        query.prepare("SELECT * FROM device_data "
                      "WHERE device_id = ? AND update_time BETWEEN ? AND ? "
                      "ORDER BY update_time DESC");
        query.addBindValue(deviceId.toInt());
        query.addBindValue(start.toString("yyyy-MM-dd HH:mm:ss"));
        query.addBindValue(end.toString("yyyy-MM-dd HH:mm:ss"));
    }

    query.exec();
    while (query.next()) {
        DeviceInfo info;
        info.id          = query.value("device_id").toInt();
        info.name        = query.value("device_name").toString();
        info.status      = static_cast<DeviceStatus>(query.value("status").toInt());
        info.temperature = query.value("temperature").toDouble();
        info.pressure    = query.value("pressure").toDouble();
        info.speed       = query.value("speed").toDouble();
        info.voltage     = query.value("voltage").toDouble();
        info.updateTime  = QDateTime::fromString(
            query.value("update_time").toString(), "yyyy-MM-dd HH:mm:ss");
        result.append(info);
    }

    return result;
}

void DbManager::insertAlarm(const AlarmRecord& record)
{
    QSqlQuery query;
    query.prepare("INSERT INTO alarm_record "
                  "(device_id, device_name, alarm_type, description, alarm_time, resolved) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(record.deviceId);
    query.addBindValue(record.deviceName);
    query.addBindValue(record.alarmType);
    query.addBindValue(record.description);
    query.addBindValue(record.alarmTime.toString("yyyy-MM-dd HH:mm:ss"));
    query.addBindValue(record.resolved ? 1 : 0);
    query.exec();
}

QList<AlarmRecord> DbManager::queryAlarms(int deviceId)
{
    QList<AlarmRecord> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM alarm_record WHERE device_id = ? ORDER BY alarm_time DESC");
    query.addBindValue(deviceId);
    query.exec();

    while (query.next()) {
        AlarmRecord record;
        record.id          = query.value("id").toInt();
        record.deviceId    = query.value("device_id").toInt();
        record.deviceName  = query.value("device_name").toString();
        record.alarmType   = query.value("alarm_type").toString();
        record.description = query.value("description").toString();
        record.alarmTime   = QDateTime::fromString(
            query.value("alarm_time").toString(), "yyyy-MM-dd HH:mm:ss");
        record.resolved    = query.value("resolved").toInt() == 1;
        result.append(record);
    }

    return result;
}

void DbManager::initUserTable()
{
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS user_account ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "password_hash TEXT NOT NULL"
               ")");

    // 检查是否有用户，没有则创建默认admin账户
    query.exec("SELECT COUNT(*) FROM user_account");
    if (query.next() && query.value(0).toInt() == 0) {
        addUser("admin", "123456");
        qDebug() << "已创建默认管理员账户: admin";
    }
}

QString DbManager::hashPassword(const QString& password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

bool DbManager::verifyUser(const QString& username, const QString& password)
{
    QSqlQuery query;
    query.prepare("SELECT password_hash FROM user_account WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        return false;
    }

    QString storedHash = query.value(0).toString();
    return storedHash == hashPassword(password);
}

bool DbManager::addUser(const QString& username, const QString& password)
{
    QSqlQuery query;
    query.prepare("INSERT INTO user_account (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    return query.exec();
}
