#pragma once

#include <thrift/c/processor/t_processor.h>
#include <thrift/c/protocol/t_protocol.h>
#include <thrift/c/protocol/t_protocol_factory.h>
#include <thrift/c/transport/t_server_transport.h>
#include <thrift/c/transport/t_transport_factory.h>

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
  return server->serve == NULL || server->stop == NULL || !t_processor_is_valid(server->processor)
         || !t_server_transport_is_valid(server->server_transport)
         || t_transport_factory_is_valid(server->input_transport_factory)
         || (server->output_transport_factory != NULL
             && !t_transport_factory_is_valid(server->output_transport_factory))
         || t_protocol_factory_is_valid(server->output_protocol_factory)
         || (server->output_protocol_factory != NULL
             && !t_protocol_factory_is_valid(server->output_protocol_factory));
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
