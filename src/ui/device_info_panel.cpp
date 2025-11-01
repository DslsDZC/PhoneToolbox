#include "device_info_panel.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>

// 可选择的QLabel子类，支持右键复制菜单
class SelectableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SelectableLabel(const QString &text = "", QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
        setTextInteractionFlags(Qt::TextSelectableByMouse);
        setCursor(Qt::IBeamCursor);
    }

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        QMenu *menu = new QMenu(this);
        
        QAction *copyAction = new QAction("复制", this);
        connect(copyAction, &QAction::triggered, this, [this]() {
            QApplication::clipboard()->setText(this->text());
        });
        
        QAction *selectAllAction = new QAction("全选", this);
        connect(selectAllAction, &QAction::triggered, this, [this]() {
            this->setSelection(0, this->text().length());
        });
        
        menu->addAction(copyAction);
        menu->addAction(selectAllAction);
        menu->exec(event->globalPos());
        delete menu;
    }
};

DeviceInfoPanel::DeviceInfoPanel(QWidget *parent)
    : QWidget(parent)
    , m_formLayout(nullptr)
{
    setupUI();
}

void DeviceInfoPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    QGroupBox *infoGroup = new QGroupBox("设备详细信息 (所有文本均可选择复制)", this);
    m_formLayout = new QFormLayout(infoGroup);
    
    // 创建设备信息标签 - 使用可选择版本
    m_serialLabel = createSelectableLabel("未连接");
    m_modelLabel = createSelectableLabel("未连接");
    m_manufacturerLabel = createSelectableLabel("未连接");
    m_androidVersionLabel = createSelectableLabel("未连接");
    m_bootloaderLabel = createSelectableLabel("未连接");
    m_rootStatusLabel = createSelectableLabel("未连接");
    m_batteryLabel = createSelectableLabel("未连接");
    
    // 模式信息使用QTextEdit以支持多行选择和复制
    m_modeTextEdit = createReadOnlyTextEdit("未连接");
    m_modeTextEdit->setFixedHeight(50); // 固定高度以显示两行文本
    
    // 添加到表单
    m_formLayout->addRow("序列号:", m_serialLabel);
    m_formLayout->addRow("型号:", m_modelLabel);
    m_formLayout->addRow("制造商:", m_manufacturerLabel);
    m_formLayout->addRow("Android版本:", m_androidVersionLabel);
    m_formLayout->addRow("Bootloader:", m_bootloaderLabel);
    m_formLayout->addRow("当前模式:", m_modeTextEdit);
    m_formLayout->addRow("Root状态:", m_rootStatusLabel);
    m_formLayout->addRow("电池状态:", m_batteryLabel);
    
    // 存储部件引用
    m_infoWidgets["serial"] = m_serialLabel;
    m_infoWidgets["model"] = m_modelLabel;
    m_infoWidgets["manufacturer"] = m_manufacturerLabel;
    m_infoWidgets["android"] = m_androidVersionLabel;
    m_infoWidgets["bootloader"] = m_bootloaderLabel;
    m_infoWidgets["mode"] = m_modeTextEdit;
    m_infoWidgets["root"] = m_rootStatusLabel;
    m_infoWidgets["battery"] = m_batteryLabel;
    
    mainLayout->addWidget(infoGroup);
    mainLayout->addStretch();
}

QLabel* DeviceInfoPanel::createSelectableLabel(const QString &text)
{
    SelectableLabel *label = new SelectableLabel(text, this);
    label->setStyleSheet("QLabel { "
                         "background-color: #f8f8f8; "
                         "border: 1px solid #e0e0e0; "
                         "border-radius: 3px; "
                         "padding: 2px 5px; "
                         "}");
    return label;
}

