#ifndef EDL_9008_H
#define EDL_9008_H

#include <QObject>

class EDL9008 : public QObject
{
    Q_OBJECT

public:
    explicit EDL9008(QObject *parent = nullptr);
};

#endif // EDL_9008_H
