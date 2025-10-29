#ifndef DEVICE_WIDGET_H
#define DEVICE_WIDGET_H

#include <QWidget>

class DeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWidget(QWidget *parent = nullptr);
};

#endif // DEVICE_WIDGET_H
