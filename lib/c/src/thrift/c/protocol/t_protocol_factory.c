#include "t_protocol_factory.h"

bool t_protocol_factory_is_valid(struct t_protocol_factory* proto) {
  return !(proto == NULL || proto->get_protocol == NULL || proto->put_protocol == NULL);
}
