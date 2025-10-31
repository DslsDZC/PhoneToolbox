#include "device_info.h"
#include <QString>

DeviceInfo::DeviceInfo()
    : isBootloaderUnlocked(false)
    , isFastbootdMode(false)
    , isRooted(false)
    , mode(0)
{
}

QMap<QString, QVariant> DeviceInfo::toMap() const
{
    QMap<QString, QVariant> map;
    
    // 基础信息
    map["serialNumber"] = serialNumber;
    map["manufacturer"] = manufacturer;
    map["model"] = model;
    map["deviceName"] = deviceName;
    map["androidVersion"] = androidVersion;
    map["buildNumber"] = buildNumber;
    
    // 网络信息
    map["imei"] = imei;
    map["meid"] = meid;
    map["simState"] = simState;
    map["networkType"] = networkType;
    map["wifiMac"] = wifiMac;
    map["bluetoothMac"] = bluetoothMac;
    
    // 硬件信息
    map["cpuInfo"] = cpuInfo;
    map["ramSize"] = ramSize;
    map["storageSize"] = storageSize;
    map["screenResolution"] = screenResolution;
    map["batteryHealth"] = batteryHealth;
    
    // Fastboot特定信息
    map["productName"] = productName;
    map["variant"] = variant;
    map["hwVersion"] = hwVersion;
    map["bootloaderVersion"] = bootloaderVersion;
    map["basebandVersion"] = basebandVersion;
    map["isBootloaderUnlocked"] = isBootloaderUnlocked;
    map["isFastbootdMode"] = isFastbootdMode;
    
    // 系统信息
    map["isRooted"] = isRooted;
    map["selinuxStatus"] = selinuxStatus;
    map["mode"] = mode;
    
    return map;
}

QString DeviceInfo::toString() const
{
    if (mode == 2 || mode == 3) { // Fastboot 或 Fastbootd 模式
        return toFastbootString();
    }
    
    // ADB模式的原有toString逻辑
    return QString(
        "📱 Device Info:\n"
        "   Serial: %1\n"
        "   Manufacturer: %2\n"
        "   Model: %3\n"
        "   Device: %4\n"
        "   Android: %5\n"
        "   Build: %6\n"
        "   IMEI: %7\n"
        "   Mode: %8"
    ).arg(serialNumber, manufacturer, model, deviceName, 
          androidVersion, buildNumber, imei).arg(mode);
}

QString DeviceInfo::toFastbootString() const
{
    QString modeStr = isFastbootdMode ? "Fastbootd" : "传统Fastboot";
    QString lockStr = isBootloaderUnlocked ? "🔓 已解锁" : "🔒 已锁定";
    
    QString result;
    result += "🚀 Fastboot设备信息:\n";
    result += QString("   序列号: %1\n").arg(serialNumber);
    result += QString("   产品型号: %1\n").arg(productName);
    result += QString("   设备变体: %1\n").arg(variant);
    result += QString("   硬件版本: %1\n").arg(hwVersion);
    result += QString("   Bootloader版本: %1\n").arg(bootloaderVersion);
    result += QString("   BL锁状态: %1\n").arg(lockStr);
    result += QString("   运行模式: %1\n").arg(modeStr);
    result += QString("   电池状态: %1\n").arg(batteryHealth);
    result += QString("   制造商: %1\n").arg(manufacturer);
    
    return result;
}
