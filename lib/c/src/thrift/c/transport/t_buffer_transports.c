#include <assert.h>
#include <errno.h>
#include <string.h>

#include "thrift/c/transport/t_buffer_transports.h"
#include "thrift/c/transport/t_transport.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

extern int t_transport_init(struct t_transport* transport);

/*
 * t_memory_buffer
 */

static inline void t_memory_buffer_reinit(struct t_memory_buffer* t, void* buffer, uint32_t size) {
  t->open_ = false;
  t->buffer_ = buffer;
  t->bufferSize_ = size;
  t->rBase_ = buffer;
  t->rBound_ = buffer;
  t->wBase_ = buffer;
  t->wBound_ = t->wBase_ + size;
}

static int t_memory_buffer_open(struct t_transport* t) {
  struct t_memory_buffer* const t_ = (struct t_memory_buffer*)t;

  if (t_ == NULL) {
    return -EINVAL;
  }

  if (t_->open_) {
    return -EALREADY;
  }

  t_memory_buffer_reinit(t_, t_->buffer_, t_->bufferSize_);

  t_->open_ = true;

  return 0;
}

static int t_memory_buffer_close(struct t_transport* t) {
  struct t_memory_buffer* const t_ = (struct t_memory_buffer*)t;

  if (t_ == NULL) {
    return -EINVAL;
  }

  if (!t_->open_) {
    return -EALREADY;
  }

  t_->open_ = false;

  return 0;
}

static bool t_memory_buffer_is_open(struct t_transport* t) {
  struct t_memory_buffer* const t_ = (struct t_memory_buffer*)t;

  if (t_ == NULL) {
    return false;
  }

  return t_->open_;
}

static int t_memory_buffer_available_read(struct t_transport* t) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  if (!mb->open_) {
    return -EBADF;
  }

  assert(mb->rBound_ >= mb->rBase_);

  return mb->rBound_ - mb->rBase_;
}

static int t_memory_buffer_available_write(struct t_transport* t) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  assert(mb->wBound_ >= mb->wBase_);

  return mb->wBound_ - mb->wBase_;
}

static int t_memory_buffer_read(struct t_transport* t, void* buf, uint32_t size) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;
  int avail;

  if (!t_transport_is_valid(t) || buf == NULL) {
    return -EINVAL;
  }

  if (!mb->open_) {
    return -EBADF;
  }

  avail = t_memory_buffer_available_read(t);
  if (avail <= 0) {
    return avail;
  }

  size = MIN(size, (uint32_t)avail);

  memcpy(buf, mb->rBase_, size);
  mb->rBase_ += size;

  return size;
}

static int t_memory_buffer_write(struct t_transport* t, const void* buf, uint32_t size) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;
  int avail;

  if (!t_transport_is_valid(t) || buf == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  avail = t_memory_buffer_available_write(t);
  if (avail < 0) {
    return avail;
  }

  size = MIN(size, (uint32_t)avail);
  memcpy(mb->wBase_, buf, size);
  mb->rBound_ += size;
  mb->wBase_ += size;

  return size;
}

static int t_memory_buffer_borrow(struct t_transport* t, void** buf, uint32_t* len) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;

  if (!t_transport_is_valid(t) || buf == NULL || len == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  if (*len == 0) {
    return 0;
  }

  *len = MIN((int)*len, t_memory_buffer_available_read(t));
  if (*buf == NULL) {
    *buf = mb->rBase_;
  } else {
    memcpy(*buf, mb->rBase_, *len);
  }

  return *len;
}

static int t_memory_buffer_consume(struct t_transport* t, uint32_t len) {
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  assert((int)len <= t_memory_buffer_available_read(t));

  mb->rBase_ += len;

  return 0;
}

static const char* t_memory_buffer_get_origin(struct t_transport* t) {
  return (t == NULL) ? NULL : "memory";
}

int t_memory_buffer_init(struct t_memory_buffer* t, void* buffer, uint32_t size) {
  int r;

  if (t == NULL || buffer == NULL || size == 0) {
    return -EINVAL;
  }

  r = t_transport_init((struct t_transport*)t);
  if (r < 0) {
    return r;
  }

  t->is_open = t_memory_buffer_is_open;
  t->open = t_memory_buffer_open;
  t->close = t_memory_buffer_close;
  t->read = t_memory_buffer_read;
  t->write = t_memory_buffer_write;
  t->borrow = t_memory_buffer_borrow;
  t->consume = t_memory_buffer_consume;
  t->get_origin = t_memory_buffer_get_origin;
  t->available_read = t_memory_buffer_available_read;
  t->available_write = t_memory_buffer_available_write;

  t_memory_buffer_reinit(t, buffer, size);

  return 0;
}

/*
 * t_buffered_transport
 */

static inline void t_buffered_transport_reinit(struct t_buffered_transport* t,
                                               void* buffer,
                                               uint32_t size) {
  t_memory_buffer_reinit((struct t_memory_buffer*)t, buffer, size);
  t->wBase_ = buffer + size;
  t->wBound_ = buffer + size;
}

