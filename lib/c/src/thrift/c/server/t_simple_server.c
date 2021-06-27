#include <errno.h>

#include "t_simple_server.h"

static int t_simple_server_serve(struct t_server* server) {
  int r;
  struct t_simple_server* const s = (struct t_simple_server*)server;
  struct t_transport* t = NULL;
  struct t_transport* input_transport = NULL;
  struct t_transport* output_transport = NULL;
  struct t_protocol* input_protocol = NULL;
  struct t_protocol* output_protocol = NULL;

  if (!t_server_is_valid(server)) {
    return -EINVAL;
  }

  if (s->running) {
    return -EALREADY;
  }

  r = s->server_transport->listen(s->server_transport);
  if (r < 0) {
    return r;
  }

  s->running = true;

  for (; s->running;) {
    r = s->server_transport->accept(s->server_transport, &t);
    if (r < 0) {
      goto close_server_transport;
    }

    r = s->input_transport_factory->get_transport(s->input_transport_factory, t, &input_transport);
    if (r < 0) {
      goto put_input_transport;
    }

    r = s->output_transport_factory->get_transport(s->output_transport_factory, t,
                                                   &output_transport);
    if (r < 0) {
      goto put_output_transport;
    }

    r = s->input_protocol_factory->get_protocol(s->input_protocol_factory, input_transport,
                                                &input_protocol);
    if (r < 0) {
      goto put_input_protocol;
    }

    r = s->output_protocol_factory->get_protocol(s->output_protocol_factory, output_transport,
                                                 &output_protocol);
    if (r < 0) {
      goto put_output_protocol;
    }

    r = s->processor->process(s->processor, input_protocol, output_protocol);
    if (r < 0) {
      goto put_output_protocol;
    }

    continue;

  put_output_protocol:
    s->output_protocol_factory->put_protocol(s->output_protocol_factory, output_protocol);

  put_input_protocol:
    s->input_protocol_factory->put_protocol(s->input_protocol_factory, input_protocol);

  put_output_transport:
    s->output_transport_factory->put_transport(s->output_transport_factory, output_transport);

  put_input_transport:
    s->input_transport_factory->put_transport(s->input_transport_factory, input_transport);

  close_server_transport:
    s->server_transport->close(s->server_transport);

    s->running = false;
  }

  return r;
}

static int t_simple_server_stop(struct t_server* server) {

  struct t_simple_server* const s = (struct t_simple_server*)server;

  if (!t_server_is_valid(server)) {
    return -EINVAL;
  }

  if (!s->running) {
    return 0;
  }

  s->running = false;

  return 0;
}

int t_simple_server_init(struct t_simple_server* server,
                         struct t_processor* processor,
                         struct t_server_transport* server_transport,
                         struct t_transport_factory* input_transport_factory,
                         struct t_transport_factory* output_transport_factory,
                         struct t_protocol_factory* input_protocol_factory,
                         struct t_protocol_factory* output_protocol_factory) {

  int r;

  r = t_server_init((struct t_server*)server, processor, server_transport, input_transport_factory,
                    output_transport_factory, input_protocol_factory, output_protocol_factory);

  if (r == 0) {
    server->serve = t_simple_server_serve;
    server->stop = t_simple_server_stop;
    server->running = false;
  }

  return r;
}
