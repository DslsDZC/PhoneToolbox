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
        MODE_EDL_9008 = 3,
        MODE_MTK_DA = 4,
        MODE_RECOVERY = 5
    };

    explicit DeviceDetector(QObject *parent = nullptr);
    ~DeviceDetector();

    void startMonitoring();
    void stopMonitoring();

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
    
    // 特定模式检测
    bool detectEDLMode();
    bool detectMTKDAMode();
    bool detectADBDevices(QStringList &devices);
    bool detectFastbootDevices(QStringList &devices);
};

#endif // DEVICE_DETECTOR_H
