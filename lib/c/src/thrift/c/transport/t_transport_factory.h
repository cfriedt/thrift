#pragma once

#include <stdbool.h>

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_TRANSPORT_FACTORY_METHODS                                                                \
  int (*get_transport)(struct t_transport_factory * factory, struct t_transport * xport,           \
                       struct t_transport * *new_xport);                                           \
  int (*put_transport)(struct t_transport_factory * factory, struct t_transport * xport)

struct t_transport_factory;
struct t_transport_factory {
  T_TRANSPORT_FACTORY_METHODS;
};

bool t_transport_factory_is_valid(struct t_transport_factory* f);

#ifdef __cplusplus
}
#endif
