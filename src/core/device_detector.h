#ifndef DEVICE_DETECTOR_H
#define DEVICE_DETECTOR_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QString>
#include "device_info.h"

class DeviceDetector : public QObject
{
    Q_OBJECT

public:
    enum DeviceMode {
        MODE_UNKNOWN = 0,
        MODE_ADB = 1,
        MODE_FASTBOOT = 2,
        MODE_FASTBOOTD = 3,
        MODE_EDL_9008 = 4,
        MODE_MTK_DA = 5,
        MODE_RECOVERY = 6
    };

    explicit DeviceDetector(QObject *parent = nullptr);
    ~DeviceDetector();

    void startMonitoring();
    void stopMonitoring();

    // Fastboot相关方法
    bool detectFastbootDevices(QStringList &devices);
    DeviceInfo getFastbootDeviceInfo(const QString &deviceId);
    QString executeFastbootCommand(const QString &command, const QString &deviceId = "");
    QString formatDeviceInfoForDisplay(const DeviceInfo &info) const;
    QString getBootloaderStatusIcon(bool isUnlocked) const;
    QString getModeDisplayName(DeviceMode mode) const;
    void forceRefresh() { checkDevices(); }

signals:
    void deviceConnected(const DeviceInfo &info);
    void deviceDisconnected(const QString &serial);
    void deviceModeChanged(const QString &serial, DeviceMode newMode);

private slots:
    void checkDevices();

private:
    QTimer *m_monitorTimer;
    QMap<QString, DeviceInfo> m_currentDevices;
    
    void detectConnectedDevices();
    DeviceMode detectDeviceMode(const QString &deviceId);
    DeviceInfo getDeviceInfo(const QString &deviceId, DeviceMode mode);
    
    // Fastboot特定检测
    QString getBootloaderStatus(const QString &deviceId);
    bool isFastbootdMode(const QString &deviceId);
    QString getFastbootVar(const QString &varName, const QString &deviceId);
    
    // 特定模式检测
    bool detectEDLMode();
    bool detectMTKDAMode();
    bool detectADBDevices(QStringList &devices);
    
    QString formatValue(const QString &value) const;
};

#endif // DEVICE_DETECTOR_H
