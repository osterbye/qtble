#include "gattserver.h"

// BlueZ shared local copy
extern "C" {
#include "uuid.h"
#include "src/shared/att.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"
#include "src/shared/util.h"
}

#include "qtbleauth.h"
#include "advertising.h"
#include "blecharacteristic.h"
#include "bleservice.h"
#include "btatt.h"
#include "btsocket.h"
#include "btuuid.h"

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
      m_deviceName(deviceName),
      m_deviceManufacturer(deviceManufacturer),
      m_deviceAppearance(deviceAppearance),
      m_authAlwaysAllowed(nullptr)
{
    m_advertising = new Advertising(this);
    m_btAtt = new BtAtt(this);
    m_btSocket = new BtSocket(this);
    m_btUuid = new BtUuid(this);
    connect(m_btAtt, &BtAtt::clientDisconnected, this, &GattServer::clientDisconnected);
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

/*void GattServer::addGattService(gattService *service)
{
    GS_D(__PRETTY_FUNCTION__ << ", uuid:" << service->serviceUuid.attributeUuid);
    m_services.append(service);
}*/

/*void GattServer::btStateChanged(BT_STATE state)
{
    GS_D(__PRETTY_FUNCTION__);
    if (m_startRequested && BT_STATE_ON == state && m_state < GATT_STATE_WAITING_FOR_CLIENT)
        startService();
}*/

void GattServer::startService()
{
    GS_D(__PRETTY_FUNCTION__);
    if (GATT_STATE_WAITING_FOR_CLIENT == m_state || GATT_STATE_CONNECTED == m_state)
        return;

    m_advertising->setLeAdvertising(false);
    m_advertising->setLeAdvertisingParameters();
    m_advertising->setLeAdvertising(true);
    m_advertising->setLeAdvertisingData(m_deviceName);

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

    createGapService();
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

void GattServer::createGapService()
{
    GS_D(__PRETTY_FUNCTION__);
    /*
        bt_uuid_t uuid;
        struct gatt_db_attribute *service, *tmp;
        quint16 appearance;
        const char *manufacturer = "Paralenz\0";

        m_btUuid->btUuid16Create(&uuid, UUID_GAP);
        service = gatt_db_add_service(m_gattDb, &uuid, true, 8);

        m_btUuid->btUuid16Create(&uuid, GATT_CHARAC_DEVICE_NAME);
        gatt_db_service_add_characteristic(service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ,
                                           gap_device_name_read_cb, NULL, NULL);

        m_btUuid->btUuid16Create(&uuid, GATT_CHARAC_EXT_PROPER_UUID);
        gatt_db_service_add_descriptor(service, &uuid, BT_ATT_PERM_READ,
                                       gap_device_name_ext_prop_read_cb, NULL, NULL);

        m_btUuid->btUuid16Create(&uuid, GATT_CHARAC_APPEARANCE);
        tmp = gatt_db_service_add_characteristic(service, &uuid, BT_ATT_PERM_READ,
                                                 BT_GATT_CHRC_PROP_READ, NULL, NULL, NULL);
        put_le16(0x1440, &appearance);
        gatt_db_attribute_write(tmp, 0, (quint8 *)&appearance, sizeof(appearance),
       BT_ATT_OP_WRITE_REQ, NULL, confirm_write, NULL);

        m_btUuid->btUuid16Create(&uuid, GATT_CHARAC_MANUFACTURER);
        struct gatt_db_attribute *manu = gatt_db_service_add_characteristic(
                service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ, NULL, NULL, NULL);
        gatt_db_attribute_write(manu, 0, (quint8 *)manufacturer, sizeof(manufacturer),
                                BT_ATT_OP_WRITE_REQ, NULL, confirm_write, NULL);

        gatt_db_service_set_active(service, true);
    */
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

void GattServer::confirm_write(struct gatt_db_attribute *attr, int err, void *user_data)
{
    Q_UNUSED(user_data)
    if (!err)
        return;

    qWarning("[GattServer] Error caching attribute %p - err: %d\n", attr, err);
}
/*
void GattServer::gap_device_name_read_cb(struct gatt_db_attribute *attrib, unsigned int id,
                                         uint16_t offset, uint8_t opcode, struct bt_att *att,
                                         void *user_data)
{
    Q_UNUSED(opcode)
    Q_UNUSED(att)
    Q_UNUSED(user_data)
    QByteArray ba;

    uint8_t error = 0;
    size_t len = 0;
    const uint8_t *value = NULL;

    GS_D("GAP Device Name Read called");

    len = GattServer::m_deviceName.size();
    if (offset > len) {
        GS_D("Error: offset (" << offset << ") > len (" << len << ")");
        error = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    len -= offset;
    ba = GattServer::m_deviceName.toLatin1();
    value = len ? (const uint8_t *)ba.constData() : NULL;

done:
    gatt_db_attribute_read_result(attrib, id, error, value, len);
}

void GattServer::gap_device_name_ext_prop_read_cb(struct gatt_db_attribute *attrib, unsigned int id,
                                                  uint16_t offset, uint8_t opcode,
                                                  struct bt_att *att, void *user_data)
{
    Q_UNUSED(offset)
    Q_UNUSED(opcode)
    Q_UNUSED(att)
    Q_UNUSED(user_data)
    uint8_t value[2];

    GS_D("Device Name Extended Properties Read called");

    value[0] = BT_GATT_CHRC_EXT_PROP_RELIABLE_WRITE;
    value[1] = 0;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}
*/
