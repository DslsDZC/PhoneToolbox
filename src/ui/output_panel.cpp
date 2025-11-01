#include "output_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QDateTime>
#include <QMenu>
#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include <QLabel> 

// 可复制的文本编辑框
class CopyableTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CopyableTextEdit(QWidget *parent = nullptr) : QTextEdit(parent)
    {
        setReadOnly(true);
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CopyableTextEdit::customContextMenuRequested,
                this, &CopyableTextEdit::showContextMenu);
    }

private slots:
    void showContextMenu(const QPoint &pos)
    {
        QMenu *menu = createStandardContextMenu();
        
        // 添加额外的复制选项
        QAction *copyAllAction = new QAction("复制全部内容", this);
        connect(copyAllAction, &QAction::triggered, this, [this]() {
            QApplication::clipboard()->setText(this->toPlainText());
        });
        
        menu->insertAction(menu->actions().first(), copyAllAction);
        menu->insertSeparator(menu->actions().at(1));
        
        menu->exec(mapToGlobal(pos));
        delete menu;
    }
};

OutputPanel::OutputPanel(QWidget *parent)
    : QWidget(parent)
    , m_outputText(nullptr)
    , m_clearButton(nullptr)
{
    setupUI();
}

void OutputPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // 标题和清除按钮
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_clearButton = new QPushButton("清空输出", this);
    
    headerLayout->addWidget(new QLabel("命令输出 (所有文本均可选择复制)", this));
    headerLayout->addStretch();
    headerLayout->addWidget(m_clearButton);
    
    // 输出文本框 - 使用可复制的版本
    m_outputText = new CopyableTextEdit(this);
    m_outputText->setFont(QFont("Monospace", 9));
    m_outputText->setStyleSheet("QTextEdit { "
                               "background-color: #f8f8f8; "
                               "border: 1px solid #e0e0e0; "
                               "}");
    
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(m_outputText);
    
    // 连接信号
    connect(m_clearButton, &QPushButton::clicked, this, &OutputPanel::clearOutput);
}

void OutputPanel::appendOutput(const QString &message, bool isError)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedMessage = QString("[%1] %2").arg(timestamp).arg(message);
    
    // 保存当前文本颜色
    QTextCursor cursor = m_outputText->textCursor();
    QTextCharFormat originalFormat = cursor.charFormat();
    
    QTextCharFormat format;
    if (isError) {
        format.setForeground(QColor(200, 0, 0)); // 红色错误信息
    } else {
        format.setForeground(QColor(0, 0, 0)); // 黑色正常信息
    }
    
    // 移动到文档末尾
    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(format);
    cursor.insertText(formattedMessage + "\n");
    
    // 恢复原始格式
    cursor.setCharFormat(originalFormat);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = m_outputText->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void OutputPanel::clearOutput()
{
    m_outputText->clear();
}

#include "output_panel.moc"
