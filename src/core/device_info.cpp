#include "device_info.h"
#include <QString>

DeviceInfo::DeviceInfo()
    : isRooted(false)
    , mode(DeviceDetector::MODE_UNKNOWN)
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
    
    return map;
}

QString DeviceInfo::toString() const
{
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
    ).arg(serialNumber, manufacturer, model, deviceName, 
          androidVersion, buildNumber, imei).arg(static_cast<int>(mode));
}