QTextEdit* DeviceInfoPanel::createReadOnlyTextEdit(const QString &text)
{
    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setPlainText(text);
    textEdit->setReadOnly(true);
    textEdit->setFrameStyle(QFrame::StyledPanel);
    textEdit->setStyleSheet("QTextEdit { "
                           "background-color: #f8f8f8; "
                           "border: 1px solid #e0e0e0; "
                           "border-radius: 3px; "
                           "}");
    // 设置字体与QLabel一致
    textEdit->setFont(QFont());
    
    return textEdit;
}

void DeviceInfoPanel::updateDeviceInfo(const DeviceInfo &info)
{
    // 基本信息
    m_serialLabel->setText(info.serialNumber.isEmpty() ? "未知" : info.serialNumber);
    m_modelLabel->setText(info.model.isEmpty() ? "未知" : info.model);
    m_manufacturerLabel->setText(info.manufacturer.isEmpty() ? "未知" : info.manufacturer);
    m_androidVersionLabel->setText(info.androidVersion.isEmpty() ? "未知" : info.androidVersion);
    
    // Bootloader信息
    QString bootloaderText = info.bootloaderVersion.isEmpty() ? "未知" : info.bootloaderVersion;
    if (info.isBootloaderUnlocked) {
        bootloaderText += " (已解锁)";
    } else {
        bootloaderText += " (已锁定)";
    }
    m_bootloaderLabel->setText(bootloaderText);
    
    // 电池状态
    m_batteryLabel->setText(info.batteryHealth.isEmpty() ? "未知" : info.batteryHealth);
    
    // 模式信息 - 更详细的描述
    QString modeStr;
    QString modeDescription;
    switch (info.mode) {
    case DeviceDetector::MODE_ADB: 
        modeStr = "ADB模式";
        modeDescription = "Android调试模式，可执行ADB命令";
        break;
    case DeviceDetector::MODE_FASTBOOT: 
        modeStr = "Fastboot模式";
        modeDescription = "引导程序模式，可刷写分区";
        break;
    case DeviceDetector::MODE_FASTBOOTD: 
        modeStr = "Fastbootd模式";
        modeDescription = "Android 10+ 用户空间Fastboot，可刷写系统分区";
        break;
    case DeviceDetector::MODE_EDL_9008: 
        modeStr = "EDL 9008模式";
        modeDescription = "紧急下载模式，可进行底层修复";
        break;
    case DeviceDetector::MODE_MTK_DA: 
        modeStr = "MTK DA模式";
        modeDescription = "联发科下载模式，可刷写MTK设备";
        break;
    default: 
        modeStr = "未知模式";
        modeDescription = "无法识别的设备模式";
        break;
    }
    m_modeTextEdit->setPlainText(QString("%1\n%2").arg(modeStr).arg(modeDescription));
    
    // Root状态 - 根据Android版本显示不同信息
    QString rootStatus;
    if (info.isRooted) {
        rootStatus = "✅ 已Root";
    } else {
        // 检查Android版本，提供不同的Root建议
        QString androidVersion = info.androidVersion;
        if (androidVersion.contains("10") || androidVersion.contains("11") || 
            androidVersion.contains("12") || androidVersion.contains("13")) {
            rootStatus = "❌ 未Root (Android 10+ 建议使用Magisk)";
        } else {
            rootStatus = "❌ 未Root (可考虑使用SuperSU或Magisk)";
        }
    }
    m_rootStatusLabel->setText(rootStatus);
    
    // 根据Android版本显示兼容性信息
    QString androidVersion = info.androidVersion;
    if (!androidVersion.isEmpty()) {
        QString versionText = androidVersion;
        
        // 添加版本特定的兼容性信息
        if (androidVersion.contains("17")) {
            versionText += " (Android 17 - 支持Fastbootd)";
        } else if (androidVersion.contains("16")) {
            versionText += " (Android 16 - 支持Fastbootd)";
        } else if (androidVersion.contains("15")) {
            versionText += " (Android 15 - 支持Fastbootd)";
        } else if (androidVersion.contains("14")) {
            versionText += " (Android 14 - 支持Fastbootd)";
        } else if (androidVersion.contains("13")) {
            versionText += " (Android 13 - 支持Fastbootd)";
        } else if (androidVersion.contains("12")) {
            versionText += " (Android 12 - 支持Fastbootd)";
        } else if (androidVersion.contains("11")) {
            versionText += " (Android 11 - 支持Fastbootd)";
        } else if (androidVersion.contains("10")) {
            versionText += " (Android 10 - 支持Fastbootd)";
        } else if (androidVersion.contains("9")) {
            versionText += " (Android 9 - 仅传统Fastboot)";
        } else if (androidVersion.contains("8")) {
            versionText += " (Android 8 - 仅传统Fastboot)";
        } else {
            versionText += " (旧版本Android)";
        }
        
        m_androidVersionLabel->setText(versionText);
    }
    
    // 显示更多硬件信息（如果可用）
    if (!info.cpuInfo.isEmpty()) {
        if (!m_infoWidgets.contains("cpu")) {
            QLabel *cpuLabel = createSelectableLabel(info.cpuInfo);
            m_formLayout->addRow("CPU信息:", cpuLabel);
            m_infoWidgets["cpu"] = cpuLabel;
        } else {
            QLabel *cpuLabel = qobject_cast<QLabel*>(m_infoWidgets["cpu"]);
            if (cpuLabel) {
                cpuLabel->setText(info.cpuInfo);
            }
        }
    }
    
    if (!info.cpuInfo.isEmpty()) {
        const QString key = "cpu";
        if (!m_infoWidgets.contains(key)) {
            QLabel *cpuLabel = createSelectableLabel(info.cpuInfo);
            m_formLayout->addRow("CPU信息:", cpuLabel);
            m_infoWidgets[key] = cpuLabel;
        } else {
            QLabel *cpuLabel = qobject_cast<QLabel*>(m_infoWidgets[key]);
            if (cpuLabel) {  // 确保转换成功
                cpuLabel->setText(info.cpuInfo);
            } else {
                // 错误处理：类型不匹配
                qWarning() << "Widget for key" << key << "is not a QLabel";
            }
        }
    }
    
    if (!info.storageSize.isEmpty()) {
        if (!m_infoWidgets.contains("storage")) {
            QLabel *storageLabel = createSelectableLabel(info.storageSize);
            m_formLayout->addRow("存储空间:", storageLabel);
            m_infoWidgets["storage"] = storageLabel;
        } else {
            QLabel *storageLabel = qobject_cast<QLabel*>(m_infoWidgets["storage"]);
            if (storageLabel) {
                storageLabel->setText(info.storageSize);
            }
        }
    }
}

void DeviceInfoPanel::clearDeviceInfo()
{
    // 重置固定信息标签为默认值
    m_serialLabel->setText("未连接");
    m_modelLabel->setText("未连接");
    m_manufacturerLabel->setText("未连接");
    m_androidVersionLabel->setText("未连接");
    m_bootloaderLabel->setText("未连接");
    m_modeTextEdit->setPlainText("未连接");
    m_rootStatusLabel->setText("未连接");
    m_batteryLabel->setText("未连接");

    // 移除动态添加的额外信息项（CPU、内存、存储等）
    if (m_formLayout) {
        // 从后往前移除行（保留前8行固定项）
        int totalRows = m_formLayout->rowCount();
        // 从最后一行开始删除，直到第8行（索引7）
        for (int i = totalRows - 1; i >= 8; --i) {
            // 通过布局的removeRow方法安全移除行，会自动销毁该行的部件和布局项
            m_formLayout->removeRow(i);
            // 从映射中移除对应部件（避免后续更新时引用已删除的部件）
            QString key = m_infoWidgets.key(m_infoWidgets.value("cpu")); // 示例，需根据实际key调整
            if (!key.isEmpty()) {
                m_infoWidgets.remove(key);
            }
        }
    }
}

#include "device_info_panel.moc"
