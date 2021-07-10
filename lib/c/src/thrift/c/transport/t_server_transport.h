#pragma once

#include <stdbool.h>

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_SERVER_TRANSPORT_METHODS                                                                 \
  int (*listen)(struct t_server_transport * t);                                                    \
  int (*accept)(struct t_server_transport * t, struct t_transport * *xport);                       \
  int (*close)(struct t_server_transport * t)

struct t_server_transport;
struct t_server_transport {
  T_SERVER_TRANSPORT_METHODS;
};

bool t_server_transport_is_valid(struct t_server_transport* t);

#ifdef __cplusplus
}
#endif
