#ifndef UUID_TYPES_H
#define UUID_TYPES_H

#ifndef __BLUETOOTH_H
typedef struct {
    uint8_t data[16];
} uint128_t;
#endif

typedef struct {
    enum {
        BT_UUID_UNSPEC = 0,
        BT_UUID16 = 16,
        BT_UUID32 = 32,
        BT_UUID128 = 128,
    } type;
    union {
        uint16_t  u16;
        uint32_t  u32;
        uint128_t u128;
    } value;
} bt_uuid_t;

#endif /* UUID_TYPES_H */
