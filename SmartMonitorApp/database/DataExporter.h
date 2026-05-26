/**
 * @file    DataExporter.h
 * @brief   数据导出器
 *
 * 模块作用：
 *     将数据库中的设备数据导出为CSV文件，用户可以用Excel打开查看。
 *
 * 设计思路：
 *     全部用静态方法，不需要实例化。因为导出是无状态的操作，
 *     传入数据和路径就能完成，不需要保存任何内部状态。
 *
 * 面试考点：
 *     1. 什么时候用静态方法？答：工具函数、无状态操作、不需要访问成员变量时。
 *     2. CSV格式的注意事项？答：字段用逗号分隔，含逗号的字段要用引号包裹，编码用UTF-8。
 */

#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QString>
#include <QList>
#include "../model/DeviceInfo.h"

class DataExporter
{
public:
    static bool exportToCsv(const QString& filePath, const QList<DeviceInfo>& data);
};

#endif // DATAEXPORTER_H
