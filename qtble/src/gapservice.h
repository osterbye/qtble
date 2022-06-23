#ifndef GAPSERVICE_H
#define GAPSERVICE_H

#include <QObject>

class BtUuid;
struct bt_att;
struct gatt_db;
struct gatt_db_attribute;

class GapService : public QObject
{
    Q_OBJECT
public:
    explicit GapService(QString deviceName, QString storagePath, QString deviceManufacturer,
                        quint16 deviceAppearance, QObject *parent = nullptr);

    void create(gatt_db *gattDb, BtUuid *btUuid);
    const QString &deviceName();

private:
    void saveNewDeviceName(QString name);
    static void confirmAttrWrite(struct gatt_db_attribute *attr, int err, void *user_data);
    static void readName(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                         quint8 opcode, struct bt_att *att, void *user_data);
    static void writeName(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                          const quint8 *value, size_t len, quint8 opcode, struct bt_att *att,
                          void *user_data);
    static void readNameExtProps(struct gatt_db_attribute *attrib, unsigned int id, quint16 offset,
                                 quint8 opcode, struct bt_att *att, void *user_data);

private:
    QString m_deviceName;
    QString m_storagePath;
    QString m_deviceManufacturer;
    quint16 m_deviceAppearance;
};

#endif // GAPSERVICE_H
