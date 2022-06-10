#ifndef QTBLE_H
#define QTBLE_H
#include "qtble_global.h"
#include "qtble_types.h"
#include <QObject>

class QtBleAuth;
class QtBleValue;
class GattServer;

class QTBLE_EXPORT QtBle : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief QtBle
     * @param deviceName The default name of your device. If a storage path is provided remote
     * devices will be allowed to change the device name.
     * @param storagePath If you want to allow remote devices to change the name if your device,
     * supply a path to a valid location where it can be stored. The name will be stored in a plain
     * text file.
     * @param deviceManufacturer The name of you or your company. Leave as an empty string if you
     * don't want your device to have a manufacturer characteristic.
     * @param deviceAppearance Device appearance value. Look for valid values on
     * https://www.bluetooth.com/specifications/assigned-numbers/ or set to 0 if you don't want your
     * device to have an appearance characteristic.
     * @param parent Qt parent, for easy garbage collection.
     */
    QtBle(QString deviceName, QString storagePath = "", QString deviceManufacturer = "",
          quint16 deviceAppearance = 0, QObject *parent = nullptr);

    /**
     * @brief Create a GATT service. New services will only be visible if created before server is
     * started.
     * @param uuid Service UUID
     * @param authenticator Pointer to your authentication class or leave as nullptr to always allow
     * remote devices to access this service.
     * @return True if successful. False if another service with the supplied UUID already exist.
     */
    bool createGattService(uuid128 uuid, QtBleAuth *authenticator = nullptr);

    /**
     * @brief Add a characteristic to a specific service. New characteristics will only be visible
     * if created before server is started.
     * @param serviceUuid The UUID of the service that you wish to add the characteristic to.
     * @param value Your class containing the value that you would like the characteristic to use.
     * @return True if successful. False if no service with the supplied UUID exists.
     */
    bool addCharacteristicToService(uuid128 serviceUuid, QtBleValue *value);

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

private:
    GattServer *m_gattServer;
};

#endif // QTBLE_H
