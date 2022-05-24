#include "qtble.h"
#include "qtble_types.h"

QtBle::QtBle(QString deviceName, QString deviceManufacturer, quint16 deviceAppearance,
             QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<GATT_SERVER_STATE>();
    qRegisterMetaType<gattService>();
}

quint8 QtBle::createGattService(uuid128 uuid, QtBleAuth *authenticator)
{
    m_gattServer->addGattService(uuid);
}

bool QtBle::addCharacteristicToService(quint8 serviceIndex, QtBleValue *value)
{
    m_gattServer->addCharacteristicToService(quint8 serviceIndex, QtBleValue * value);
}

void QtBle::clearGattServices()
{
    m_gattServer->clearServices();
}

GATT_SERVER_STATE QtBle::stateGatt()
{
    return m_gattServer->state();
}

void QtBle::startGattServer()
{
    m_gattServer->start();
}

void QtBle::stopGattServer()
{
    m_gattServer->stop();
}
