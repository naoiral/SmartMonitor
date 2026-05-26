/**
 * @file    DeviceInfo.h
 * @brief   设备信息数据模型
 *
 * 模块作用：
 *     定义工业设备的核心数据结构，是整个系统数据流转的基础。
 *     所有模块（网络、数据库、界面）都依赖这个头文件。
 *
 * 设计思路：
 *     用结构体（struct）而非类（class）来存储数据，因为这里是纯数据，
 *     不需要复杂的业务逻辑。结构体默认public，方便各模块直接访问。
 *     枚举类（enum class）比普通enum更安全，不会隐式转换为int。
 *
 * 面试考点：
 *     1. struct和class的区别？答：默认访问权限不同，struct默认public，class默认private。
 *     2. enum class和enum的区别？答：enum class有类型安全，不会隐式转int，不会命名冲突。
 *     3. 为什么用static函数？答：不需要实例就能调用，适合工具函数。
 */

#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>
#include <QDateTime>

/**
 * @brief 设备运行状态枚举
 *     Online      - 设备正常运行
 *     Offline     - 设备离线（未连接或断开）
 *     Alarm       - 设备报警（温度/压力超限等）
 *     Maintenance - 设备维护中（人工标记）
 */
enum class DeviceStatus {
    Online,
    Offline,
    Alarm,
    Maintenance
};

/**
 * @brief 设备信息结构体
 *     一台设备的所有运行参数都存在这里。
 *     每次从设备收到数据，就会生成一个DeviceInfo，然后传给界面显示、存入数据库。
 */
struct DeviceInfo {
    int          id;           // 设备编号（唯一标识）
    QString      name;         // 设备名称，如"1号压缩机"
    DeviceStatus status;       // 当前运行状态
    double       temperature;  // 温度（℃）
    double       pressure;     // 压力（MPa）
    double       speed;        // 转速（RPM）
    double       voltage;      // 电压（V）
    QDateTime    updateTime;   // 数据采集时间

    DeviceInfo()
        : id(0), status(DeviceStatus::Offline),
          temperature(0), pressure(0), speed(0), voltage(0) {}

    /**
     * @brief 将设备状态枚举转为中文字符串（界面显示用）
     * @param s  设备状态枚举值
     * @return 对应的中文描述
     */
    static QString statusToString(DeviceStatus s) {
        switch (s) {
        case DeviceStatus::Online:      return "运行中";
        case DeviceStatus::Offline:     return "离线";
        case DeviceStatus::Alarm:       return "报警";
        case DeviceStatus::Maintenance: return "维护中";
        default:                        return "未知";
        }
    }
};

#endif // DEVICEINFO_H
