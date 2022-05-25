#ifndef GATTSERVICE_H
#define GATTSERVICE_H

#include "qtble_types.h"
#include <QVector>

class QtBleAuth;
class QtBleValue;

typedef struct {
    uuid128 serviceUuid;
    QtBleAuth *authenticator;
    QVector<QtBleValue *> values;
} gattService;

#endif // GATTSERVICE_H
