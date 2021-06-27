#include <stdint.h>

#include "t_protocol.h"

#define IS_LE() (*(uint8_t*)&(uint16_t){1})

#define bswap_16(x) (0 | ((x >> 8) & 0x00ff) | ((x << 8) & 0xff00))

#define bswap_32(x)                                                                                \
  (((x >> 24) & 0x000000ff) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000)                    \
   | ((x << 24) & 0xff000000))

#define bswap_64(x)                                                                                \
  (((x >> 56) & 0x00000000000000ff) | ((x >> 40) & 0x000000000000ff00)                             \
   | ((x >> 24) & 0x0000000000ff0000) | ((x >> 8) & 0x00000000ff000000)                            \
   | ((x << 8) & 0x000000ff00000000) | ((x << 24) & 0x0000ff0000000000)                            \
   | ((x << 40) & 0x00ff000000000000) | ((x << 56) & 0xff00000000000000))

static uint16_t t_network_big_endian_to_wire16(uint16_t x) {
  if (IS_LE()) {
    x = bswap_16(x);
  }

  return x;
}

static uint32_t t_network_big_endian_to_wire32(uint32_t x) {
  if (IS_LE()) {
    x = bswap_32(x);
  }

  return x;
}

static uint64_t t_network_big_endian_to_wire64(uint64_t x) {
  if (IS_LE()) {
    x = bswap_64(x);
  }

  return x;
}

const struct t_byte_order t_network_big_endian = {
    .to_wire16 = t_network_big_endian_to_wire16,
    .to_wire32 = t_network_big_endian_to_wire32,
    .to_wire64 = t_network_big_endian_to_wire64,
    .from_wire16 = t_network_big_endian_to_wire16,
    .from_wire32 = t_network_big_endian_to_wire32,
    .from_wire64 = t_network_big_endian_to_wire64,
};

static uint16_t t_network_little_endian_to_wire16(uint16_t x) {
  if (!IS_LE()) {
    x = bswap_16(x);
  }

  return x;
}

static uint32_t t_network_little_endian_to_wire32(uint32_t x) {
  if (!IS_LE()) {
    x = bswap_32(x);
  }

  return x;
}

static uint64_t t_network_little_endian_to_wire64(uint64_t x) {
  if (!IS_LE()) {
    x = bswap_64(x);
  }

  return x;
}

const struct t_byte_order t_network_little_endian = {
    .to_wire16 = t_network_little_endian_to_wire16,
    .to_wire32 = t_network_little_endian_to_wire32,
    .to_wire64 = t_network_little_endian_to_wire64,
    .from_wire16 = t_network_little_endian_to_wire16,
    .from_wire32 = t_network_little_endian_to_wire32,
    .from_wire64 = t_network_little_endian_to_wire64,
};

bool t_byte_order_is_valid(const struct t_byte_order* bo) {
  return !(bo == NULL || bo->to_wire16 == NULL || bo->to_wire32 == NULL || bo->to_wire64 == NULL
           || bo->from_wire16 == NULL || bo->from_wire32 == NULL || bo->from_wire64 == NULL);
}

bool t_protocol_is_valid(const struct t_protocol* p) {
  return !(p == NULL || p->write_message_begin == NULL || p->write_message_end == NULL
           || p->write_struct_begin == NULL || p->write_struct_end == NULL
           || p->write_field_begin == NULL || p->write_field_end == NULL
           || p->write_field_stop == NULL || p->write_map_begin == NULL || p->write_map_end == NULL
           || p->write_list_begin == NULL || p->write_list_end == NULL || p->write_set_begin == NULL
           || p->write_set_end == NULL || p->write_bool == NULL || p->write_byte == NULL
           || p->write_i16 == NULL || p->write_i32 == NULL || p->write_i64 == NULL
           || p->write_double == NULL || p->write_string == NULL || p->read_message_begin == NULL
           || p->read_message_end == NULL || p->read_struct_begin == NULL
           || p->read_struct_end == NULL || p->read_field_begin == NULL || p->read_field_end == NULL
           || p->read_map_begin == NULL || p->read_map_end == NULL || p->read_list_begin == NULL
           || p->read_list_end == NULL || p->read_set_begin == NULL || p->read_set_end == NULL
           || p->read_bool == NULL || p->read_byte == NULL || p->read_i16 == NULL
           || p->read_i32 == NULL || p->read_i64 == NULL || p->read_double == NULL
           || p->read_string == NULL);
}
