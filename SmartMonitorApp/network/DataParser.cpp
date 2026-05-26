/**
 * @file    DataParser.cpp
 * @brief   协议数据解析器实现
 *
 * 关键实现细节：
 *     parse()：逐字节扫描原始数据，找到帧头帧尾后提取23字节，
 *              用qFromBigEndian按大端序还原各字段。
 *     buildTestData()：反向操作，用qToBigEndian按大端序组装一帧数据。
 */

#include "DataParser.h"
#include <QtEndian>

const char DataParser::FRAME_HEADER[2] = {static_cast<char>(0xAA), static_cast<char>(0x55)};
const char DataParser::FRAME_TAIL[2]   = {static_cast<char>(0x55), static_cast<char>(0xAA)};

QList<DeviceInfo> DataParser::parse(const QByteArray& rawData)
{
    QList<DeviceInfo> result;
    int pos = 0;

    // 逐字节扫描，查找完整帧
    while (pos + FRAME_SIZE <= rawData.size()) {
        // 匹配帧头
        if (rawData.at(pos) != FRAME_HEADER[0] || rawData.at(pos + 1) != FRAME_HEADER[1]) {
            pos++;
            continue;
        }

        // 匹配帧尾
        int tailPos = pos + FRAME_SIZE - 2;
        if (rawData.at(tailPos) != FRAME_TAIL[0] || rawData.at(tailPos + 1) != FRAME_TAIL[1]) {
            pos++;
            continue;
        }

        // 找到完整帧，解析各字段
        DeviceInfo info;
        const char* frame = rawData.constData() + pos;

        // 设备ID：偏移2，2字节，大端序
        info.id = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(frame + 2));

        // 温度：偏移4，4字节float
        float temp;
        qFromBigEndian<float>(reinterpret_cast<const uchar*>(frame + 4), 1, &temp);
        info.temperature = static_cast<double>(temp);

        // 压力：偏移8，4字节float
        float pressure;
        qFromBigEndian<float>(reinterpret_cast<const uchar*>(frame + 8), 1, &pressure);
        info.pressure = static_cast<double>(pressure);

        // 转速：偏移12，4字节float
        float speed;
        qFromBigEndian<float>(reinterpret_cast<const uchar*>(frame + 12), 1, &speed);
        info.speed = static_cast<double>(speed);

        // 电压：偏移16，4字节float
        float voltage;
        qFromBigEndian<float>(reinterpret_cast<const uchar*>(frame + 16), 1, &voltage);
        info.voltage = static_cast<double>(voltage);

        // 运行状态：偏移20，1字节
        int statusVal = static_cast<uchar>(frame[20]);
        info.status = (statusVal <= 3)
            ? static_cast<DeviceStatus>(statusVal)
            : DeviceStatus::Offline;

        info.name = QString("设备%1").arg(info.id);
        info.updateTime = QDateTime::currentDateTime();

        result.append(info);
        pos += FRAME_SIZE;
    }

    return result;
}

QByteArray DataParser::buildTestData(int deviceId, const QString& name,
                                      float temp, float pressure,
                                      float speed, float voltage,
                                      int status)
{
    Q_UNUSED(name)

    QByteArray frame(FRAME_SIZE, 0);

    // 帧头
    frame[0] = FRAME_HEADER[0];
    frame[1] = FRAME_HEADER[1];

    // 设备ID（大端序）
    qToBigEndian(static_cast<quint16>(deviceId), reinterpret_cast<uchar*>(frame.data() + 2));

    // 温度、压力、转速、电压（大端序float）
    qToBigEndian(temp,     reinterpret_cast<uchar*>(frame.data() + 4));
    qToBigEndian(pressure, reinterpret_cast<uchar*>(frame.data() + 8));
    qToBigEndian(speed,    reinterpret_cast<uchar*>(frame.data() + 12));
    qToBigEndian(voltage,  reinterpret_cast<uchar*>(frame.data() + 16));

    // 运行状态
    frame[20] = static_cast<char>(status);

    // 帧尾
    frame[21] = FRAME_TAIL[0];
    frame[22] = FRAME_TAIL[1];

    return frame;
}
