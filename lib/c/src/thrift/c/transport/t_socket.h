#pragma once

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THRIFT_INVALID_SOCKET (-1)

struct t_socket {
  T_TRANSPORT_METHODS;
  union {
    const char* host;
    const char* path;
  };
  uint16_t port;
  int sd;
};

int t_socket_init(struct t_socket* transport, const char* host, uint16_t port);
int t_socket_init_path(struct t_socket* transport, const char* path);

#ifdef __cplusplus
}
#endif
