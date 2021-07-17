#include <assert.h>
#include <errno.h>

#include "t_simple_server.h"
#include "thrift/c/protocol/t_binary_protocol.h"

#include <pthread.h>
#include <stdio.h>
#define D(fmt, args...)                                                                            \
  printf("%p: %s(): %d: " fmt "\n", pthread_self(), __func__, __LINE__, ##args)

#define E(fmt, args...)                                                                            \
  fprintf(stderr, "E: %s:%d: %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##args)

static int t_simple_server_serve(struct t_server* server) {
  int r;
  struct t_simple_server* const s = (struct t_simple_server*)server;
  struct t_binary_protocol _protocol;

  if (!t_server_is_valid(server)) {
    E("server is not valid");
    return -EINVAL;
  }

  D("calling listen()");
  r = s->server_transport->listen(s->server_transport);
  if (r < 0) {
    E("listen failed: %d", r);
    return r;
  }

  s->running = true;
  for (; s->running;) {
    struct t_transport* t = NULL;
    struct t_transport* input_transport = NULL;
    struct t_transport* output_transport = NULL;
    struct t_protocol* input_protocol = (struct t_protocol*)&_protocol;
    struct t_protocol* output_protocol = NULL;

    D("calling accept()");
    r = s->server_transport->accept(s->server_transport, &t);
    D("returned from accept()");
    if (r < 0) {
      E("accept() failed: %d", r);
      goto close_server_transport;
    }

    assert(t_transport_is_valid(t));

    if (!s->running) {
      D("no longer running");
      t->close(t);
      break;
    }

    D("calling get_transport()");
    r = s->input_transport_factory->get_transport(s->input_transport_factory, t, &input_transport);
    if (r < 0) {
      E("get_transport() failed: %d", r);
      goto close_server_transport;
    }

    assert(input_transport != NULL);

    if (s->output_transport_factory == NULL) {
      output_transport = input_transport;
    } else {
      D("calling get_transport()");
      r = s->output_transport_factory->get_transport(s->output_transport_factory, t,
                                                     &output_transport);
      if (r < 0) {
        E("get_transport() failed: %d", r);
        goto put_input_transport;
      }
    }

    assert(output_transport != NULL);

    D("calling get_protocol()");
    r = s->input_protocol_factory->get_protocol(s->input_protocol_factory, input_transport,
                                                &input_protocol);
    if (r < 0) {
      E("get_protocol() failed: %d", r);
      goto put_output_transport;
    }

    assert(input_protocol != NULL);

    if (s->output_protocol_factory == NULL) {
      output_protocol = input_protocol;
    } else {
      D("calling get_protocol()");
      r = s->output_protocol_factory->get_protocol(s->output_protocol_factory, output_transport,
                                                   &output_protocol);
      if (r < 0) {
        E("get_protocol() failed: %d", r);
        goto put_input_protocol;
      }
    }

    assert(output_protocol != NULL);

    for (;;) {

      D("calling peek()");
      if (!input_transport->peek(input_transport)) {
        E("peek failed()");
        r = -EIO;
        break;
      }

      D("calling process()");
      r = s->processor->process(s->processor, input_protocol, output_protocol);
      if (r < 0) {
        E("process() failed(): %d", r);
        goto put_output_protocol;
      }
    }

  put_output_protocol:
    if (output_protocol != input_protocol) {
      D("calling put_protocol()");
      s->output_protocol_factory->put_protocol(s->output_protocol_factory, output_protocol);
    }

  put_input_protocol:
    D("calling put_protocol()");
    s->input_protocol_factory->put_protocol(s->input_protocol_factory, input_protocol);

  put_output_transport:
    if (output_transport != input_transport) {
      D("calling put_transport()");
      s->output_transport_factory->put_transport(s->output_transport_factory, output_transport);
    }

  put_input_transport:
    D("calling put_transport()");
    s->input_transport_factory->put_transport(s->input_transport_factory, input_transport);

    if (r < 0) {
      s->running = false;
    }
  }

close_server_transport:
  D("calling close()");
  s->server_transport->close(s->server_transport);

  s->running = false;
  return r;
}

static int t_simple_server_stop(struct t_server* server) {

  struct t_simple_server* const s = (struct t_simple_server*)server;

  if (!t_server_is_valid(server) || !t_server_transport_is_valid(s->server_transport)) {
    return -EINVAL;
  }

  s->running = false;
  D("calling interrupt_children()");
  s->server_transport->interrupt_children(s->server_transport);
  D("calling interrupt()");
  s->server_transport->interrupt(s->server_transport);

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
