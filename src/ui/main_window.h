#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "core/device_detector.h"
#include "ui/tool_panel.h"
#include "ui/device_info_panel.h"
#include "ui/output_panel.h"

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
    void onDeviceSelectionChanged(const QString &deviceId);
    void onOutputMessage(const QString &message, bool isError = false);
    void onRefreshRequested();

private:
    void setupUI();
    void setupConnections();
    
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    ToolPanel *m_toolPanel;
    DeviceInfoPanel *m_deviceInfoPanel;
    OutputPanel *m_outputPanel;
    
    DeviceDetector m_deviceDetector;
    QMap<QString, DeviceInfo> m_currentDevices;
};

#endif // MAIN_WINDOW_H
