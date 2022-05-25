#include "bleservice.h"
#include "btuuid.h"
#include "blecharacteristic.h"

// BlueZ shared local copy
extern "C" {
#include "src/shared/gatt-db.h"
}

#define BLES_DEBUG
#if defined BLES_DEBUG
#include <QDebug>
#define BLES_D(x) qDebug() << "[BleService] " << x
#else
#define BLES_D(x)
#endif

BleService::BleService(bt_gatt_server *server, gatt_db *gattDb, BtUuid *btUuid, gattService *srvc,
                       QObject *parent)
    : QObject(parent)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *service;
    BLES_D("Service UUID:" << srvc->serviceUuid.attributeUuid);

    btUuid->btUuid128Create(&uuid, srvc->serviceUuid.attributeUuid);
    service = gatt_db_add_service(gattDb, &uuid, true, (srvc->values.count() * 2) + 4);

    BLES_D("Number of characteristics:" << srvc->values.count());
    for (int j = 0; j < srvc->values.count(); ++j) {
        m_characteristics.append(new BleCharacteristic(srvc->values.at(j), srvc->authenticator,
                                                       server, btUuid, service, this));
    }

    gatt_db_service_set_active(service, true);
}
