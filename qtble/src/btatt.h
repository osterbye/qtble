#ifndef BTATT_H
#define BTATT_H

#include <QObject>

struct bt_att;

class BtAtt : public QObject
{
    Q_OBJECT
public:
    explicit BtAtt(QObject *parent = nullptr);

    bool create(int fd);
    void close();
    bt_att *att();

signals:
    void clientDisconnected();

private:
    static void clientDisconnect(int err, void *user_data);

private:
    bt_att *m_att;
};

#endif // BTATT_H
