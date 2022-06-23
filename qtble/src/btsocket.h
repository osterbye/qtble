#ifndef BTSOCKET_H
#define BTSOCKET_H

#include <QObject>

class BtSocket : public QObject
{
    Q_OBJECT
public:
    explicit BtSocket(QObject *parent = nullptr);
    ~BtSocket();

    int waitForClient();
    void abortWait();

private:
    int m_pipe[2];
};

#endif // BTSOCKET_H
