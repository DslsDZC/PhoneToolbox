#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QStackedWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "core/device_detector.h"
#include "core/adb_embedded.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDeviceConnected(const DeviceInfo &info);
    void onDeviceDisconnected(const QString &serial);
    void onDeviceModeChanged(const QString &serial, DeviceDetector::DeviceMode newMode);
    
    void onRestartButtonClicked();
    void onRefreshDevicesClicked();
    void onClearOutputClicked();
    
    void onDeviceListSelectionChanged();

private:
    void setupUI();
    void setupConnections();
    void updateDeviceList();
    void appendOutput(const QString &message, bool isError = false);
    
    // 重启相关功能
    bool checkRootPermission(const QString &deviceId);
    void restartToMode(const QString &deviceId, const QString &targetMode);
    QString getCurrentRestartCommand(const QString &deviceId, const QString &targetMode);

    QSplitter *m_mainSplitter;
    QListWidget *m_deviceList;
    QStackedWidget *m_contentStack;
    QTextEdit *m_outputText;
    
    // 设备控制区域
    QWidget *m_deviceControlWidget;
    QComboBox *m_restartModeCombo;
    QPushButton *m_restartButton;
    QPushButton *m_refreshButton;
    QPushButton *m_clearButton;
    QLabel *m_statusLabel;
    
    DeviceDetector m_deviceDetector;
    QMap<QString, DeviceInfo> m_currentDevices;
    QString m_currentSelectedDevice;
};

#endif // MAIN_WINDOW_H
