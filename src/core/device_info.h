#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <QString>
#include <QMap>
#include <QVariant>

class DeviceInfo
{
public:
    // 定义设备模式枚举
    enum DeviceMode {
        MODE_UNKNOWN = 0,
        MODE_NORMAL,
        MODE_RECOVERY,
        MODE_BOOTLOADER,
        MODE_FASTBOOT
    };
    
    DeviceInfo();
    
    // 基本信息
    QString serialNumber;
    QString manufacturer;
    QString model;
    QString deviceName;
    QString androidVersion;
    QString buildNumber;
    
    // 网络信息
    QString imei;
    QString meid;
    QString simState;
    QString networkType;
    QString wifiMac;
    QString bluetoothMac;
    
    // 硬件信息
    QString cpuInfo;
    QString ramSize;
    QString storageSize;
    QString screenResolution;
    QString batteryHealth;
    
    // 系统信息
    QString bootloaderVersion;
    QString basebandVersion;
    bool isRooted;
    QString selinuxStatus;
    
    DeviceMode mode;  // 使用正确的枚举类型
    
    QMap<QString, QVariant> toMap() const;
    QString toString() const;
};

#endif // DEVICE_INFO_H
