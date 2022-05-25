#include "qtble.h"
#include "gattserver.h"

QtBle::QtBle(QString deviceName, QString deviceManufacturer, quint16 deviceAppearance,
             QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<GATT_SERVER_STATE>();

    m_gattServer = new GattServer(deviceName, deviceManufacturer, deviceAppearance, this);
    connect(m_gattServer, &GattServer::gattStateChanged, this, &QtBle::gattStateChanged);
}

bool QtBle::createGattService(uuid128 uuid, QtBleAuth *authenticator)
{
    return m_gattServer->createService(uuid, authenticator);
}

bool QtBle::addCharacteristicToService(uuid128 serviceUuid, QtBleValue *value)
{
    return m_gattServer->addCharacteristicToService(serviceUuid, value);
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
