#ifndef QTBLEVALUE_H
#define QTBLEVALUE_H
#include "qtble_types.h"
#include <QObject>

class QTimer;

/**
 * @brief Inherit from this class to create \a characteristic. Override the virtual functions to
 * react to actions made by the remote device.
 */
class QtBleValue : public QObject
{
    Q_OBJECT

public:
    enum ACCESS { PERM_READ, PERM_WRITE, PERM_READ_AND_WRITE };
    enum NOTIFICATIONS { NOTI_NONE, NOTI_INDICATE, NOTI_NOTIFY, NOTI_INDICATE_AND_NOTIFY };

    /**
     * @brief Inherit from this class to control the value of a \a characteristic.
     * @param uuid The UUID of this characteristic. Must be unique within all the services on this
     * device.
     * @param permission Access permissions for a remote device.
     * @param notification Supported \a indication/notification for this \a characteristic.
     * @param value The default value of this \a characteristic.
     * @param parent Qt parent, for easy garbage collection.
     */
    QtBleValue(uuid128 uuid, ACCESS permission, NOTIFICATIONS notification,
               QObject *parent = nullptr);

    QtBleValue(uuid128 uuid, ACCESS permission, NOTIFICATIONS notification, QByteArray value,
               QObject *parent = nullptr);

    /**
     * @brief Call this function to update the value of this \a characteristic. If \a indications or
     * \a notifications are enabled, for this \a characteristic, they will be handled automatically.
     * @param value The new value of this \a characteristic.
     */
    void updateValue(QByteArray value);

protected:
    /**
     * @brief This function will be called when a remote device acknowledges an \a indication.
     */
    virtual void indicationReply() {};
    /**
     * @brief This function will be called when a remote device has performed a \a write action on
     * this value.
     */
    virtual void remoteValueUpdate() {};
    /**
     * @brief Call this function to read the current \a characteristic value.
     * @return Current \a characteristic value.
     */
    QByteArray getValue();

private:
    QByteArray read();
    void write(QByteArray value, quint16 offset);
    uuid128 getUuid();
    ACCESS accessPermissions();
    NOTIFICATIONS notificationPermissions();

private slots:
    void performWrite();

signals:
    void notify(QPrivateSignal);

private:
    uuid128 m_uuid;
    ACCESS m_accessPermissions;
    NOTIFICATIONS m_notificationPermissions;
    QByteArray m_value;
    QByteArray m_newValue;
    QTimer *m_writeTimer;

    friend class BleCharacteristic;
};

#endif // QTBLEVALUE_H
