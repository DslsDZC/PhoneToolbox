#include "device_info.h"
#include <QString>

DeviceInfo::DeviceInfo()
    : isRooted(false),
      mode(MODE_UNKNOWN)
{
}

QMap<QString, QVariant> DeviceInfo::toMap() const
{
    QMap<QString, QVariant> map;
    
    map["serialNumber"] = serialNumber;
    map["manufacturer"] = manufacturer;
    map["model"] = model;
    map["deviceName"] = deviceName;
    map["androidVersion"] = androidVersion;
    map["buildNumber"] = buildNumber;
    
    map["imei"] = imei;
    map["meid"] = meid;
    map["simState"] = simState;
    map["networkType"] = networkType;
    map["wifiMac"] = wifiMac;
    map["bluetoothMac"] = bluetoothMac;
    
    map["cpuInfo"] = cpuInfo;
    map["ramSize"] = ramSize;
    map["storageSize"] = storageSize;
    map["screenResolution"] = screenResolution;
    map["batteryHealth"] = batteryHealth;
    
    map["bootloaderVersion"] = bootloaderVersion;
    map["basebandVersion"] = basebandVersion;
    map["isRooted"] = isRooted;
    map["selinuxStatus"] = selinuxStatus;
    map["mode"] = static_cast<int>(mode);
    
    return map;
}

QString DeviceInfo::toString() const
{
    QString modeString;
    switch(mode) {
        case MODE_NORMAL: modeString = "Normal"; break;
        case MODE_RECOVERY: modeString = "Recovery"; break;
        case MODE_BOOTLOADER: modeString = "Bootloader"; break;
        case MODE_FASTBOOT: modeString = "Fastboot"; break;
        case MODE_UNKNOWN:
        default: modeString = "Unknown"; break;
    }
    
    return QString(
        "Device Info:\n"
        "  Serial: %1\n"
        "  Manufacturer: %2\n"
        "  Model: %3\n"
        "  Device: %4\n"
        "  Android: %5\n"
        "  Build: %6\n"
        "  IMEI: %7\n"
        "  Mode: %8"
    )
    .arg(serialNumber)
    .arg(manufacturer)
    .arg(model)
    .arg(deviceName)
    .arg(androidVersion)
    .arg(buildNumber)
    .arg(imei)
    .arg(modeString);
}
