#pragma once

#include <stdbool.h>

#include <thrift/c/protocol/t_protocol_factory.h>
#include <thrift/c/server/t_server.h>
#include <thrift/c/transport/t_transport_factory.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_simple_server {
  T_SERVER_METHODS;
  T_SERVER_FIELDS;
  bool running;
};

int t_simple_server_init(struct t_simple_server* server,
                         struct t_processor* processor,
                         struct t_server_transport* server_transport,
                         struct t_transport_factory* input_transport_factory,
                         struct t_transport_factory* output_transport_factory,
                         struct t_protocol_factory* input_protocol_factory,
                         struct t_protocol_factory* output_protocol_factory);

#ifdef __cplusplus
}
#endif
