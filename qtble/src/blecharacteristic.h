#ifndef BLECHARACTERISTIC_H
#define BLECHARACTERISTIC_H

#include <QObject>

class QtBleAuth;
class QtBleValue;
class BtUuid;
struct bt_gatt_server;
struct gatt_db_attribute;

class BleCharacteristic : public QObject
{
    Q_OBJECT
public:
    BleCharacteristic(QtBleValue *value, QtBleAuth *auth, bt_gatt_server *server, BtUuid *btUuid,
                      gatt_db_attribute *service, QObject *parent = nullptr);

    /**
     * @brief This function is called when remote device wants to read an attribute value
     */
    static void read(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                     uint8_t opcode, struct bt_att *att, void *user_data);

    /**
     * @brief This function is called when remote device wants to update an attribute value
     */
    static void write(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                      const uint8_t *value, size_t len, uint8_t opcode, struct bt_att *att,
                      void *user_data);

    static void indicationCb(void *user_data);

    static void cccRead(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                        uint8_t opcode, struct bt_att *att, void *user_data);
    static void cccWrite(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                         const uint8_t *value, size_t len, uint8_t opcode, struct bt_att *att,
                         void *user_data);
private slots:
    void notify();

private:
    QtBleValue *p_value;
    QtBleAuth *p_auth;
    bt_gatt_server *p_gatt;
    quint16 m_handle;
    quint8 m_ccc;
};

#endif // BLECHARACTERISTIC_H
