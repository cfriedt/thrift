#include <errno.h>

#include "t_simple_transport_factory.h"

static int t_simple_transport_factory_get_transport(struct t_transport_factory* factory,
                                                    struct t_transport* xport,
                                                    struct t_transport** new_xport) {

  if (factory == NULL || new_xport == NULL || !t_transport_is_valid(xport)) {
    return -EINVAL;
  }

  *new_xport = xport;

  if (xport->is_open(xport)) {
    return 0;
  }

  return xport->open(xport);
}

int t_simple_transport_factory_put_transport(struct t_transport_factory* factory,
                                             struct t_transport* xport) {
  if (factory == NULL || !t_transport_is_valid(xport)) {
    return -EINVAL;
  }

  return xport->close(xport);
}

int t_simple_transport_factory_init(struct t_simple_transport_factory* factory) {
  struct t_simple_transport_factory* const s = (struct t_simple_transport_factory*)factory;

  if (factory == NULL) {
    return -EINVAL;
  }

  s->get_transport = t_simple_transport_factory_get_transport;
  s->put_transport = t_simple_transport_factory_put_transport;

  return 0;
}
