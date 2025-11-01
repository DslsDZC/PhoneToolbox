#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentSelectedDevice("")
{
    setupUI();
    setupConnections();
    
    // 初始化ADB和设备检测
    if (AdbEmbedded::instance().initialize()) {
        m_statusLabel->setText("状态: ADB 初始化成功");
        m_deviceDetector.startMonitoring();
    } else {
        m_statusLabel->setText("状态: ADB 初始化失败");
        appendOutput("❌ 无法初始化嵌入式ADB工具", true);
    }
    
    appendOutput("🚀 Phone Toolbox 已启动");
}

MainWindow::~MainWindow()
{
    m_deviceDetector.stopMonitoring();
}

void MainWindow::setupUI()
{
    setWindowTitle("Phone Toolbox - 跨平台手机工具箱");
    setMinimumSize(1000, 700);
    
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // 创建分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 左侧设备列表和控制面板
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    // 设备列表
    QGroupBox *deviceGroup = new QGroupBox("设备列表", this);
    QVBoxLayout *deviceLayout = new QVBoxLayout(deviceGroup);
    m_deviceList = new QListWidget(this);
    deviceLayout->addWidget(m_deviceList);
    
    // 设备控制
    QGroupBox *controlGroup = new QGroupBox("设备控制", this);
    QFormLayout *controlLayout = new QFormLayout(controlGroup);
    
    m_restartModeCombo = new QComboBox(this);
    m_restartModeCombo->addItem("正常模式 (System)", "system");
    m_restartModeCombo->addItem("恢复模式 (Recovery)", "recovery");
    m_restartModeCombo->addItem("引导程序 (Bootloader)", "bootloader");
    m_restartModeCombo->addItem("Fastboot模式", "fastboot");
    m_restartModeCombo->addItem("EDL模式", "edl");
    m_restartModeCombo->addItem("关机", "shutdown");
    
    m_restartButton = new QPushButton("重启设备", this);
    m_restartButton->setEnabled(false);
    
    controlLayout->addRow("目标模式:", m_restartModeCombo);
    controlLayout->addRow(m_restartButton);
    
    // 工具按钮
    QHBoxLayout *toolLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("刷新", this);
    m_clearButton = new QPushButton("清空输出", this);
    
    toolLayout->addWidget(m_refreshButton);
    toolLayout->addWidget(m_clearButton);
    
    // 状态标签
    m_statusLabel = new QLabel("状态: 正在初始化...", this);
    
    // 组装左侧面板
    leftLayout->addWidget(deviceGroup);
    leftLayout->addWidget(controlGroup);
    leftLayout->addLayout(toolLayout);
    leftLayout->addWidget(m_statusLabel);
    leftLayout->addStretch();
    
    // 右侧输出区域
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QLabel *outputLabel = new QLabel("命令输出", this);
    m_outputText = new QTextEdit(this);
    m_outputText->setReadOnly(true);
    m_outputText->setFont(QFont("Monospace", 9));
    
    rightLayout->addWidget(outputLabel);
    rightLayout->addWidget(m_outputText);
    
    // 添加到分割器
    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(rightPanel);
    
    // 设置分割比例
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 3);
    
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
    
    // 按钮信号
    connect(m_restartButton, &QPushButton::clicked,
            this, &MainWindow::onRestartButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked,
            this, &MainWindow::onRefreshDevicesClicked);
    connect(m_clearButton, &QPushButton::clicked,
            this, &MainWindow::onClearOutputClicked);
    
    // 设备列表选择
    connect(m_deviceList, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onDeviceListSelectionChanged);
}

void MainWindow::onDeviceConnected(const DeviceInfo &info)
{
    m_currentDevices[info.serialNumber] = info;
    updateDeviceList();
    
    QString modeStr;
    switch (info.mode) {
    case DeviceDetector::MODE_ADB: modeStr = "ADB"; break;
    case DeviceDetector::MODE_FASTBOOT: modeStr = "Fastboot"; break;
    case DeviceDetector::MODE_FASTBOOTD: modeStr = "Fastbootd"; break;
    case DeviceDetector::MODE_EDL_9008: modeStr = "EDL 9008"; break;
    case DeviceDetector::MODE_MTK_DA: modeStr = "MTK DA"; break;
    default: modeStr = "未知"; break;
    }
    
    appendOutput(QString("🔗 设备已连接: %1 (%2) - %3")
                .arg(info.serialNumber)
                .arg(info.model)
                .arg(modeStr));
}

