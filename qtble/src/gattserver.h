#ifndef GATTSERVER_H
#define GATTSERVER_H
#include "qtble_types.h"
#include "gattservice.h"
#include <QObject>
#include <QString>
#include <QVector>

class BleCharacteristic;
class BleService;
class Advertising;
class BtAtt;
class BtSocket;
class BtUuid;
class QtBleAuth;
class QtBleValue;

struct gatt_db;
struct bt_gatt_server;

class GattServer : public QObject
{
    Q_OBJECT
public:
    explicit GattServer(QString deviceName, QString deviceManufacturer, quint16 deviceAppearance,
                        QObject *parent = nullptr);
    ~GattServer();

    bool createService(uuid128 uuid, QtBleAuth *authenticator);
    bool addCharacteristicToService(uuid128 serviceUuid, QtBleValue *value);
    void clearServices();
    GATT_SERVER_STATE state();
    void start();
    void stop();
    // void addGattService(gattService *service);

signals:
    void gattStateChanged(GATT_SERVER_STATE state);

    // public slots:
    // void btStateChanged(BT_STATE state);

private slots:
    void startService();
    void stopService();
    void clientDisconnected();

private:
    bool createServer();
    void destroyServer();
    void createGapService();
    void createCustomServices();
    void setState(GATT_SERVER_STATE state);
    static void confirm_write(struct gatt_db_attribute *attr, int err, void *user_data);
    /*static void gap_device_name_read_cb(struct gatt_db_attribute *attrib, unsigned int id,
                                        uint16_t offset, uint8_t opcode, struct bt_att *att,
                                        void *user_data);
    static void gap_device_name_ext_prop_read_cb(struct gatt_db_attribute *attrib, unsigned int id,
                                                 uint16_t offset, uint8_t opcode,
                                                 struct bt_att *att, void *user_data);*/

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
    QString m_deviceName;
    QString m_deviceManufacturer;
    quint16 m_deviceAppearance;
    QtBleAuth *m_authAlwaysAllowed;

    // static QString m_deviceName;
};

#endif // GATTSERVER_H
