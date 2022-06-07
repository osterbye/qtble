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
    QVector<quint8> value;
    uuid128() { value = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; };
    uuid128(QVector<quint8> v) { value = v; };
    bool operator==(const uuid128 &a) const { return (value == a.value); }
};

#endif // QTBLE_TYPES_H
