#pragma once

#include <thrift/c/transport/t_transport.h>

#define T_TRANSPORT_FACTORY_METHODS                                                                \
  int (*get_transport)(struct t_transport_factory * factory, struct t_transport * transport,       \
                       struct t_transport * *new_transport);                                       \
  int (*put_transport)(struct t_transport_factory * factory, struct t_transport * transport)

struct t_transport_factory;
struct t_transport_factory {
  T_TRANSPORT_FACTORY_METHODS;
};
