#ifndef BTUUID_H
#define BTUUID_H

#include <QObject>
#include <QVector>

// BlueZ shared local copy
extern "C" {
#include "uuid-types.h"
}

#define UUID_GAP 0x1800
#define UUID_GATT 0x1801
#define GATT_CHARAC_EXT_PROPER_UUID 0x2900
#define GATT_CLIENT_CHARAC_CFG_UUID 0x2902
#define GATT_CHARAC_DEVICE_NAME 0x2A00
#define GATT_CHARAC_APPEARANCE 0x2A01
#define GATT_CHARAC_SERVICE_CHANGED 0x2A05
#define GATT_CHARAC_MANUFACTURER 0x2A29

class BtUuid : public QObject
{
    Q_OBJECT
public:
    explicit BtUuid(QObject *parent = nullptr);

    int btUuid16Create(bt_uuid_t *btuuid, quint16 value);
    int btUuid32Create(bt_uuid_t *btuuid, quint32 value);
    int btUuid128Create(bt_uuid_t *btuuid, uint128_t value);
    int btUuid128Create(bt_uuid_t *btuuid, QVector<quint8> value);
};

#endif // BTUUID_H
