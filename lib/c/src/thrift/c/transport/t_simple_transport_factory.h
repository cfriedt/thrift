#pragma once

#include <thrift/c/transport/t_transport_factory.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_simple_transport_factory {
  T_TRANSPORT_FACTORY_METHODS;
  struct t_transport* xport;
};

int t_simple_transport_factory_init(struct t_simple_transport_factory* factory);

#ifdef __cplusplus
}
#endif