static int t_buffered_transport_open(struct t_transport* t) {
  int r;
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t_ == NULL) {
    return -EINVAL;
  }

  if (t_->open_) {
    return -EALREADY;
  }

  assert(t_transport_is_valid(t_->trans));

  r = t_->trans->open(t_->trans);
  if (r < 0) {
    return r;
  }

  t_buffered_transport_reinit(t_, t_->buffer_, t_->bufferSize_);
  t_->open_ = true;

  return 0;
}

static int t_buffered_transport_close(struct t_transport* t) {
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t_ == NULL) {
    return -EINVAL;
  }

  if (!t_->open_) {
    return -EALREADY;
  }

  assert(t_transport_is_valid(t_->trans));

  return t_->trans->close(t_->trans);
}

static bool t_buffered_transport_is_open(struct t_transport* t) {
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t_ == NULL) {
    return false;
  }

  assert(t_transport_is_valid(t_->trans));

  return t_->trans->is_open(t_->trans) || t_memory_buffer_is_open(t);
}

static int t_buffered_transport_available_read(struct t_transport* t) {
  int r;
  int sum;
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  assert(t_transport_is_valid(t_->trans));

  sum = 0;
  r = t_memory_buffer_available_read(t);
  if (r < 0) {
    return r;
  }

  sum += r;

  r = t_->trans->available_read(t_->trans);
  if (r < 0) {
    return sum;
  }

  sum += r;

  return sum;
}

static int t_buffered_transport_available_write(struct t_transport* t) {
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  assert(t_transport_is_valid(t_->trans));

  return t_->trans->available_read(t_->trans);
}

static int t_buffered_transport_refill(struct t_buffered_transport* t_) {
  int r;
  struct t_transport* const t = (struct t_transport*)t_;

  if (!t_->trans->is_open(t_->trans)) {
    // if the underlying transport has already been closed and
    // the buffer contents have been read out, then close the buffer
    // as well.
    t_memory_buffer_close(t);
    return -EBADF;
  }

  // rewind rBase & rBound
  t_buffered_transport_reinit(t_, t_->buffer_, t_->bufferSize_);
  t_->open_ = true;
  // try to fill up the buffer
  r = t_->trans->read(t_->trans, t_->buffer_, t_->bufferSize_);
  if (r < 0) {
    t_memory_buffer_close(t);
    return r;
  }

  assert(r <= (int)t_->bufferSize_);
  t_->rBound_ += r;

  return r;
}

static int t_buffered_transport_read(struct t_transport* t, void* buf, uint32_t size) {
  int r;
  int avail;
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;
  struct t_memory_buffer* const mb = (struct t_memory_buffer*)t;

  if (t == NULL || buf == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  avail = t_memory_buffer_available_read(t);
  assert(avail >= 0);

  if (avail == 0) {
    r = t_buffered_transport_refill(t_);
    if (r < 0) {
      return r;
    }

    avail += r;
  }

  assert(offsetof(struct t_memory_buffer, open_) == offsetof(struct t_buffered_transport, open_));
  assert(mb->open_ == t_->open_);
  assert(t_->open_);
  assert(mb->open_);

  size = MIN(avail, (int)size);
  r = t_memory_buffer_read(t, buf, size);
  assert(r == (int)size);

  return size;
}

static int t_buffered_transport_write(struct t_transport* t, void* buf, uint32_t size) {
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  assert(t_transport_is_valid(t_->trans));

  return t_->trans->write(t_->trans, buf, size);
}

static int t_buffered_transport_borrow(struct t_transport* t, void** buf, uint32_t* len) {
  int r;
  int avail;
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t == NULL || buf == NULL || len == NULL) {
    return -EINVAL;
  }

  if (!t_memory_buffer_is_open(t)) {
    return -EBADF;
  }

  avail = t_memory_buffer_available_read(t);
  assert(avail >= 0);

  if (avail == 0) {
    r = t_buffered_transport_refill(t_);
    if (r < 0) {
      return r;
    }

    avail += r;
  }

  return t_memory_buffer_borrow(t, buf, len);
}

static const char* t_buffered_transport_get_origin(struct t_transport* t) {
  struct t_buffered_transport* const t_ = (struct t_buffered_transport*)t;

  if (t == NULL) {
    return NULL;
  }

  if (!t_transport_is_valid(t_->trans)) {
    return NULL;
  }

  return t_->trans->get_origin(t_->trans);
}

int t_buffered_transport_init(struct t_buffered_transport* t,
                              void* buffer,
                              uint32_t size,
                              struct t_transport* trans) {
  int r;

  if (!t_transport_is_valid(trans)) {
    return -EINVAL;
  }

  r = t_memory_buffer_init((struct t_memory_buffer*)t, buffer, size);
  if (r < 0) {
    return r;
  }

  t->is_open = t_buffered_transport_is_open;
  t->open = t_buffered_transport_open;
  t->close = t_buffered_transport_close;
  t->read = t_buffered_transport_read;
  t->write = t_buffered_transport_write;
  t->borrow = t_buffered_transport_borrow;
  t->consume = t_memory_buffer_consume;
  t->get_origin = t_buffered_transport_get_origin;
  t->available_read = t_buffered_transport_available_read;
  t->available_write = t_buffered_transport_available_write;

  t->trans = trans;

  return 0;
}
