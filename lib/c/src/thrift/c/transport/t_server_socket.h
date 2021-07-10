#pragma once

#include <stdbool.h>

#include <thrift/c/transport/t_server_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_server_socket {
  T_SERVER_TRANSPORT_METHODS;
  union {
    const char* addr;
    const char* path;
  };
  int sd;
  uint16_t port;
  uint16_t backlog;
  int cancel[2];
};

int t_server_socket_init(struct t_server_socket* t, const char* addr, uint16_t port);
int t_server_socket_init_port(struct t_server_socket* t, uint16_t port);
int t_server_socket_init_path(struct t_server_socket* t, const char* path);

#ifdef __cplusplus
}
#endif
