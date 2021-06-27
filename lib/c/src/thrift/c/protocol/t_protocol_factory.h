#pragma once

#include <thrift/c/protocol/t_protocol.h>
#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_PROTOCOL_FACTORY_METHODS                                                                 \
  int (*get_protocol)(struct t_protocol_factory * factory, struct t_transport * transport,         \
                      struct t_protocol * *protocol);                                              \
  int (*put_protocol)(struct t_protocol_factory * factory, struct t_protocol * protocol)

struct t_protocol_factory;
struct t_protocol_factory {
  T_PROTOCOL_FACTORY_METHODS;
};

#ifdef __cplusplus
}
#endif
