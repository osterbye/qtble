#include "gattserver.h"

// BlueZ shared local copy
extern "C" {
#include "uuid.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"
}

#include "qtbleauth.h"
#include "advertising.h"
#include "blecharacteristic.h"
#include "bleservice.h"
#include "btatt.h"
#include "btsocket.h"
#include "btuuid.h"
#include "gapservice.h"

#include <QTimer>
#include <qplatformdefs.h>

#define GS_DEBUG
#if defined GS_DEBUG
#include <QDebug>
#define GS_D(x) qDebug() << "[GattServer] " << x
static void debugCallback(const char *str, void *user_data)
{
    Q_UNUSED(user_data)
    qDebug("[GattServer] (BlueZ): %s", str);
}
#else
#define GS_D(x)
#endif

GattServer::GattServer(QString deviceName, QString deviceManufacturer, quint16 deviceAppearance,
                       QObject *parent)
    : QObject(parent),
      m_fdClient(-1),
      m_gattDb(nullptr),
      m_gatt(nullptr),
      m_startRequested(false),
      m_authAlwaysAllowed(nullptr)
{
    m_advertising = new Advertising(this);
    m_btAtt = new BtAtt(this);
    m_btSocket = new BtSocket(this);
    m_btUuid = new BtUuid(this);
    connect(m_btAtt, &BtAtt::clientDisconnected, this, &GattServer::clientDisconnected);
    m_gapService = new GapService(deviceName, deviceManufacturer, deviceAppearance, this);
    m_state = GATT_STATE_OFF;
}

GattServer::~GattServer()
{
    stop();
    clearServices();
}

bool GattServer::createService(uuid128 uuid, QtBleAuth *authenticator)
{
    for (int i = 0; i < m_services.count(); ++i) {
        if (uuid == m_services.at(i)->serviceUuid)
            return false;
    }

    if (nullptr == authenticator) {
        if (nullptr == m_authAlwaysAllowed)
            m_authAlwaysAllowed = new QtBleAuth(true, this);

        authenticator = m_authAlwaysAllowed;
    }
    gattService *service = new gattService();
    service->serviceUuid = uuid;
    service->authenticator = authenticator;
    m_services.append(service);

    return true;
}

bool GattServer::addCharacteristicToService(uuid128 serviceUuid, QtBleValue *value)
{
    gattService *service = nullptr;
    for (int i = m_services.count() - 1; i >= 0; --i) {
        if (serviceUuid == m_services.at(i)->serviceUuid) {
            service = m_services.at(i);
            break;
        }
    }

    if (nullptr == service)
        return false;

    service->values.append(value);

    return true;
}

void GattServer::clearServices()
{
    while (!m_services.isEmpty())
        delete m_services.takeLast();
}

GATT_SERVER_STATE GattServer::state()
{
    return m_state;
}

void GattServer::start()
{
    GS_D(__PRETTY_FUNCTION__);
    if (!m_startRequested) {
        m_startRequested = true;
        setState(GATT_STATE_STARTING);
        startService();
    }
}

void GattServer::stop()
{
    GS_D(__PRETTY_FUNCTION__);
    if (m_startRequested) {
        m_startRequested = false;
        stopService();
        setState(GATT_STATE_OFF);
    }
}

void GattServer::startService()
{
    GS_D(__PRETTY_FUNCTION__);
    if (GATT_STATE_WAITING_FOR_CLIENT == m_state || GATT_STATE_CONNECTED == m_state)
        return;

    m_advertising->setLeAdvertising(false);
    m_advertising->setLeAdvertisingParameters();
    m_advertising->setLeAdvertising(true);
    m_advertising->setLeAdvertisingData(m_gapService->deviceName());

    setState(GATT_STATE_WAITING_FOR_CLIENT);
    m_fdClient = m_btSocket->waitForClient();
    if (m_fdClient < 0) {
        if (m_startRequested)
            setState(GATT_STATE_ERROR);

        return;
    }

    if (!createServer()) {
        stop();
        setState(GATT_STATE_ERROR);
        return;
    }

    m_gapService->create(m_gattDb, m_btUuid);
    createCustomServices();

    setState(GATT_STATE_CONNECTED);
}

void GattServer::stopService()
{
    GS_D(__PRETTY_FUNCTION__);
    m_advertising->setLeAdvertising(false);

    if (GATT_STATE_CONNECTED == m_state) {
        destroyServer();
    } else if (GATT_STATE_WAITING_FOR_CLIENT == m_state) {
        m_btSocket->abortWait();
    }

    if (m_fdClient >= 0) {
        QT_CLOSE(m_fdClient);
        m_fdClient = -1;
    }
}

void GattServer::clientDisconnected()
{
    GS_D(__PRETTY_FUNCTION__);
    stopService();
    setState(GATT_STATE_RESTARTING);
    startService();
}

bool GattServer::createServer()
{
    GS_D(__PRETTY_FUNCTION__);
    const quint16 mtu = 0;

    if (m_fdClient < 0)
        return false;

    if (!m_btAtt->create(m_fdClient)) {
        GS_D("Failed to initialize ATT");
        return false;
    }

    m_gattDb = gatt_db_new();
    if (!m_gattDb) {
        GS_D("Failed to create GATT database");
        m_btAtt->close();
        return false;
    }

    m_gatt = bt_gatt_server_new(m_gattDb, m_btAtt->att(), mtu);
    if (!m_gatt) {
        GS_D("Failed to create GATT server");
        gatt_db_unref(m_gattDb);
        m_btAtt->close();
        return false;
    }

#if defined GS_DEBUG
    bt_gatt_server_set_debug(m_gatt, debugCallback, NULL, NULL);
#endif

    return true;
}

void GattServer::destroyServer()
{
    GS_D(__PRETTY_FUNCTION__);
    if (m_gatt)
        bt_gatt_server_unref(m_gatt);

    if (m_gattDb)
        gatt_db_unref(m_gattDb);

    m_btAtt->close();

    while (!m_customServices.isEmpty())
        delete m_customServices.takeLast();
}

void GattServer::createCustomServices()
{
    GS_D(__PRETTY_FUNCTION__ << ": " << m_services.count() << " services");
    for (int i = 0; i < m_services.count(); ++i) {
        BleService *service = new BleService(m_gatt, m_gattDb, m_btUuid, m_services.at(i), this);
        m_customServices.append(service);
    }
}

void GattServer::setState(GATT_SERVER_STATE state)
{
    GS_D("New state:" << state);
    m_state = state;
    emit gattStateChanged(m_state);
}
