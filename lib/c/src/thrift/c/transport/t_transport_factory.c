#include "t_transport_factory.h"

bool t_transport_factory_is_valid(struct t_transport_factory* f) {
  return !(f == NULL || f->get_transport == NULL || f->put_transport == NULL);
}