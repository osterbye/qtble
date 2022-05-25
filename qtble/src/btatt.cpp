#include "btatt.h"

// BlueZ shared local copy
extern "C" {
#include "src/shared/att.h"
}

#define ATT_DEBUG
#if defined ATT_DEBUG
#include <QDebug>
#define ATT_D(x) qDebug() << "[BtAtt] " << x
static void debugCallback(const char *str, void *user_data)
{
    Q_UNUSED(user_data)
    qDebug("[BtAtt] %s", str);
}
#else
#define ATT_D(x)
#endif

BtAtt::BtAtt(QObject *parent) : QObject(parent), m_att(nullptr) { }

bool BtAtt::create(int fd)
{
    if (m_att)
        return false;

    m_att = bt_att_new(fd, false);
    if (!m_att) {
        ATT_D("Failed to create new ATT");
        return false;
    }

    if (!bt_att_set_close_on_unref(m_att, true)) {
        ATT_D("Failed to set close on unref");
        bt_att_unref(m_att);
        return false;
    }

    if (!bt_att_register_disconnect(m_att, clientDisconnect, this, NULL)) {
        ATT_D("Failed to register disconnect handler");
        bt_att_unref(m_att);
        return false;
    }

#if defined ATT_DEBUG
    bt_att_set_debug(m_att, debugCallback, NULL, NULL);
#endif

    return true;
}

void BtAtt::close()
{
    if (m_att)
        bt_att_unref(m_att);

    m_att = nullptr;
}

bt_att *BtAtt::att()
{
    return m_att;
}

void BtAtt::clientDisconnect(int err, void *user_data)
{
    Q_UNUSED(err)
    emit((BtAtt *)user_data)->clientDisconnected();
}
