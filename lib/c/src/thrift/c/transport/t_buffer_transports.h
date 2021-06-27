#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_BUFFER_TRANSPORT_FIELDS                                                                  \
  bool open_;                                                                                      \
  uint8_t* buffer_;                                                                                \
  size_t bufferSize_;                                                                              \
  uint8_t* rBase_;                                                                                 \
  uint8_t* rBound_;                                                                                \
  uint8_t* wBase_;                                                                                 \
  uint8_t* wBound_

struct t_memory_buffer {
  T_TRANSPORT_METHODS;
  T_BUFFER_TRANSPORT_FIELDS;
};

int t_memory_buffer_init(struct t_memory_buffer* t, void* buffer, uint32_t size);

struct t_buffered_transport {
  T_TRANSPORT_METHODS;
  T_BUFFER_TRANSPORT_FIELDS;
  struct t_transport* trans;
};

int t_buffered_transport_init(struct t_buffered_transport* t,
                              void* buffer,
                              uint32_t size,
                              struct t_transport* trans);

#ifdef __cplusplus
}
#endif