void MainWindow::onDeviceDisconnected(const QString &serial)
{
    if (m_currentDevices.contains(serial)) {
        appendOutput(QString("❌ 设备已断开: %1").arg(serial));
        m_currentDevices.remove(serial);
        updateDeviceList();
    }
}

void MainWindow::onDeviceModeChanged(const QString &serial, DeviceDetector::DeviceMode newMode)
{
    if (m_currentDevices.contains(serial)) {
        m_currentDevices[serial].mode = newMode;
        updateDeviceList();
        
        QString modeStr;
        switch (newMode) {
        case DeviceDetector::MODE_ADB: modeStr = "ADB"; break;
        case DeviceDetector::MODE_FASTBOOT: modeStr = "Fastboot"; break;
        case DeviceDetector::MODE_FASTBOOTD: modeStr = "Fastbootd"; break;
        default: modeStr = "未知"; break;
        }
        
        appendOutput(QString("🔄 设备模式改变: %1 -> %2").arg(serial).arg(modeStr));
    }
}

void MainWindow::updateDeviceList()
{
    m_deviceList->clear();
    
    for (const DeviceInfo &info : m_currentDevices) {
        QString displayText = QString("%1\n%2").arg(info.serialNumber).arg(info.model);
        
        // 添加模式信息
        QString modeInfo;
        switch (info.mode) {
        case DeviceDetector::MODE_ADB: modeInfo = " [ADB]"; break;
        case DeviceDetector::MODE_FASTBOOT: modeInfo = " [Fastboot]"; break;
        case DeviceDetector::MODE_FASTBOOTD: modeInfo = " [Fastbootd]"; break;
        case DeviceDetector::MODE_EDL_9008: modeInfo = " [EDL]"; break;
        case DeviceDetector::MODE_MTK_DA: modeInfo = " [MTK DA]"; break;
        default: modeInfo = " [未知]"; break;
        }
        
        displayText += modeInfo;
        
        QListWidgetItem *item = new QListWidgetItem(displayText, m_deviceList);
        item->setData(Qt::UserRole, info.serialNumber);
    }
    
    // 如果没有设备，显示提示
    if (m_currentDevices.isEmpty()) {
        m_deviceList->addItem("无设备连接");
        m_restartButton->setEnabled(false);
    }
}

void MainWindow::onDeviceListSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = m_deviceList->selectedItems();
    
    if (selectedItems.isEmpty()) {
        m_currentSelectedDevice = "";
        m_restartButton->setEnabled(false);
        return;
    }
    
    QListWidgetItem *item = selectedItems.first();
    m_currentSelectedDevice = item->data(Qt::UserRole).toString();
    
    // 确保选择的是真实设备（不是提示文本）
    if (m_currentDevices.contains(m_currentSelectedDevice)) {
        m_restartButton->setEnabled(true);
    } else {
        m_restartButton->setEnabled(false);
    }
}

void MainWindow::onRestartButtonClicked()
{
    if (m_currentSelectedDevice.isEmpty() || !m_currentDevices.contains(m_currentSelectedDevice)) {
        QMessageBox::warning(this, "错误", "请先选择一个设备");
        return;
    }
    
    QString targetMode = m_restartModeCombo->currentData().toString();
    QString deviceId = m_currentSelectedDevice;
    
    appendOutput(QString("🔄 尝试重启设备 %1 到 %2 模式...")
                .arg(deviceId)
                .arg(m_restartModeCombo->currentText()));
    
    restartToMode(deviceId, targetMode);
}

void MainWindow::onRefreshDevicesClicked()
{
    appendOutput("🔄 手动刷新设备列表...");
    // 强制重新检测设备
    m_deviceDetector.startMonitoring();
}

void MainWindow::onClearOutputClicked()
{
    m_outputText->clear();
}

