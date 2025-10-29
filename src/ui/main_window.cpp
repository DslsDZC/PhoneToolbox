#include "main_window.h"
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    QLabel *titleLabel = new QLabel("Phone Toolbox - 设备检测运行中...", this);
    layout->addWidget(titleLabel);
    
    setCentralWidget(centralWidget);
    setWindowTitle("Phone Toolbox");
    resize(800, 600);
}
