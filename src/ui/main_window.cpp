#include "main_window.h"
#include "core/adb_embedded.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mainSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_toolPanel(nullptr)
    , m_deviceInfoPanel(nullptr)
    , m_outputPanel(nullptr)
{
    setupUI();
    setupConnections();
    
    // 初始化ADB和设备检测
    if (AdbEmbedded::instance().initialize()) {
        m_deviceDetector.startMonitoring();
        m_outputPanel->appendOutput("✅ ADB 初始化成功");
    } else {
        m_outputPanel->appendOutput("❌ 无法初始化嵌入式ADB工具", true);
    }
    
    m_outputPanel->appendOutput("🚀 Phone Toolbox 已启动");
}

MainWindow::~MainWindow()
{
    m_deviceDetector.stopMonitoring();
}

void MainWindow::setupUI()
{
    setWindowTitle("Phone Toolbox - 跨平台手机工具箱");
    setMinimumSize(1200, 800);
    
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // 创建主分割器（左右分割）
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 创建右侧分割器（上下分割）
    m_rightSplitter = new QSplitter(Qt::Vertical, this);
    
    // 创建各个面板
    m_toolPanel = new ToolPanel(this);
    m_deviceInfoPanel = new DeviceInfoPanel(this);
    m_outputPanel = new OutputPanel(this);
    
    // 将右侧面板添加到右侧分割器
    m_rightSplitter->addWidget(m_deviceInfoPanel);
    m_rightSplitter->addWidget(m_outputPanel);
    
    // 设置右侧分割器的比例（设备信息:输出 = 1:2）
    m_rightSplitter->setStretchFactor(0, 1);
    m_rightSplitter->setStretchFactor(1, 2);
    
    // 将左侧和右侧添加到主分割器
    m_mainSplitter->addWidget(m_toolPanel);
    m_mainSplitter->addWidget(m_rightSplitter);
    
    // 设置主分割器的比例（工具:右侧 = 1:2）
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 2);
    
    mainLayout->addWidget(m_mainSplitter);
}

void MainWindow::setupConnections()
{
    // 设备检测信号
    connect(&m_deviceDetector, &DeviceDetector::deviceConnected,
            this, &MainWindow::onDeviceConnected);
    connect(&m_deviceDetector, &DeviceDetector::deviceDisconnected,
            this, &MainWindow::onDeviceDisconnected);
    connect(&m_deviceDetector, &DeviceDetector::deviceModeChanged,
            this, &MainWindow::onDeviceModeChanged);
    
    // 工具面板信号
    connect(m_toolPanel, &ToolPanel::deviceSelectionChanged,
            this, &MainWindow::onDeviceSelectionChanged);
    connect(m_toolPanel, &ToolPanel::outputMessage,
            this, &MainWindow::onOutputMessage);
    connect(m_toolPanel, &ToolPanel::refreshRequested,
            this, &MainWindow::onRefreshRequested);
}

void MainWindow::onDeviceConnected(const DeviceInfo &info)
{
    m_currentDevices[info.serialNumber] = info;
    m_toolPanel->updateDeviceList(m_currentDevices);
    
    QString modeStr;
    switch (info.mode) {
    case DeviceDetector::MODE_ADB: modeStr = "ADB"; break;
    case DeviceDetector::MODE_FASTBOOT: modeStr = "Fastboot"; break;
    case DeviceDetector::MODE_FASTBOOTD: modeStr = "Fastbootd"; break;
    case DeviceDetector::MODE_EDL_9008: modeStr = "EDL 9008"; break;
    case DeviceDetector::MODE_MTK_DA: modeStr = "MTK DA"; break;
    default: modeStr = "未知"; break;
    }
    
    m_outputPanel->appendOutput(QString("🔗 设备已连接: %1 (%2) - %3")
                               .arg(info.serialNumber)
                               .arg(info.model)
                               .arg(modeStr));
}

void MainWindow::onDeviceDisconnected(const QString &serial)
{
    if (m_currentDevices.contains(serial)) {
        m_outputPanel->appendOutput(QString("❌ 设备已断开: %1").arg(serial));
        m_currentDevices.remove(serial);
        m_toolPanel->updateDeviceList(m_currentDevices);
    }
}

void MainWindow::onDeviceModeChanged(const QString &serial, DeviceDetector::DeviceMode newMode)
{
    if (m_currentDevices.contains(serial)) {
        m_currentDevices[serial].mode = newMode;
        m_toolPanel->updateDeviceList(m_currentDevices);
        
        QString modeStr;
        switch (newMode) {
        case DeviceDetector::MODE_ADB: modeStr = "ADB"; break;
        case DeviceDetector::MODE_FASTBOOT: modeStr = "Fastboot"; break;
        case DeviceDetector::MODE_FASTBOOTD: modeStr = "Fastbootd"; break;
        default: modeStr = "未知"; break;
        }
        
        m_outputPanel->appendOutput(QString("🔄 设备模式改变: %1 -> %2").arg(serial).arg(modeStr));
        
        // 如果当前选中的设备模式改变，更新设备信息面板
        if (m_toolPanel->getSelectedDevice() == serial) {
            m_deviceInfoPanel->updateDeviceInfo(m_currentDevices[serial]);
        }
    }
}

void MainWindow::onDeviceSelectionChanged(const QString &deviceId)
{
    if (deviceId.isEmpty() || !m_currentDevices.contains(deviceId)) {
        // 清空设备信息面板
        m_deviceInfoPanel->clearDeviceInfo();
    } else {
        // 更新设备信息面板
        m_deviceInfoPanel->updateDeviceInfo(m_currentDevices[deviceId]);
    }
}

void MainWindow::onOutputMessage(const QString &message, bool isError)
{
    m_outputPanel->appendOutput(message, isError);
}

void MainWindow::onRefreshRequested()
{
    m_outputPanel->appendOutput("🔄 手动刷新设备列表...");
    m_deviceDetector.startMonitoring();
}
