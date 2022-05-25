#include "blecharacteristic.h"
#include "qtbleauth.h"
#include "qtblevalue.h"
#include "btuuid.h"

// BlueZ shared local copy
extern "C" {
#include "src/shared/att-types.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"
}

#define BLEC_DEBUG
#if defined BLEC_DEBUG
#include <QDebug>
#define BLEC_D(x) qDebug() << "[BleCharacteristic] " << x
#else
#define BLEC_D(x)
#endif

BleCharacteristic::BleCharacteristic(QtBleValue *value, QtBleAuth *auth, bt_gatt_server *server,
                                     BtUuid *btUuid, gatt_db_attribute *service, QObject *parent)
    : QObject(parent), p_value(value), p_auth(auth), p_gatt(server), m_ccc(0)
{
    BLEC_D(value->getUuid().attributeUuid);
    bt_uuid_t characUuid;
    struct gatt_db_attribute *attr;
    btUuid->btUuid128Create(&characUuid, value->getUuid().attributeUuid);
    quint32 permissions = 0;
    quint8 properties = 0;
    switch (value->accessPermissions()) {
    case QtBleValue::PERM_READ:
        permissions = BT_ATT_PERM_READ;
        properties = BT_GATT_CHRC_PROP_READ;
        break;
    case QtBleValue::PERM_WRITE:
        permissions = BT_ATT_PERM_WRITE;
        properties = BT_GATT_CHRC_PROP_WRITE;
        break;
    case QtBleValue::PERM_READ_AND_WRITE:
        permissions = BT_ATT_PERM_READ | BT_ATT_PERM_WRITE;
        properties = BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE;
        break;
    }
    switch (value->notificationPermissions()) {
    case QtBleValue::NOTI_NONE:
        // Do nothing
        break;
    case QtBleValue::NOTI_INDICATE:
        properties |= BT_GATT_CHRC_PROP_INDICATE;
        break;
    case QtBleValue::NOTI_NOTIFY:
        properties |= BT_GATT_CHRC_PROP_NOTIFY;
        break;
    case QtBleValue::NOTI_INDICATE_AND_NOTIFY:
        properties |= BT_GATT_CHRC_PROP_NOTIFY | BT_GATT_CHRC_PROP_INDICATE;
        break;
    }

    attr = gatt_db_service_add_characteristic(service, &characUuid, permissions, properties, read,
                                              write, this);
    m_handle = gatt_db_attribute_get_handle(attr);

    if ((properties & BT_GATT_CHRC_PROP_INDICATE) || (properties & BT_GATT_CHRC_PROP_NOTIFY)) {
        btUuid->btUuid16Create(&characUuid, GATT_CLIENT_CHARAC_CFG_UUID);
        gatt_db_service_add_descriptor(attr, &characUuid, BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                       cccRead, cccWrite, this);
    }

    connect(value, &QtBleValue::notify, this, &BleCharacteristic::notify);
}

void BleCharacteristic::read(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                             uint8_t opcode, struct bt_att *att, void *user_data)
{
    Q_UNUSED(opcode)
    Q_UNUSED(att)

    BLEC_D(__PRETTY_FUNCTION__);

    quint8 error = 0;
    const uint8_t *value = NULL;
    size_t len = 0;
    BleCharacteristic *t = static_cast<BleCharacteristic *>(user_data);
    if (!t->p_auth->isAuthorized()
        || (QtBleValue::PERM_READ != t->p_value->accessPermissions()
            && QtBleValue::PERM_READ_AND_WRITE != t->p_value->accessPermissions()))
        error = BT_ATT_ERROR_READ_NOT_PERMITTED;

    if (0 == error) {
        if (offset >= t->p_value->read().length())
            error = BT_ATT_ERROR_INVALID_OFFSET;

        if (0 == error) {
            len = t->p_value->read().length();
            len -= offset;
            value = reinterpret_cast<const uint8_t *>(t->p_value->read().constData());
            value += offset;
        } else {
            BLEC_D("Error: read offset:" << offset << ">=" << t->p_value->read().length());
        }
    } else {
        BLEC_D("Error: Read no permitted");
    }

    gatt_db_attribute_read_result(attrib, id, error, value, len);
}

