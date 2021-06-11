#include <stdint.h>

#include "t_protocol.h"

#define IS_LE() (*(uint8_t *)&(uint16_t){1})

#define bswap_16(x) \
( \
    0 \
    | ((x >> 8) & 0x00ff) \
    | ((x << 8) & 0xff00) \
)

#define bswap_32(x) \
( \
    0 \
    | ((x >> 24) & 0x000000ff) \
    | ((x >>  8) & 0x0000ff00) \
    | ((x <<  8) & 0x00ff0000) \
    | ((x << 24) & 0xff000000) \
)

#define bswap_64(x) \
( \
    0 \
    | ((x >> 56) & 0x00000000000000ff) \
    | ((x >> 40) & 0x000000000000ff00) \
    | ((x >> 24) & 0x0000000000ff0000) \
    | ((x >>  8) & 0x00000000ff000000) \
    | ((x <<  8) & 0x000000ff00000000) \
    | ((x << 24) & 0x0000ff0000000000) \
    | ((x << 40) & 0x00ff000000000000) \
    | ((x << 56) & 0xff00000000000000) \
)

static uint16_t t_network_big_endian_toWire16(uint16_t x)
{
    if (IS_LE()) {
        x = bswap_16(x);
    }

    return x;
}

static uint32_t t_network_big_endian_toWire32(uint32_t x)
{
    if (IS_LE()) {
        x = bswap_32(x);
    }

    return x;
}

static uint64_t t_network_big_endian_toWire64(uint64_t x)
{
    if (IS_LE()) {
        x = bswap_64(x);
    }

    return x;
}

const struct t_byte_order t_network_big_endian =
{
    .toWire16 = t_network_big_endian_toWire16,
    .toWire32 = t_network_big_endian_toWire32,
    .toWire64 = t_network_big_endian_toWire64,
    .fromWire16 = t_network_big_endian_toWire16,
    .fromWire32 = t_network_big_endian_toWire32,
    .fromWire64 = t_network_big_endian_toWire64,
};

static uint16_t t_network_little_endian_toWire16(uint16_t x)
{
    if (!IS_LE()) {
        x = bswap_16(x);
    }

    return x;
}

static uint32_t t_network_little_endian_toWire32(uint32_t x)
{
    if (!IS_LE()) {
        x = bswap_32(x);
    }

    return x;
}

static uint64_t t_network_little_endian_toWire64(uint64_t x)
{
    if (!IS_LE()) {
        x = bswap_64(x);
    }

    return x;
}

const struct t_byte_order t_network_little_endian =
{
    .toWire16 = t_network_little_endian_toWire16,
    .toWire32 = t_network_little_endian_toWire32,
    .toWire64 = t_network_little_endian_toWire64,
    .fromWire16 = t_network_little_endian_toWire16,
    .fromWire32 = t_network_little_endian_toWire32,
    .fromWire64 = t_network_little_endian_toWire64,
};
