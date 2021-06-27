#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <thrift/c/protocol/t_enum.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_byte_order {
  uint16_t (*to_wire16)(uint16_t x);
  uint32_t (*to_wire32)(uint32_t x);
  uint64_t (*to_wire64)(uint64_t x);
  uint16_t (*from_wire16)(uint16_t x);
  uint32_t (*from_wire32)(uint32_t x);
  uint64_t (*from_wire64)(uint64_t x);
};

extern const struct t_byte_order t_network_big_endian;
extern const struct t_byte_order t_network_little_endian;

bool t_byte_order_is_valid(const struct t_byte_order* bo);

// FIXME: these should use underscores instead of camelCase
#define T_PROTOCOL_METHODS                                                                         \
  int (*write_message_begin)(struct t_protocol * p, const char* name,                              \
                             enum t_message_type message_type, uint32_t seq);                      \
  int (*write_message_end)(struct t_protocol * p);                                                 \
  int (*write_struct_begin)(struct t_protocol * p, const char* name);                              \
  int (*write_struct_end)(struct t_protocol * p);                                                  \
  int (*write_field_begin)(struct t_protocol * p, const char* name, enum t_type field_type,        \
                           int field_id);                                                          \
  int (*write_field_end)(struct t_protocol * p);                                                   \
  int (*write_field_stop)(struct t_protocol * p);                                                  \
  int (*write_map_begin)(struct t_protocol * p, enum t_type ktype, enum t_type vtype,              \
                         uint32_t size);                                                           \
  int (*write_map_end)(struct t_protocol * p);                                                     \
  int (*write_list_begin)(struct t_protocol * p, enum t_type etype, uint32_t size);                \
  int (*write_list_end)(struct t_protocol * p);                                                    \
  int (*write_set_begin)(struct t_protocol * p, enum t_type etype, uint32_t size);                 \
  int (*write_set_end)(struct t_protocol * p);                                                     \
  int (*write_bool)(struct t_protocol * p, bool b);                                                \
  int (*write_byte)(struct t_protocol * p, uint8_t b);                                             \
  int (*write_i16)(struct t_protocol * p, int16_t s);                                              \
  int (*write_i32)(struct t_protocol * p, int32_t w);                                              \
  int (*write_i64)(struct t_protocol * p, int64_t l);                                              \
  int (*write_double)(struct t_protocol * p, double d);                                            \
  int (*write_string)(struct t_protocol * p, uint32_t size, const char* s);                        \
                                                                                                   \
  int (*read_message_begin)(struct t_protocol * p, uint32_t * len, char** name,                    \
                            enum t_message_type* message_type, uint32_t* seq);                     \
  int (*read_message_end)(struct t_protocol * p);                                                  \
  int (*read_struct_begin)(struct t_protocol * p, uint32_t * len, char** name);                    \
  int (*read_struct_end)(struct t_protocol * p);                                                   \
  int (*read_field_begin)(struct t_protocol * p, uint32_t * len, char** name,                      \
                          enum t_type* field_type, uint16_t* field_id);                            \
  int (*read_field_end)(struct t_protocol * p);                                                    \
  int (*read_map_begin)(struct t_protocol * p, enum t_type * ktype, enum t_type * vtype,           \
                        uint32_t * size);                                                          \
  int (*read_map_end)(struct t_protocol * p);                                                      \
  int (*read_list_begin)(struct t_protocol * p, enum t_type * etype, uint32_t * size);             \
  int (*read_list_end)(struct t_protocol * p);                                                     \
  int (*read_set_begin)(struct t_protocol * p, enum t_type * etype, uint32_t * size);              \
  int (*read_set_end)(struct t_protocol * p);                                                      \
  int (*read_bool)(struct t_protocol * p, bool* b);                                                \
  int (*read_byte)(struct t_protocol * p, uint8_t * b);                                            \
  int (*read_i16)(struct t_protocol * p, int16_t * s);                                             \
  int (*read_i32)(struct t_protocol * p, int32_t * w);                                             \
  int (*read_i64)(struct t_protocol * p, int64_t * l);                                             \
  int (*read_double)(struct t_protocol * p, double* d);                                            \
  int (*read_string)(struct t_protocol * p, uint32_t * size, char** s);

struct t_protocol;
struct t_protocol {
  T_PROTOCOL_METHODS;
};

bool t_protocol_is_valid(const struct t_protocol* p);

#ifdef __cplusplus
}
#endif
