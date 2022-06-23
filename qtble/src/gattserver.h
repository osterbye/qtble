#ifndef GATTSERVER_H
#define GATTSERVER_H
#include "qtble_types.h"
#include "gattservice.h"
#include <QObject>
#include <QString>
#include <QVector>

class Advertising;
class BleCharacteristic;
class BleService;
class BtAtt;
class BtSocket;
class BtUuid;
class GapService;
class QtBleAuth;
class QtBleValue;

struct gatt_db;
struct bt_gatt_server;

class GattServer : public QObject
{
    Q_OBJECT
public:
    explicit GattServer(QString deviceName, QString storagePath, QString deviceManufacturer,
                        quint16 deviceAppearance, QObject *parent = nullptr);
    ~GattServer();

    bool createService(uuid128 uuid, QtBleAuth *authenticator);
    bool addCharacteristicToService(uuid128 serviceUuid, QtBleValue *value);
    void clearServices();
    GATT_SERVER_STATE state();

public slots:
    void start();
    void stop();

signals:
    void gattStateChanged(GATT_SERVER_STATE state);

private slots:
    void clientDisconnected();

private:
    void startService();
    void stopService();
    bool createServer();
    void destroyServer();
    void createCustomServices();
    void setState(GATT_SERVER_STATE state);

private:
    Advertising *m_advertising;
    BtAtt *m_btAtt;
    BtSocket *m_btSocket;
    BtUuid *m_btUuid;
    GATT_SERVER_STATE m_state;
    int m_fdClient;
    gatt_db *m_gattDb;
    bt_gatt_server *m_gatt;
    bool m_startRequested;
    QVector<gattService *> m_services;
    QVector<BleCharacteristic *> m_characteristics;
    QVector<BleService *> m_customServices;
    GapService *m_gapService;
    QtBleAuth *m_authAlwaysAllowed;
};

#endif // GATTSERVER_H
