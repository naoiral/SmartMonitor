/**
 * @file    DataParser.h
 * @brief   协议数据解析器
 *
 * 模块作用：
 *     将TCP收到的原始字节流按照自定义协议解析成DeviceInfo结构体。
 *     也可以反向构造测试数据帧（buildTestData），用于模拟设备发送。
 *
 * 设计思路：
 *     采用自定义二进制协议，比文本协议更紧凑，解析效率更高。
 *     协议格式（23字节一帧）：
 *     ┌────────┬────────┬────────┬────────┬────────┬────────┬──────┬────────┐
 *     │ 帧头   │ 设备ID │ 温度   │ 压力   │ 转速   │ 电压   │ 状态 │ 帧尾   │
 *     │ 2字节  │ 2字节  │ 4字节  │ 4字节  │ 4字节  │ 4字节  │ 1字节│ 2字节  │
 *     │0xAA55  │ uint16 │ float  │ float  │ float  │ float  │ uint8│0x55AA  │
 *     └────────┴────────┴────────┴────────┴────────┴────────┴──────┴────────┘
 *     所有多字节字段采用大端序（网络字节序）。
 *
 * 面试考点：
 *     1. 什么是大端序和小端序？
 *        - 大端序：高位字节在低地址（网络协议通用）
 *        - 小端序：低位字节在低地址（x86 CPU内部使用）
 *     2. 为什么网络传输用大端序？
 *        - 统一标准，避免不同CPU架构之间解析出错
 *     3. 如何处理TCP粘包/拆包？
 *        - 通过帧头帧尾标记定界，逐字节扫描找到完整帧
 */

#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QByteArray>
#include <QList>
#include "../model/DeviceInfo.h"

class DataParser
{
public:
    static QList<DeviceInfo> parse(const QByteArray& rawData);

    static QByteArray buildTestData(int deviceId, const QString& name,
                                     float temp, float pressure,
                                     float speed, float voltage,
                                     int status);

private:
    static const char FRAME_HEADER[2];  // 帧头标识 0xAA 0x55
    static const char FRAME_TAIL[2];    // 帧尾标识 0x55 0xAA
    static const int FRAME_SIZE = 23;   // 单帧字节数
};

#endif // DATAPARSER_H
