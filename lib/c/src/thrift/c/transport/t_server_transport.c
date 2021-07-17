#include <errno.h>

#include "t_server_transport.h"

// by default does nothing
static int t_server_transport_listen(struct t_server_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  return 0;
}

// "pure virtual" (subclasses must override this method in their own init)
static int t_server_transport_accept(struct t_server_transport* t, struct t_transport** xport) {
  (void)xport;
  if (t == NULL) {
    return -EINVAL;
  }

  return -ENOSYS;
}

// "pure virtual" (subclasses must override this method in their own init)
static int t_server_transport_close(struct t_server_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  return -ENOSYS;
}

// by default does nothing
static int t_server_transport_interrupt(struct t_server_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  return 0;
}

// by default does nothing
static int t_server_transport_interrupt_children(struct t_server_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  return 0;
}

bool t_server_transport_is_valid(struct t_server_transport* t) {
  return !(t == NULL || t->listen == NULL || t->accept == NULL || t->close == NULL
           || t->interrupt == NULL || t->interrupt_children == NULL);
}

int t_server_transport_init(struct t_server_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  t->listen = t_server_transport_listen;
  t->accept = t_server_transport_accept;
  t->close = t_server_transport_close;
  t->interrupt = t_server_transport_interrupt;
  t->interrupt_children = t_server_transport_interrupt_children;

  return 0;
}