#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_transport;

#define T_TRANSPORT_METHODS                                                                        \
  bool (*is_open)(struct t_transport * t);                                                         \
  bool (*peek)(struct t_transport * t);                                                            \
  int (*open)(struct t_transport * t);                                                             \
  int (*close)(struct t_transport * t);                                                            \
  int (*read)(struct t_transport * t, void* buf, uint32_t size);                                   \
  int (*read_all)(struct t_transport * t, void* buf, uint32_t size);                               \
  int (*write)(struct t_transport * t, const void* buf, uint32_t size);                            \
  int (*write_all)(struct t_transport * t, const void* buf, uint32_t size);                        \
  int (*flush)(struct t_transport * t);                                                            \
  int (*read_end)(struct t_transport * t);                                                         \
  int (*write_end)(struct t_transport * t);                                                        \
  int (*borrow)(struct t_transport * t, uint8_t * *buf, uint32_t * len);                           \
  int (*consume)(struct t_transport * t, uint32_t len);                                            \
  const char* (*get_origin)(struct t_transport * t);                                               \
  int (*available_read)(struct t_transport * t);                                                   \
  int (*available_write)(struct t_transport * t)

struct t_transport {
  T_TRANSPORT_METHODS;
};

bool t_transport_is_valid(struct t_transport* t);

#ifdef __cplusplus
}
#endif
