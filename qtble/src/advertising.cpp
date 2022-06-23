#include "advertising.h"
#include <QProcess>

#define ADV_DEBUG
#if defined ADV_DEBUG
#include <QDebug>
#define ADV_D(x) qDebug() << "[Advertising] " << x
#else
#define ADV_D(x)
#endif

Advertising::Advertising(QObject *parent) : QObject(parent) { }

void Advertising::setLeAdvertising(bool enabled)
{
    ADV_D(__PRETTY_FUNCTION__);
    QString enableArg = (enabled) ? "01" : "00";
    QStringList arguments = QStringList() << "cmd"
                                          << "0x08"
                                          << "0x000A" << enableArg;
    systemCall("hcitool", arguments);
    ADV_D(__PRETTY_FUNCTION__ << " done");
}

void Advertising::setLeAdvertisingParameters()
{
    ADV_D(__PRETTY_FUNCTION__);
    QStringList advertisingIntervalMin = QStringList() << "20"
                                                       << "03";
    QStringList advertisingIntervalMax = QStringList() << "00"
                                                       << "08";
    QString advertisingTypeConnectable = "00";
    QString ownAddressTypePublic = "00";
    QString directAddressTypePublic = "00";
    QStringList directAddress = QStringList() << "00"
                                              << "00"
                                              << "00"
                                              << "00"
                                              << "00"
                                              << "00";
    QString advertisingChannelMapAll = "07";
    QString advertisingFilterPolicyNoWhite = "00";

    QStringList arguments = QStringList()
            << "cmd"
            << "0x08"
            << "0x0006" << advertisingIntervalMin << advertisingIntervalMax
            << advertisingTypeConnectable << ownAddressTypePublic << directAddressTypePublic
            << directAddress << advertisingChannelMapAll << advertisingFilterPolicyNoWhite;

    systemCall("hcitool", arguments);
    ADV_D(__PRETTY_FUNCTION__ << " done");
}

void Advertising::setLeAdvertisingData(const QString &deviceName)
{
    ADV_D(__PRETTY_FUNCTION__);
    QStringList flag = QStringList() << "02"
                                     << "01"
                                     << "1e";
    QStringList power = QStringList() << "02"
                                      << "0a"
                                      << "04";
    QStringList name;
    int nameLen = deviceName.size() + 1;
    name.append(QString("%1").arg(nameLen, 2, 16, QChar('0')));
    // Name type
    name.append("09");
    for (int i = 0; i < deviceName.size(); ++i) {
        QString s = QString("%1").arg(deviceName.at(i).unicode(), 2, 16, QChar('0'));
        // ADV_D(" " << s);
        name.append(s);
    }

    QStringList arguments = QStringList() << "cmd"
                                          << "0x08"
                                          << "0x0008"
                                          << "1e" << flag << power << name;
    // Advertisement payload must be 32 bytes + cmd + OCF + OGF
    while (arguments.size() < 35)
        arguments.append("00");

    systemCall("hcitool", arguments);
    ADV_D(__PRETTY_FUNCTION__ << " done");
}

void Advertising::systemCall(const QString &program, const QStringList &arguments)
{
    QProcess pCtrl;
    pCtrl.start(program, arguments);
    bool retval = pCtrl.waitForFinished();
    if (!retval) {
        qWarning("[Advertising] systemCall(%s, &arguments) error: %s", qUtf8Printable(program),
                 qUtf8Printable(pCtrl.errorString()));
    } else {
        // ADV_D(pCtrl.readAllStandardOutput());
    }
}
