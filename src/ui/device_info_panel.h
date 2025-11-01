#ifndef DEVICE_INFO_PANEL_H
#define DEVICE_INFO_PANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QFormLayout>
#include "core/device_detector.h"

class DeviceInfoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInfoPanel(QWidget *parent = nullptr);
    void updateDeviceInfo(const DeviceInfo &info);
    void clearDeviceInfo();

private:
    void setupUI();
    QLabel* createSelectableLabel(const QString &text = "");
    QTextEdit* createReadOnlyTextEdit(const QString &text = "");
    
    QFormLayout *m_formLayout;
    
    // 使用可选择的标签或文本框
    QLabel *m_serialLabel;
    QLabel *m_modelLabel;
    QLabel *m_manufacturerLabel;
    QLabel *m_androidVersionLabel;
    QLabel *m_bootloaderLabel;
    QTextEdit *m_modeTextEdit;  // 改为QTextEdit以支持多行选择和复制
    QLabel *m_rootStatusLabel;
    QLabel *m_batteryLabel;
    
    // 用于存储标签的引用，方便更新
    QMap<QString, QWidget*> m_infoWidgets;
};

#endif // DEVICE_INFO_PANEL_H
