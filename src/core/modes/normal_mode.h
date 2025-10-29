#ifndef NORMAL_MODE_H
#define NORMAL_MODE_H

#include <QObject>

class NormalMode : public QObject
{
    Q_OBJECT

public:
    explicit NormalMode(QObject *parent = nullptr);
};

#endif // NORMAL_MODE_H
