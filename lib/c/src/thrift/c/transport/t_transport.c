#include <errno.h>

#include "t_transport.h"

static bool t_transport_false(struct t_transport* t) {
  (void)t;
  return false;
}

static int t_transport_zero(struct t_transport* t) {
  return (t == NULL) ? -EINVAL : 0;
}

static int t_transport_enosys(struct t_transport* t) {
  return (t == NULL) ? -EINVAL : -ENOSYS;
}

static int t_transport_enosys_voidp_u32(struct t_transport* t, void* x, uint32_t y) {
  (void)x;
  (void)y;
  return (t == NULL) ? -EINVAL : -ENOSYS;
}

static void* t_transport_str(struct t_transport* t) {
  return (t == NULL) ? NULL : "Unknown";
}

static int t_transport_enosys_u32(struct t_transport* t, uint32_t x) {
  (void)x;
  return (t == NULL) ? -EINVAL : -ENOSYS;
}

static bool t_transport_peek(struct t_transport* t) {
  if (t == NULL) {
    return false;
  }

  return t->is_open(t);
}

static int t_transport_read_all(struct t_transport* t, void* buf, uint32_t size) {
  int r;
  uint8_t* b;

  if (!t_transport_is_valid(t)) {
    return -EINVAL;
  }

  for (b = (uint8_t*)buf; size > 0; size -= r, b += r) {
    r = t->read(t, b, size);
    if (r < 0) {
      return r;
    }
  }

  return b - (uint8_t*)buf;
}

static int t_transport_write_all(struct t_transport* t, const void* buf, uint32_t size) {
  int r;
  const uint8_t* b;

  if (!t_transport_is_valid(t)) {
    return -EINVAL;
  }

  for (b = (uint8_t*)buf; size > 0; size -= r, b += r) {
    r = t->write(t, b, size);
    if (r < 0) {
      return r;
    }
  }

  return b - (uint8_t*)buf;
}

bool t_transport_is_valid(struct t_transport* t) {
  return !(t == NULL || t->is_open == NULL || t->peek == NULL || t->open == NULL || t->close == NULL
           || t->read == NULL || t->read_all == NULL || t->write == NULL || t->write_all == NULL
           || t->flush == NULL || t->read_end == NULL || t->write_end == NULL || t->borrow == NULL
           || t->consume == NULL || t->get_origin == NULL || t->available_read == NULL
           || t->available_write == NULL);
}

int t_transport_init(struct t_transport* t) {
  if (t == NULL) {
    return -EINVAL;
  }

  t->is_open = t_transport_false;
  t->peek = t_transport_peek;
  t->open = t_transport_enosys;
  t->close = t_transport_enosys;
  t->read = t_transport_enosys_voidp_u32;
  t->read_all = t_transport_read_all;
  t->write = t_transport_enosys_voidp_u32;
  t->write_all = t_transport_write_all;
  t->flush = t_transport_zero;
  t->read_end = t_transport_zero;
  t->write_end = t_transport_zero;
  t->borrow = t_transport_enosys_voidp_u32;
  t->consume = t_transport_enosys_u32;
  t->get_origin = t_transport_str;
  t->available_read = t_transport_enosys;
  t->available_write = t_transport_enosys;

  return 0;
}