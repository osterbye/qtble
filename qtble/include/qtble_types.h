#ifndef QTBLE_TYPES_H
#define QTBLE_TYPES_H

#include <QMetaType>
#include <QVector>

enum GATT_SERVER_STATE {
    GATT_STATE_ERROR,
    GATT_STATE_OFF,
    GATT_STATE_STARTING,
    GATT_STATE_WAITING_FOR_CLIENT,
    GATT_STATE_CONNECTED,
    GATT_STATE_RESTARTING
};
Q_DECLARE_METATYPE(GATT_SERVER_STATE);

struct uuid128 {
    QVector<quint8> attributeUuid;
    bool operator==(const uuid128 &a) const { return (attributeUuid == a.attributeUuid); }
};

const int uuidServiceIndex = 2;
const int uuidCharacteristicIndex = 3;

#endif // QTBLE_TYPES_H
