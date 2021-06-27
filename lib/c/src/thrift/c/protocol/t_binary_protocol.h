#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <thrift/c/protocol/t_protocol.h>
#include <thrift/c/protocol/t_protocol_factory.h>
#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_binary_protocol {
  T_PROTOCOL_METHODS;

  struct t_transport* trans;
  const struct t_byte_order* byte_order;
};

int t_binary_protocol_init(struct t_binary_protocol* protocol,
                           struct t_transport* transport,
                           const struct t_byte_order* byte_order);

struct t_binary_protocol_factory {
  T_PROTOCOL_FACTORY_METHODS;
  const struct t_byte_order* byte_order;
};

int t_binary_protocol_factory_init_with_byte_order(struct t_binary_protocol_factory* factory,
                                                   const struct t_byte_order* byte_order);
int t_binary_protocol_factory_init(struct t_binary_protocol_factory* factory);

#ifdef __cplusplus
}
#endif
