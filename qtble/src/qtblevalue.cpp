#include "qtblevalue.h"
#include <QTimer>

QtBleValue::QtBleValue(uuid128 uuid, ACCESS permission, NOTIFICATIONS notification, QObject *parent)
    : QObject(parent),
      m_uuid(uuid),
      m_accessPermissions(permission),
      m_notificationPermissions(notification),
      m_value("")
{
    m_writeTimer = new QTimer(this);
    m_writeTimer->setSingleShot(true);
    m_writeTimer->setInterval(500);
    connect(m_writeTimer, &QTimer::timeout, this, &QtBleValue::performWrite);
}

QtBleValue::QtBleValue(uuid128 uuid, ACCESS permission, NOTIFICATIONS notification,
                       QByteArray value, QObject *parent)
    : QObject(parent),
      m_uuid(uuid),
      m_accessPermissions(permission),
      m_notificationPermissions(notification),
      m_value(value)
{
    m_writeTimer = new QTimer(this);
    m_writeTimer->setSingleShot(true);
    m_writeTimer->setInterval(500);
    connect(m_writeTimer, &QTimer::timeout, this, &QtBleValue::performWrite);
}

void QtBleValue::updateValue(QByteArray value)
{
    m_value = value;
    if (NOTI_NONE != m_notificationPermissions)
        emit notify(QPrivateSignal());
}

QByteArray QtBleValue::getValue()
{
    return m_value;
}

QByteArray QtBleValue::read()
{
    if (PERM_READ != m_accessPermissions && PERM_READ_AND_WRITE != m_accessPermissions)
        return "";

    return m_value;
}

void QtBleValue::write(QByteArray value, quint16 offset)
{
    if (PERM_WRITE != m_accessPermissions && PERM_READ_AND_WRITE != m_accessPermissions)
        return;

    if (0 == offset && m_newValue.isEmpty()) {
        m_newValue = value;
    } else {
        quint32 totalLength = offset + value.length();
        m_newValue = m_newValue.leftJustified(totalLength, 0x00);
        m_newValue.replace(offset, value.length(), value);
    }
    m_writeTimer->start();
}

uuid128 QtBleValue::getUuid()
{
    return m_uuid;
}

QtBleValue::ACCESS QtBleValue::accessPermissions()
{
    return m_accessPermissions;
}

QtBleValue::NOTIFICATIONS QtBleValue::notificationPermissions()
{
    return m_notificationPermissions;
}

void QtBleValue::performWrite()
{
    m_value = m_newValue;
    m_newValue.clear();
    remoteValueUpdate();
}
