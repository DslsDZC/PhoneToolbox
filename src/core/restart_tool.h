#ifndef RESTART_TOOL_H
#define RESTART_TOOL_H

#include <QObject>
#include <QString>
#include "device_detector.h"

class RestartTool : public QObject
{
    Q_OBJECT

public:
    explicit RestartTool(QObject *parent = nullptr);
    
    enum RestartMode {
        MODE_SYSTEM = 0,
        MODE_RECOVERY = 1,
        MODE_BOOTLOADER = 2,
        MODE_FASTBOOT = 3,
        MODE_EDL = 4,
        MODE_SHUTDOWN = 5
    };

    QString restartDevice(const QString &deviceId, DeviceDetector::DeviceMode currentMode, RestartMode targetMode);
    bool checkRootPermission(const QString &deviceId);
    QString getModeName(RestartMode mode) const;

signals:
    void outputMessage(const QString &message, bool isError = false);

private:
    QString getRestartCommand(const QString &deviceId, DeviceDetector::DeviceMode currentMode, RestartMode targetMode, bool hasRoot);
};

#endif // RESTART_TOOL_H
