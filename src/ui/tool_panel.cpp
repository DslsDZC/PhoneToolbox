#include "tool_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>

ToolPanel::ToolPanel(QWidget *parent)
    : QWidget(parent)
    , m_deviceList(nullptr)
    , m_restartModeCombo(nullptr)
    , m_restartButton(nullptr)
    , m_refreshButton(nullptr)
    , m_restartTool(new RestartTool(this))
    , m_currentSelectedDevice("")
{
    setupUI();
    setupConnections();
}

void ToolPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // 设备列表
    QGroupBox *deviceGroup = new QGroupBox("设备列表", this);
    QVBoxLayout *deviceLayout = new QVBoxLayout(deviceGroup);
    m_deviceList = new QListWidget(this);
    m_deviceList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 设置设备列表样式
    m_deviceList->setStyleSheet("QListWidget { "
                               "background-color: #f8f8f8; "
                               "border: 1px solid #e0e0e0; "
                               "border-radius: 3px; "
                               "}"
                               "QListWidget::item:selected { "
                               "background-color: #e0e0e0; "
                               "}");
    
    deviceLayout->addWidget(m_deviceList);
    
    // 重启工具
    QGroupBox *restartGroup = new QGroupBox("重启工具", this);
    QFormLayout *restartLayout = new QFormLayout(restartGroup);
    
    m_restartModeCombo = new QComboBox(this);
    m_restartModeCombo->addItem("正常模式 (System)", RestartTool::MODE_SYSTEM);
    m_restartModeCombo->addItem("恢复模式 (Recovery)", RestartTool::MODE_RECOVERY);
    m_restartModeCombo->addItem("引导程序 (Fastboot)", RestartTool::MODE_BOOTLOADER);
    m_restartModeCombo->addItem("Fastbootd 模式", RestartTool::MODE_FASTBOOT);
    m_restartModeCombo->addItem("EDL 模式", RestartTool::MODE_EDL);
    m_restartModeCombo->addItem("关机", RestartTool::MODE_SHUTDOWN);
    
    m_restartButton = new QPushButton("重启设备", this);
    m_restartButton->setEnabled(false);
    
    restartLayout->addRow("目标模式:", m_restartModeCombo);
    restartLayout->addRow(m_restartButton);
    
    // 添加模式说明
    QLabel *modeHelp = new QLabel(this);
    modeHelp->setText("💡 Fastbootd: Android 10+ 用户空间Fastboot\n💡 Fastboot: 传统引导程序模式");
    modeHelp->setWordWrap(true);
    modeHelp->setStyleSheet("color: #666; font-size: 10px;");
    restartLayout->addRow(modeHelp);
    
    // 工具按钮
    QHBoxLayout *toolLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("刷新设备", this);
    
    toolLayout->addWidget(m_refreshButton);
    toolLayout->addStretch();
    
    // 组装面板
    mainLayout->addWidget(deviceGroup);
    mainLayout->addWidget(restartGroup);
    mainLayout->addLayout(toolLayout);
    mainLayout->addStretch();
}

void ToolPanel::setupConnections()
{
    connect(m_deviceList, &QListWidget::itemSelectionChanged,
            this, &ToolPanel::onDeviceListSelectionChanged);
    connect(m_restartButton, &QPushButton::clicked,
            this, &ToolPanel::onRestartButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked,
            this, &ToolPanel::onRefreshButtonClicked);
    connect(m_restartTool, &RestartTool::outputMessage,
            this, &ToolPanel::onRestartToolOutput);
}

void ToolPanel::updateDeviceList(const QMap<QString, DeviceInfo> &devices)
{
    m_currentDevices = devices;
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
        QListWidgetItem *item = new QListWidgetItem("无设备连接", m_deviceList);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        m_restartButton->setEnabled(false);
    }
}

QString ToolPanel::getSelectedDevice() const
{
    return m_currentSelectedDevice;
}

void ToolPanel::onDeviceListSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = m_deviceList->selectedItems();
    
    if (selectedItems.isEmpty()) {
        m_currentSelectedDevice = "";
        m_restartButton->setEnabled(false);
        emit deviceSelectionChanged("");
        return;
    }
    
    QListWidgetItem *item = selectedItems.first();
    
    // 检查是否是"无设备连接"提示项
    if (item->text() == "无设备连接") {
        m_currentSelectedDevice = "";
        m_restartButton->setEnabled(false);
        emit deviceSelectionChanged("");
        return;
    }
    
    m_currentSelectedDevice = item->data(Qt::UserRole).toString();
    
    // 确保选择的是真实设备
    if (m_currentDevices.contains(m_currentSelectedDevice)) {
        m_restartButton->setEnabled(true);
        emit deviceSelectionChanged(m_currentSelectedDevice);
    } else {
        m_restartButton->setEnabled(false);
        emit deviceSelectionChanged("");
    }
}

void ToolPanel::onRestartButtonClicked()
{
    if (m_currentSelectedDevice.isEmpty() || !m_currentDevices.contains(m_currentSelectedDevice)) {
        emit outputMessage("❌ 请先选择一个设备", true);
        return;
    }
    
    const DeviceInfo &info = m_currentDevices[m_currentSelectedDevice];
    RestartTool::RestartMode targetMode = static_cast<RestartTool::RestartMode>(
        m_restartModeCombo->currentData().toInt());
    
    // 替换原来的调用
    m_restartTool->restartDevice(
        m_currentSelectedDevice, 
        static_cast<DeviceDetector::DeviceMode>(info.mode),  // 添加显式类型转换
        targetMode
    );
}

void ToolPanel::onRefreshButtonClicked()
{
    emit refreshRequested();
}

void ToolPanel::onRestartToolOutput(const QString &message, bool isError)
{
    emit outputMessage(message, isError);
}
