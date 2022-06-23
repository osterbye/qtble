#include "qtble.h"
#include "gattserver.h"
#include <QThread>
#include <QTimer>

QtBle::QtBle(QString deviceName, QString storagePath, QString deviceManufacturer,
             quint16 deviceAppearance, QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<GATT_SERVER_STATE>();

    m_gattServer = new GattServer(deviceName, storagePath, deviceManufacturer, deviceAppearance);
    m_serverThread = new QThread(this);
    m_gattServer->moveToThread(m_serverThread);
    m_serverThread->start();
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
    QTimer::singleShot(0, m_gattServer, &GattServer::start);
}

void QtBle::stopGattServer()
{
    QTimer::singleShot(0, m_gattServer, &GattServer::stop);
}
