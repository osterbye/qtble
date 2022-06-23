#ifndef BLESERVICE_H
#define BLESERVICE_H

#include <QObject>
#include <QVector>
#include "gattservice.h"

class BleCharacteristic;
class BtUuid;
struct bt_gatt_server;
struct gatt_db;

class BleService : public QObject
{
    Q_OBJECT
public:
    explicit BleService(bt_gatt_server *server, gatt_db *gattDb, BtUuid *btUuid, gattService *srvc,
                        QObject *parent = nullptr);

private:
    QVector<BleCharacteristic *> m_characteristics;
};

#endif // BLESERVICE_H
