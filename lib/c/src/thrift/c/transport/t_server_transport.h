#pragma once

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_server_transport;
struct t_server_transport {
  int (*listen)(struct t_server_transport* t);
  int (*accept)(struct t_server_transport* t, struct t_transport** xport);
  int (*close)(struct t_server_transport* t);
};

#ifdef __cplusplus
}
#endif
