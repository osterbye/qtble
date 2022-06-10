#include "gapservice.h"
#include "btuuid.h"

#include <QFile>

// BlueZ shared local copy
extern "C" {
#include "src/shared/att.h"
#include "src/shared/gatt-db.h"
#include "src/shared/util.h"
}

#define GAP_DEBUG
#if defined GAP_DEBUG
#include <QDebug>
#define GAP_D(x) qDebug() << "[GapService] " << x
#else
#define GAP_D(x)
#endif

#define DEVICE_NAME_FILE "/bledevicename"

GapService::GapService(QString deviceName, QString storagePath, QString deviceManufacturer,
                       quint16 deviceAppearance, QObject *parent)
    : QObject(parent),
      m_deviceName(deviceName),
      m_storagePath(storagePath),
      m_deviceManufacturer(deviceManufacturer),
      m_deviceAppearance(deviceAppearance)
{
    if (!m_storagePath.isEmpty()) {
        GAP_D("Storage path:" << m_storagePath);
        QFile f(m_storagePath + DEVICE_NAME_FILE);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray input = f.readLine();
            f.close();
            if (!input.isEmpty())
                m_deviceName = input;
        }
    }
}

void GapService::create(gatt_db *gattDb, BtUuid *btUuid)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *service, *tmp;
    quint16 appearance;

    btUuid->btUuid16Create(&uuid, UUID_GAP);
    //bt_uuid16_create(&uuid, UUID_GAP);
    service = gatt_db_add_service(gattDb, &uuid, true, 8);

    btUuid->btUuid16Create(&uuid, GATT_CHARAC_DEVICE_NAME);
    if (m_storagePath.isEmpty()) {
        gatt_db_service_add_characteristic(service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ,
                                           readName, NULL, this);

    } else {
        gatt_db_service_add_characteristic(service, &uuid, BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                           BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_EXT_PROP,
                                           readName, writeName, this);

        btUuid->btUuid16Create(&uuid, GATT_CHARAC_EXT_PROPER_UUID);
        gatt_db_service_add_descriptor(service, &uuid, BT_ATT_PERM_READ, readNameExtProps, NULL,
                                       NULL);
    }

    if (0 != m_deviceAppearance) {
        btUuid->btUuid16Create(&uuid, GATT_CHARAC_APPEARANCE);
        tmp = gatt_db_service_add_characteristic(service, &uuid, BT_ATT_PERM_READ,
                                                 BT_GATT_CHRC_PROP_READ, NULL, NULL, NULL);
        put_le16(m_deviceAppearance, &appearance);
        gatt_db_attribute_write(tmp, 0, (quint8 *)&appearance, sizeof(appearance),
                                BT_ATT_OP_WRITE_REQ, NULL, confirmAttrWrite, NULL);
    }

    if (!m_deviceManufacturer.isEmpty()) {
        QByteArray manufacturer = m_deviceManufacturer.toLatin1();
        btUuid->btUuid16Create(&uuid, GATT_CHARAC_MANUFACTURER_NAME_STRING);
        struct gatt_db_attribute *manu = gatt_db_service_add_characteristic(
                service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ, NULL, NULL, NULL);
        gatt_db_attribute_write(manu, 0, (quint8 *)manufacturer.constData(),
                                sizeof(manufacturer.constData()), BT_ATT_OP_WRITE_REQ, NULL,
                                confirmAttrWrite, NULL);
    }

    gatt_db_service_set_active(service, true);
}

const QString &GapService::deviceName()
{
    return m_deviceName;
}

void GapService::saveNewDeviceName(QString name)
{
    if (!m_storagePath.isEmpty()) {
        QFile f(m_storagePath + DEVICE_NAME_FILE);
        if (name.isEmpty()) {
            f.remove();
        } else {
            if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                f.write(name.toLatin1());
                f.close();
            }
        }
    }
}

void GapService::confirmAttrWrite(struct gatt_db_attribute *attr, int err, void *user_data)
{
    Q_UNUSED(user_data)
    if (!err)
        return;

    qWarning("[GapService] Error caching attribute %p - err: %d\n", attr, err);
}

void GapService::readName(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                          quint8 opcode, struct bt_att *att, void *user_data)
{
    Q_UNUSED(opcode)
    Q_UNUSED(att)
    GAP_D(__PRETTY_FUNCTION__);

    quint8 error = 0;
    const quint8 *value = NULL;
    size_t len = 0;
    GapService *t = static_cast<GapService *>(user_data);

    if (offset >= t->m_deviceName.length())
        error = BT_ATT_ERROR_INVALID_OFFSET;

    if (0 == error) {
        len = t->m_deviceName.length();
        len -= offset;
        value = reinterpret_cast<const quint8 *>(t->m_deviceName.toLatin1().constData());
        value += offset;
    } else {
        GAP_D("Error: read offset:" << offset << ">=" << t->m_deviceName.length());
    }
    gatt_db_attribute_read_result(attrib, id, error, value, len);
}

void GapService::writeName(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                           const quint8 *value, size_t len, quint8 opcode, struct bt_att *att,
                           void *user_data)
{
    Q_UNUSED(opcode)
    Q_UNUSED(att)
    GAP_D(__PRETTY_FUNCTION__);

    quint8 error = 0;
    GapService *t = static_cast<GapService *>(user_data);
    quint32 totalLength = offset + len;

    if (0 == totalLength) {
        t->m_deviceName = "";
    } else {
        QString stringValue = QString::fromLatin1(reinterpret_cast<const char *>(value));
        t->m_deviceName = t->m_deviceName.leftJustified(totalLength, 0x00);
        t->m_deviceName.replace(offset, len, stringValue);
    }
    t->saveNewDeviceName(t->m_deviceName);
    gatt_db_attribute_write_result(attrib, id, error);
}

void GapService::readNameExtProps(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                                  quint8 opcode, struct bt_att *att, void *user_data)
{
    Q_UNUSED(offset)
    Q_UNUSED(opcode)
    Q_UNUSED(att)
    Q_UNUSED(user_data)
    GAP_D(__PRETTY_FUNCTION__);

    quint8 value[2];

    value[0] = BT_GATT_CHRC_EXT_PROP_RELIABLE_WRITE;
    value[1] = 0;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}
