#include "btuuid.h"

BtUuid::BtUuid(QObject *parent) : QObject(parent) { }

int BtUuid::btUuid16Create(bt_uuid_t *btuuid, uint16_t value)
{
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID16;
    btuuid->value.u16 = value;

    return 0;
}

int BtUuid::btUuid32Create(bt_uuid_t *btuuid, uint32_t value)
{
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID32;
    btuuid->value.u32 = value;

    return 0;
}

int BtUuid::btUuid128Create(bt_uuid_t *btuuid, uint128_t value)
{
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID128;
    btuuid->value.u128 = value;

    return 0;
}

int BtUuid::btUuid128Create(bt_uuid_t *btuuid, QVector<quint8> value)
{
    if (value.size() != 16)
        return -1;

    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID128;
    for (int i = 0; i < 16; ++i)
        btuuid->value.u128.data[i] = value[i];

    return 0;
}
