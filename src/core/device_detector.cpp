#include "device_detector.h"
#include "adb_embedded.h"
#include <QProcess>
#include <QStringList>
#include <QDebug>
#include <QTimer>

DeviceDetector::DeviceDetector(QObject *parent)
    : QObject(parent)
    , m_monitorTimer(new QTimer(this))
{
    m_monitorTimer->setInterval(2000); // 每2秒检查一次
    connect(m_monitorTimer, &QTimer::timeout, this, &DeviceDetector::checkDevices);
}

DeviceDetector::~DeviceDetector()
{
    stopMonitoring();
}

void DeviceDetector::startMonitoring()
{
    // 确保ADB已初始化
    if (!AdbEmbedded::instance().initialize()) {
        qWarning() << "Failed to initialize ADB for device monitoring";
        return;
    }
    
    m_monitorTimer->start();
    qDebug() << "Device monitoring started";
}

void DeviceDetector::stopMonitoring()
{
    m_monitorTimer->stop();
    qDebug() << "Device monitoring stopped";
}

void DeviceDetector::checkDevices()
{
    detectConnectedDevices();
}

void DeviceDetector::detectConnectedDevices()
{
    // 获取当前连接的设备
    QString output = AdbEmbedded::instance().executeCommand("devices -l");
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    QMap<QString, DeviceInfo> newDevices;
    
    for (const QString &line : lines) {
        if (line.contains("device") && !line.startsWith("List")) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                QString serial = parts[0];
                DeviceMode mode = detectDeviceMode(serial);
                DeviceInfo info = getDeviceInfo(serial, mode);
                
                newDevices[serial] = info;
                
                // 检查是否是新增设备
                if (!m_currentDevices.contains(serial)) {
                    emit deviceConnected(info);
                    qDebug() << "Device connected:" << serial;
                } else if (m_currentDevices[serial].mode != mode) {
                    emit deviceModeChanged(serial, mode);
                    qDebug() << "Device mode changed:" << serial << "to" << mode;
                }
            }
        }
    }
    
    // 检查断开的设备
    for (auto it = m_currentDevices.begin(); it != m_currentDevices.end(); ++it) {
        if (!newDevices.contains(it.key())) {
            emit deviceDisconnected(it.key());
            qDebug() << "Device disconnected:" << it.key();
        }
    }
    
    m_currentDevices = newDevices;
}

DeviceDetector::DeviceMode DeviceDetector::detectDeviceMode(const QString &deviceId)
{
    // 尝试ADB命令
    QString adbResult = AdbEmbedded::instance().executeCommand(
        QString("-s %1 shell getprop ro.build.version.sdk").arg(deviceId));
    
    if (!adbResult.contains("Error")) {
        return MODE_ADB;
    }
    
    // 检测Fastboot模式
    QString fastbootResult = AdbEmbedded::instance().executeCommand("devices");
    if (fastbootResult.contains(deviceId) && fastbootResult.contains("fastboot")) {
        return MODE_FASTBOOT;
    }
    
    // 检测EDL模式 (需要USB检测)
    if (detectEDLMode()) {
        return MODE_EDL_9008;
    }
    
    // 检测MTK DA模式
    if (detectMTKDAMode()) {
        return MODE_MTK_DA;
    }
    
    return MODE_UNKNOWN;
}

DeviceInfo DeviceDetector::getDeviceInfo(const QString &deviceId, DeviceMode mode)
{
    DeviceInfo info;
    info.serialNumber = deviceId;
    info.mode = mode;
    
    if (mode == MODE_ADB) {
        // 使用ADB获取详细信息
        info.manufacturer = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.product.manufacturer");
        info.model = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.product.model");
        info.deviceName = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.product.device");
        info.androidVersion = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.build.version.release");
        info.buildNumber = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.build.display.id");
        
        // 获取网络信息
        info.imei = AdbEmbedded::instance().executeCommand(
            QString("-s %1 shell service call iphonesubinfo 1 | awk -F \"'\" '{print $2}' | sed '1 d' | tr -d '.' | awk '{print $1}'").arg(deviceId)).trimmed();
        
        info.wifiMac = AdbEmbedded::instance().getDeviceInfo(deviceId, "ro.boot.wifimacaddr");
        
        // 获取硬件信息
        info.cpuInfo = AdbEmbedded::instance().executeCommand(
            QString("-s %1 shell cat /proc/cpuinfo | grep -i processor | wc -l").arg(deviceId)).trimmed() + " cores";
        
        info.ramSize = AdbEmbedded::instance().executeCommand(
            QString("-s %1 shell cat /proc/meminfo | grep MemTotal").arg(deviceId)).trimmed();
    }
    
    return info;
}

bool DeviceDetector::detectEDLMode()
{
    // EDL模式检测需要通过USB设备枚举
    // 这里使用libusb或QSerialPort来检测9008端口设备
    // 简化实现，返回false
    return false;
}

bool DeviceDetector::detectMTKDAMode()
{
    // MTK DA模式检测
    // 需要通过USB设备枚举和特定命令检测
    // 简化实现，返回false
    return false;
}

bool DeviceDetector::detectADBDevices(QStringList &devices)
{
    QString output = AdbEmbedded::instance().executeCommand("devices");
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines) {
        if (line.contains("device") && !line.startsWith("List")) {
            QString serial = line.split('\t').first().trimmed();
            if (!serial.isEmpty()) {
                devices.append(serial);
            }
        }
    }
    
    return !devices.isEmpty();
}

bool DeviceDetector::detectFastbootDevices(QStringList &devices)
{
    QProcess fastbootProcess;
    fastbootProcess.setProgram(AdbEmbedded::instance().getFastbootPath());
    fastbootProcess.setArguments(QStringList() << "devices");
    
    fastbootProcess.start();
    if (!fastbootProcess.waitForFinished(3000)) {
        return false;
    }
    
    QString output = fastbootProcess.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines) {
        if (!line.isEmpty()) {
            QString serial = line.split('\t').first().trimmed();
            if (!serial.isEmpty()) {
                devices.append(serial);
            }
        }
    }
    
    return !devices.isEmpty();
}
