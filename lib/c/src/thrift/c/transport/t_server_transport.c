#include "t_server_transport.h"

bool t_server_transport_is_valid(struct t_server_transport* t) {
  return !(t == NULL || t->listen == NULL || t->accept == NULL || t->close == NULL);
}
