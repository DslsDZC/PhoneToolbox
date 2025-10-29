#ifndef ADB_EMBEDDED_H
#define ADB_EMBEDDED_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTemporaryDir>

class AdbEmbedded : public QObject
{
    Q_OBJECT

public:
    static AdbEmbedded& instance();
    
    bool initialize();
    QString executeCommand(const QString &command, int timeout = 30000);
    QString getDeviceInfo(const QString &serial, const QString &prop);
    
    QString getAdbPath() const;
    QString getFastbootPath() const;

private:
    AdbEmbedded(QObject *parent = nullptr);
    ~AdbEmbedded();
    
    bool extractEmbeddedTools();
    QString getPlatformBinaryName(const QString &baseName) const;
    
    QTemporaryDir m_tempDir;
    QString m_adbPath;
    QString m_fastbootPath;
    bool m_initialized;
};

#endif // ADB_EMBEDDED_H
