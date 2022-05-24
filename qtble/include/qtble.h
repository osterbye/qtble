#ifndef QTBLE_H
#define QTBLE_H
#include "qtble_global.h"
#include "qtble_types.h"
#include <QObject>

class QTBLE_EXPORT QtBle : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief QtBle
     * @param deviceName The name of your device.
     * @param deviceManufacturer The name of you or your company. Leave as an empty string if you
     * don't want your device to have a manufacturer characteristic.
     * @param deviceAppearance Device appearance value. Look for valid values on
     * https://www.bluetooth.com/specifications/assigned-numbers/ or set to 0 if you don't want your
     * device to have an appearance characteristic.
     * @param parent Qt parent, for easy garbage collection.
     */
    QtBle(QString deviceName, QString deviceManufacturer = "", quint16 deviceAppearance = 0,
          QObject *parent = nullptr);

    /**
     * @brief Create a GATT service
     * @param uuid Service UUID
     * @param authenticator Pointer to your authentication class or leave as nullptr to always allow
     * remote devices to access this service.
     * @return Service index. Must be used when adding characteristics to the service.
     */
    quint8 createGattService(uuid128 uuid, QtBleAuth *authenticator = nullptr);

    /**
     * @brief Add a characteristic to a specific service
     * @param serviceIndex The value returned from \a addGattService, of the service that you want
     * to add the characteristic to.
     * @param value Your class containing the value that you would like the characteristic to use.
     * @return True if successful. False if service index is invalid.
     */
    bool addCharacteristicToService(quint8 serviceIndex, QtBleValue *value);

    /**
     * @brief Can only be called when GATT server is stopped.
     */
    void clearGattServices();

    /**
     * @brief Get state of GATT server
     * @return GATT server state
     */
    GATT_SERVER_STATE stateGatt();

    /**
     * @brief Start GATT server. Remember to create services and characteristics before calling
     * this.
     */
    void startGattServer();

    /**
     * @brief Stop GATT server. This does not clear services and characteristics.
     */
    void stopGattServer();

signals:
    /**
     * @brief GATT server state notification. Connect to this signal to receive notifications
     * whenever the GATT server state changes.
     * @param state GATT server state
     */
    void gattStateChanged(GATT_SERVER_STATE state);
};

#endif // QTBLE_H
