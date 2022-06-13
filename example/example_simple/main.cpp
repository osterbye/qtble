#include <QCoreApplication>
#include <QDebug>
#include "qtble.h"
#include "qtblevalue.h"

void usage(char *name)
{
    qDebug() << "Usage:\n\t" << name << " name manufacturer\n\nExample:\n\t" << name
             << " myDevice myCompany\n";
    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (3 != argc)
        usage(argv[0]);

    QtBle qtBle(argv[1], argv[2]);
    uuid128 serviceUuid;

    serviceUuid.value = { 0x00, 0x00, 0x51, 0xda, 0xcd, 0xbd, 0x4e, 0xad,
                          0x99, 0xc8, 0xf2, 0x1d, 0x1e, 0xbb, 0x2f, 0x6d };
    qtBle.createGattService(serviceUuid);

    uuid128 characteristicUuid = serviceUuid;
    characteristicUuid.value[0] = 0x01;
    QtBleValue characteristic(characteristicUuid, QtBleValue::PERM_READ, QtBleValue::NOTI_NONE,
                              "Hello world");
    qtBle.addCharacteristicToService(serviceUuid, &characteristic);

    qtBle.startGattServer();

    return a.exec();
}