void MainWindow::appendOutput(const QString &message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedMessage = QString("[%1] %2").arg(timestamp).arg(message);
    
    if (isError) {
        m_outputText->setTextColor(QColor(200, 0, 0));
    } else {
        m_outputText->setTextColor(QColor(0, 0, 0));
    }
    
    m_outputText->append(formattedMessage);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = m_outputText->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

bool MainWindow::checkRootPermission(const QString &deviceId)
{
    if (m_currentDevices[deviceId].mode == DeviceDetector::MODE_ADB) {
        // 检查root权限
        QString result = AdbEmbedded::instance().executeCommand(
            QString("-s %1 shell su -c \"echo root\"").arg(deviceId));
        
        if (result.contains("root")) {
            appendOutput("✅ 设备具有Root权限");
            return true;
        } else {
            appendOutput("⚠️ 设备没有Root权限，尝试普通重启");
            return false;
        }
    }
    
    // Fastboot模式不需要root权限
    return true;
}

void MainWindow::restartToMode(const QString &deviceId, const QString &targetMode)
{
    QString command = getCurrentRestartCommand(deviceId, targetMode);
    
    if (command.isEmpty()) {
        appendOutput("❌ 无法生成重启命令", true);
        return;
    }
    
    appendOutput(QString("💻 执行命令: %1").arg(command));
    
    QString result;
    if (m_currentDevices[deviceId].mode == DeviceDetector::MODE_ADB) {
        result = AdbEmbedded::instance().executeCommand(command);
    } else {
        result = m_deviceDetector.executeFastbootCommand(command, deviceId);
    }
    
    appendOutput(QString("📋 命令结果: %1").arg(result));
    
    if (result.contains("Error") || result.contains("error")) {
        appendOutput("❌ 重启命令执行失败", true);
    } else {
        appendOutput("✅ 重启命令已发送");
    }
}

QString MainWindow::getCurrentRestartCommand(const QString &deviceId, const QString &targetMode)
{
    DeviceDetector::DeviceMode currentMode = static_cast<DeviceDetector::DeviceMode>(m_currentDevices[deviceId].mode);
    bool hasRoot = checkRootPermission(deviceId);
    
    QString command;
    
    if (currentMode == DeviceDetector::MODE_ADB) {
        // ADB模式下的重启命令
        if (targetMode == "system") {
            command = QString("-s %1 reboot").arg(deviceId);
        } else if (targetMode == "recovery") {
            if (hasRoot) {
                command = QString("-s %1 shell su -c \"reboot recovery\"").arg(deviceId);
            } else {
                command = QString("-s %1 reboot recovery").arg(deviceId);
            }
        } else if (targetMode == "bootloader" || targetMode == "fastboot") {
            if (hasRoot) {
                command = QString("-s %1 shell su -c \"reboot bootloader\"").arg(deviceId);
            } else {
                command = QString("-s %1 reboot bootloader").arg(deviceId);
            }
        } else if (targetMode == "edl") {
            if (hasRoot) {
                command = QString("-s %1 shell su -c \"reboot edl\"").arg(deviceId);
            } else {
                command = QString("-s %1 reboot edl").arg(deviceId);
            }
        } else if (targetMode == "shutdown") {
            if (hasRoot) {
                command = QString("-s %1 shell su -c \"reboot -p\"").arg(deviceId);
            } else {
                command = QString("-s %1 shell reboot -p").arg(deviceId);
            }
        }
    } else if (currentMode == DeviceDetector::MODE_FASTBOOT || 
               currentMode == DeviceDetector::MODE_FASTBOOTD) {
        // Fastboot模式下的重启命令
        if (targetMode == "system") {
            command = "reboot";
        } else if (targetMode == "recovery") {
            command = "reboot-recovery";
        } else if (targetMode == "bootloader") {
            command = "reboot-bootloader";
        } else if (targetMode == "fastboot") {
            command = "reboot-fastboot";
        } else if (targetMode == "shutdown") {
            command = "shutdown";
        }
        // Fastboot模式通常不支持直接重启到EDL
    }
    
    return command;
}
