#pragma once

#include <thrift/c/processor/t_processor.h>
#include <thrift/c/protocol/t_protocol.h>
#include <thrift/c/transport/t_server_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_SERVER_METHODS                                                                           \
  int (*serve)(struct t_server * server);                                                          \
  void (*stop)(struct t_server * server)

#define T_SERVER_FIELDS                                                                            \
  struct t_processor* processor;                                                                   \
  struct t_server_transport* server_transport;                                                     \
  struct t_transport_factory* input_transport_factory;                                             \
  struct t_transport_factory* output_transport_factory;                                            \
  struct t_protocol_factory* input_protocol_factory;                                               \
  struct t_protocol_factory* output_protocol_factory

struct t_server {
  T_SERVER_METHODS;
  T_SERVER_FIELDS;
};

static inline bool t_server_is_valid(struct t_server* server) {
  return !(server->serve == NULL || server->stop == NULL || server->processor == NULL
           || server->server_transport == NULL || server->input_transport_factory == NULL
           || server->output_transport_factory == NULL || server->input_protocol_factory == NULL
           || server->output_protocol_factory == NULL);
}

int t_server_init(struct t_server* server,
                  struct t_processor* processor,
                  struct t_server_transport* server_transport,
                  struct t_transport_factory* input_transport_factory,
                  struct t_transport_factory* output_transport_factory,
                  struct t_protocol_factory* input_protocol_factory,
                  struct t_protocol_factory* output_protocol_factory);

#ifdef __cplusplus
}
#endif
