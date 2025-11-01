#include <QApplication>
#include "ui/main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("Phone Toolbox");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PhoneToolbox");
    
    // 设置应用程序样式
    app.setStyle("Fusion");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
