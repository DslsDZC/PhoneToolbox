#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "core/device_detector.h"
#include "core/adb_embedded.h"
#include "ui/main_window.h"

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupConnections();
        
        // 初始化ADB
        if (AdbEmbedded::instance().initialize()) {
            m_statusLabel->setText("状态: ADB 初始化成功");
            m_detector.startMonitoring();
        } else {
            m_statusLabel->setText("状态: ADB 初始化失败");
            QMessageBox::warning(this, "错误", "无法初始化嵌入式ADB工具");
        }
    }

private slots:
    void onDeviceConnected(const DeviceInfo &info)
    {
        m_textEdit->append("🔗 设备已连接:");
        m_textEdit->append(info.toString());
        m_textEdit->append("---");
    }
    
    void onDeviceDisconnected(const QString &serial)
    {
        m_textEdit->append("❌ 设备已断开: " + serial);
        m_textEdit->append("---");
    }
    
    void onTestAdb()
    {
        m_textEdit->append("测试ADB命令...");
        QString result = AdbEmbedded::instance().executeCommand("version");
        m_textEdit->append("ADB版本信息:\n" + result);
    }
    
    void onClearLog()
    {
        m_textEdit->clear();
    }

private:
    void setupUI()
    {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // 状态标签
        m_statusLabel = new QLabel("状态: 正在初始化...", this);
        mainLayout->addWidget(m_statusLabel);
        
        // 文本显示区域
        m_textEdit = new QTextEdit(this);
        m_textEdit->setReadOnly(true);
        mainLayout->addWidget(m_textEdit);
        
        // 按钮区域
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *testButton = new QPushButton("测试ADB", this);
        QPushButton *clearButton = new QPushButton("清空日志", this);
        
        buttonLayout->addWidget(testButton);
        buttonLayout->addWidget(clearButton);
        mainLayout->addLayout(buttonLayout);
        
        setCentralWidget(centralWidget);
        setWindowTitle("Phone Toolbox - 测试界面");
        resize(800, 600);
        
        m_textEdit->append("🚀 Phone Toolbox 已启动");
        m_textEdit->append("正在初始化嵌入式ADB...");
    }
    
    void setupConnections()
    {
        connect(&m_detector, &DeviceDetector::deviceConnected,
                this, &TestWindow::onDeviceConnected);
        connect(&m_detector, &DeviceDetector::deviceDisconnected,
                this, &TestWindow::onDeviceDisconnected);
    }

    DeviceDetector m_detector;
    QTextEdit *m_textEdit;
    QLabel *m_statusLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("Phone Toolbox");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PhoneToolbox");
    
    TestWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"
