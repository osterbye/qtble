#ifndef QTBLE_TYPES_H
#define QTBLE_TYPES_H

#include <QMetaType>
#include <QVector>

class QtBleAuth;
class QtBleValue;

enum GATT_SERVER_STATE {
    GATT_STATE_ERROR,
    GATT_STATE_OFF,
    GATT_STATE_STARTING,
    GATT_STATE_WAITING_FOR_CLIENT,
    GATT_STATE_CONNECTED,
    GATT_STATE_RESTARTING
};
Q_DECLARE_METATYPE(GATT_SERVER_STATE);

typedef struct {
    QVector<quint8> attributeUuid;
} uuid128;

/*typedef struct {
    uuid128 serviceUuid;
    QtBleAuth *authenticator;
    QVector<QtBleValue *> values;
} gattService;
Q_DECLARE_METATYPE(gattService);*/

const int uuidServiceIndex = 2;
const int uuidCharacteristicIndex = 3;

#endif // QTBLE_TYPES_H
