/**
 * @file    DataExporter.cpp
 * @brief   数据导出器实现
 *
 * 实现细节：
 *     - 用QTextStream写文件，Qt6默认UTF-8编码
 *     - 第一行写表头，后续每行写一条设备数据
 *     - 返回true/false告诉调用方是否成功
 */

#include "DataExporter.h"
#include <QFile>
#include <QTextStream>

bool DataExporter::exportToCsv(const QString& filePath, const QList<DeviceInfo>& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // 写CSV表头
    out << "设备编号,设备名称,状态,温度(℃),压力(MPa),转速(RPM),电压(V),更新时间\n";

    // 逐行写设备数据
    for (const auto& info : data) {
        out << info.id << ","
            << info.name << ","
            << DeviceInfo::statusToString(info.status) << ","
            << info.temperature << ","
            << info.pressure << ","
            << info.speed << ","
            << info.voltage << ","
            << info.updateTime.toString("yyyy-MM-dd HH:mm:ss") << "\n";
    }

    file.close();
    return true;
}
