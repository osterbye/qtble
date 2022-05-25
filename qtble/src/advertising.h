#ifndef ADVERTISING_H
#define ADVERTISING_H

#include <QObject>

class Advertising : public QObject
{
    Q_OBJECT
public:
    explicit Advertising(QObject *parent = nullptr);

    void setLeAdvertising(bool enabled);
    void setLeAdvertisingParameters();
    void setLeAdvertisingData(const QString &deviceName);

private:
    void systemCall(const QString &program, const QStringList &arguments);
};

#endif // ADVERTISING_H
