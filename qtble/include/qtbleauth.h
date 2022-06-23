#ifndef QTBLEAUTH_H
#define QTBLEAUTH_H
#include <QObject>

/**
 * @brief This class handles user level authentication. It is not related to BLE protocol level
 * authentication/authorization. Inherit from this class, if you want to handle user level
 * authentication.
 */
class QtBleAuth : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief This class handles user level authentication. It is not related to BLE protocol level
     * authentication/authorization. Inherit from this class, if you want to handle user level
     * authentication.
     * @param authorized Default value. If you don't need authorization for your service, just
     * leave the value at True.
     * @param parent Qt parent, for easy garbage collection.
     */
    QtBleAuth(bool authorized = true, QObject *parent = nullptr)
        : QObject(parent), m_authorized(authorized)
    {
    }

    bool isAuthorized() { return m_authorized; }

protected:
    void setAuthorized(bool authorized) { m_authorized = authorized; }

private:
    bool m_authorized;
};

#endif // QTBLEAUTH_H