void BleCharacteristic::write(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                              const uint8_t *value, size_t len, uint8_t opcode, struct bt_att *att,
                              void *user_data)
{
    Q_UNUSED(offset)
    Q_UNUSED(opcode)
    Q_UNUSED(att)

    BLEC_D(__PRETTY_FUNCTION__);

    quint8 error = 0;
    BleCharacteristic *t = static_cast<BleCharacteristic *>(user_data);
    if (!t->p_auth->isAuthorized()
        || (QtBleValue::PERM_WRITE != t->p_value->accessPermissions()
            && QtBleValue::PERM_READ_AND_WRITE != t->p_value->accessPermissions()))
        error = BT_ATT_ERROR_WRITE_NOT_PERMITTED;

    if (0 == error) {
        QByteArray ba = QByteArray::fromRawData(reinterpret_cast<const char *>(value), len);
        t->p_value->write(ba, offset);
    } else {
        BLEC_D("Error: Write no permitted");
    }

    gatt_db_attribute_write_result(attrib, id, error);
}

void BleCharacteristic::indicationCb(void *user_data)
{
    BLEC_D(__PRETTY_FUNCTION__);

    BleCharacteristic *t = static_cast<BleCharacteristic *>(user_data);
    t->p_value->indicationReply();
}

void BleCharacteristic::cccRead(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                                uint8_t opcode, struct bt_att *att, void *user_data)
{
    Q_UNUSED(offset)
    Q_UNUSED(opcode)
    Q_UNUSED(att)

    BLEC_D(__PRETTY_FUNCTION__);

    BleCharacteristic *t = static_cast<BleCharacteristic *>(user_data);
    uint8_t value[2];
    value[0] = t->m_ccc;
    value[1] = 0x00;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

void BleCharacteristic::cccWrite(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                                 const uint8_t *value, size_t len, uint8_t opcode,
                                 struct bt_att *att, void *user_data)
{
    Q_UNUSED(opcode)
    Q_UNUSED(att)

    BLEC_D(__PRETTY_FUNCTION__);

    BleCharacteristic *t = static_cast<BleCharacteristic *>(user_data);
    quint8 error = (offset > 0) ? BT_ATT_ERROR_INVALID_OFFSET : 0;

    if (error > 0) {
        BLEC_D("Error: write offset:" << offset);
    } else {
        if (!value || len != 2) {
            error = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
            BLEC_D("Error: invalid value or length:" << len);
        } else {
            t->m_ccc = value[0];
            BLEC_D("CCC changed:" << t->m_ccc);
        }
    }

    gatt_db_attribute_write_result(attrib, id, error);
}

void BleCharacteristic::notify()
{
    BLEC_D(__PRETTY_FUNCTION__);

    if (!p_auth->isAuthorized())
        return;

    QByteArray ba = p_value->read();
    size_t len = ba.length();
    if (len > 0) {
        const uint8_t *value = reinterpret_cast<const uint8_t *>(ba.constData());
        if (1 == m_ccc
            && (QtBleValue::NOTI_NOTIFY == p_value->notificationPermissions()
                || QtBleValue::NOTI_INDICATE_AND_NOTIFY == p_value->notificationPermissions())) {
            BLEC_D("Notify");
            bt_gatt_server_send_notification(p_gatt, m_handle, value, len);
        } else if (2 == m_ccc
                   && (QtBleValue::NOTI_INDICATE == p_value->notificationPermissions()
                       || QtBleValue::NOTI_INDICATE_AND_NOTIFY
                               == p_value->notificationPermissions())) {
            BLEC_D("Indicate");
            bt_gatt_server_send_indication(p_gatt, m_handle, value, len, indicationCb, this, NULL);
        }
    }
}
