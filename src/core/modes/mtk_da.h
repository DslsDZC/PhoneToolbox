#ifndef MTK_DA_H
#define MTK_DA_H

#include <QObject>

class MTKDA : public QObject
{
    Q_OBJECT

public:
    explicit MTKDA(QObject *parent = nullptr);
};

#endif // MTK_DA_H
