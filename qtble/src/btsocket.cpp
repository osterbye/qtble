#include "btsocket.h"
#include <qplatformdefs.h>

// BlueZ
extern "C" {
#include <src/shared/bluetooth-definitions.h>
#include <src/shared/l2cap-definitions.h>
}

#define BS_DEBUG
#if defined BS_DEBUG
#include <QDebug>
#define BS_D(x) qDebug() << "[BtSocket] " << x
#define BS_ADDR_D(b) qDebug("[BtSocket] %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X", b[5], b[4], b[3], b[2], b[1], b[0])
#else
#define BS_D(x)
#define BS_ADDR_D(b)
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le16_to_cpu(val) (val)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define le16_to_cpu(val) bswap_16(val)
#else
#error "Unknown byte order"
#endif

#define ATT_CID 4
const bdaddr_t bd_any = { { 0, 0, 0, 0, 0, 0 } };

BtSocket::BtSocket(QObject *parent) : QObject(parent)
{
    if (pipe(m_pipe) < 0)
        BS_D("Failed to create pipe");
}

BtSocket::~BtSocket()
{
    abortWait();
}

int BtSocket::waitForClient()
{
    BS_D(__PRETTY_FUNCTION__);

    int l2capSocket = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (l2capSocket < 0) {
        BS_D("Failed to open L2CAP socket");
        return -1;
    }

    struct sockaddr_l2 srcaddr;
    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.l2_family = AF_BLUETOOTH;
    srcaddr.l2_cid = le16_to_cpu(ATT_CID);
    srcaddr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
    memcpy(&srcaddr.l2_bdaddr, &bd_any, sizeof(bdaddr_t));

    if (bind(l2capSocket, (struct sockaddr *)&srcaddr, sizeof(srcaddr)) < 0) {
        BS_D("Failed to bind L2CAP socket");
        QT_CLOSE(l2capSocket);
        return -1;
    }

    struct bt_security btsec;
    memset(&btsec, 0, sizeof(btsec));
    btsec.level = BT_SECURITY_LOW;
    if (setsockopt(l2capSocket, SOL_BLUETOOTH, BT_SECURITY, &btsec, sizeof(btsec)) != 0) {
        BS_D("Failed to set L2CAP security level");
        QT_CLOSE(l2capSocket);
        return -1;
    }

    if (listen(l2capSocket, 1) < 0) {
        BS_D("Failed to listen on L2CAP socket");
        QT_CLOSE(l2capSocket);
        return -1;
    }

    BS_D("Waiting for connections");

    fd_set active_fd_set, read_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(l2capSocket, &active_fd_set);
    FD_SET(m_pipe[0], &active_fd_set);

    read_fd_set = active_fd_set;
    int r = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
    if (r < 0) {
        BS_D("Select failed");
        QT_CLOSE(l2capSocket);
        return -1;
    }
    BS_D("l2capSocket = " << l2capSocket);
    BS_D("pipefd[0] = " << m_pipe[0]);

    if (FD_ISSET(m_pipe[0], &read_fd_set)) {
        BS_D("Select aborted");
        char buf;
        read(m_pipe[0], &buf, 1);
        QT_CLOSE(l2capSocket);
        return -1;
    }

    struct sockaddr_l2 addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t optlen = sizeof(addr);
    int client = accept(l2capSocket, (struct sockaddr *)&addr, &optlen);
    if (client < 0) {
        BS_D("Accept failed");
        QT_CLOSE(l2capSocket);
        return -1;
    }

    BS_D("Connect from: ");
    BS_ADDR_D(addr.l2_bdaddr.b);

    if (l2capSocket >= 0)
        QT_CLOSE(l2capSocket);

    return client;
}

void BtSocket::abortWait()
{
    BS_D(__PRETTY_FUNCTION__);
    BS_D("pipefd[0] = " << m_pipe[0]);
    BS_D("pipefd[1] = " << m_pipe[1]);

    if (m_pipe[1] > 0)
        QT_WRITE(m_pipe[1], "q", 1);
}
