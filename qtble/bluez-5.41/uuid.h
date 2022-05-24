#ifndef UUID_H
#define UUID_H

#include <bluetooth/bluetooth.h>
#include "uuid-types.h"

int bt_uuid16_create(bt_uuid_t *btuuid, uint16_t value) {
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID16;
    btuuid->value.u16 = value;

    return 0;
}

int bt_uuid32_create(bt_uuid_t *btuuid, uint32_t value)
{
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID32;
    btuuid->value.u32 = value;

    return 0;
}

int bt_uuid128_create(bt_uuid_t *btuuid, uint128_t value)
{
    memset(btuuid, 0, sizeof(bt_uuid_t));
    btuuid->type = bt_uuid_t::BT_UUID128;
    btuuid->value.u128 = value;

    return 0;
}

static uint128_t bluetooth_base_uuid = {
    .data = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
            0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
};

#define BASE_UUID16_OFFSET	2
#define BASE_UUID32_OFFSET	0

static void bt_uuid16_to_uuid128(const bt_uuid_t *src, bt_uuid_t *dst)
{
    uint16_t be16;

    dst->value.u128 = bluetooth_base_uuid;
    dst->type = bt_uuid_t::BT_UUID128;

    /*
     * No matter the system: 128-bit UUIDs should be stored
     * as big-endian. 16-bit UUIDs are stored on host order.
     */

    be16 = htons(src->value.u16);
    memcpy(&dst->value.u128.data[BASE_UUID16_OFFSET], &be16, sizeof(be16));
}

static void bt_uuid32_to_uuid128(const bt_uuid_t *src, bt_uuid_t *dst)
{
    uint32_t be32;

    dst->value.u128 = bluetooth_base_uuid;
    dst->type = bt_uuid_t::BT_UUID128;

    /*
     * No matter the system: 128-bit UUIDs should be stored
     * as big-endian. 32-bit UUIDs are stored on host order.
     */

    be32 = htonl(src->value.u32);
    memcpy(&dst->value.u128.data[BASE_UUID32_OFFSET], &be32, sizeof(be32));
}

void bt_uuid_to_uuid128(const bt_uuid_t *src, bt_uuid_t *dst)
{
    switch (src->type) {
    case bt_uuid_t::BT_UUID128:
        *dst = *src;
        break;
    case bt_uuid_t::BT_UUID32:
        bt_uuid32_to_uuid128(src, dst);
        break;
    case bt_uuid_t::BT_UUID16:
        bt_uuid16_to_uuid128(src, dst);
        break;
    case bt_uuid_t::BT_UUID_UNSPEC:
    default:
        break;
    }
}

static int bt_uuid128_cmp(const bt_uuid_t *u1, const bt_uuid_t *u2)
{
    return memcmp(&u1->value.u128, &u2->value.u128, sizeof(uint128_t));
}

int bt_uuid_cmp(const bt_uuid_t *uuid1, const bt_uuid_t *uuid2)
{
    bt_uuid_t u1, u2;

    bt_uuid_to_uuid128(uuid1, &u1);
    bt_uuid_to_uuid128(uuid2, &u2);

    return bt_uuid128_cmp(&u1, &u2);
}


int bt_uuid_to_le(const bt_uuid_t *src, void *dst)
{
    bt_uuid_t uuid;

    switch (src->type) {
    case bt_uuid_t::BT_UUID16:
        bt_put_le16(src->value.u16, dst);
        return 0;
    case bt_uuid_t::BT_UUID32:
        bt_uuid32_to_uuid128(src, &uuid);
        src = &uuid;
        /* Fallthrough */
    case bt_uuid_t::BT_UUID128:
        /* Convert from 128-bit BE to LE */
        bswap_128(&src->value.u128, dst);
        return 0;
    case bt_uuid_t::BT_UUID_UNSPEC:
    default:
        return -EINVAL;
    }
}

static inline int bt_uuid_len(const bt_uuid_t *uuid)
{
    return uuid->type / 8;
}

#endif /* UUID_H */
