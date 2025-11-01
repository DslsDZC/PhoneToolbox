#ifndef OUTPUT_PANEL_H
#define OUTPUT_PANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

class OutputPanel : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPanel(QWidget *parent = nullptr);

public slots:
    void appendOutput(const QString &message, bool isError = false);
    void clearOutput();

private:
    void setupUI();
    
    QTextEdit *m_outputText;
    QPushButton *m_clearButton;
};

#endif // OUTPUT_PANEL_H
