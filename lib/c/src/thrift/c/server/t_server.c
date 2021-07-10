#include <errno.h>

#include "t_server.h"

static int t_server_serve(struct t_server* server) {
  (void)server;
  return -ENOSYS;
}

static int t_server_stop(struct t_server* server) {
  (void)server;
  return -ENOSYS;
}

int t_server_init(struct t_server* server,
                  struct t_processor* processor,
                  struct t_server_transport* server_transport,
                  struct t_transport_factory* input_transport_factory,
                  struct t_transport_factory* output_transport_factory,
                  struct t_protocol_factory* input_protocol_factory,
                  struct t_protocol_factory* output_protocol_factory) {

  if (server == NULL || processor == NULL || server_transport == NULL
      || input_transport_factory == NULL || input_protocol_factory == NULL) {
    return -EINVAL;
  }

  server->serve = t_server_serve;
  server->stop = t_server_stop;

  server->processor = processor;
  server->server_transport = server_transport;
  server->input_transport_factory = input_transport_factory;
  server->output_transport_factory = output_transport_factory;
  server->input_protocol_factory = input_protocol_factory;
  server->output_protocol_factory = output_protocol_factory;

  return 0;
}