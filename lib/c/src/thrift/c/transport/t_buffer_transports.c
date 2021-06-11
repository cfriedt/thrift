#include <assert.h>
#include <errno.h>
#include <string.h>

#include "thrift/c/transport/t_transport.h"
#include "thrift/c/transport/t_buffer_transports.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

static int t_memory_buffer_open(struct t_transport *t)
{
    struct t_memory_buffer *t_ = (struct t_memory_buffer *)t;

    if (t_ == NULL) {
        return -EINVAL;
    }

    if (t_->open_) {
        return -EALREADY;
    }

    t_->open_ = true;

    return 0;
}

static int t_memory_buffer_close(struct t_transport *t)
{
    struct t_memory_buffer *t_ = (struct t_memory_buffer *)t;

    if (t_ == NULL) {
        return -EINVAL;
    }

    if (!t_->open_) {
        return -EALREADY;
    }

    t_->open_ = false;

    return 0;
}

static bool t_memory_buffer_isOpen(struct t_transport *t)
{
    struct t_memory_buffer *t_ = (struct t_memory_buffer *)t;

    if (t_ == NULL) {
        return false;
    }

    return t_->open_;
}

static bool t_memory_buffer_peek(struct t_transport *t)
{
    struct t_memory_buffer *t_ = (struct t_memory_buffer *)t;

    if (t_ == NULL) {
        return false;
    }

    return t_->rBase_ < t_->rBound_;
}

static int t_memory_buffer_read(struct t_transport *t, void *buf, uint32_t size)
{
    struct t_memory_buffer *const mb = (struct t_memory_buffer *)t;
    int avail;

    if (t == NULL || buf == NULL) {
        return -EINVAL;
    }     

    avail = mb->available_read(mb);
    if (avail <= 0) {
        return avail;
    }

    size = MIN(size, (uint32_t)avail);

    memcpy(buf, mb->rBase_, size);
    mb->rBase_ += size;

    return size;
}

static int t_memory_buffer_readAll(struct t_transport *t, void *buf, uint32_t size)
{
    int r;
    uint8_t *b;
 
    for(b = (uint8_t *) buf; size > 0; size -= r, b += r) {
        r = t_memory_buffer_read(t, b, size);
        if (r < 0) {
            return r;
        }
    }

    return b - (uint8_t *)buf;
}

static int t_memory_buffer_write(struct t_transport *t, const void *buf, uint32_t size)
{
    struct t_memory_buffer *const mb = (struct t_memory_buffer *)t;
    int avail;

    if (t == NULL || buf == NULL) {
        return -EINVAL;
    }     

    avail = mb->available_write(mb);
    if (avail < 0) {
        return avail;
    }

    size = MIN(size, (uint32_t)avail);
    memcpy(mb->wBase_, buf, size);
    mb->rBound_ += size;
    mb->wBase_ += size;

    return size;
}

static int t_memory_buffer_writeAll(struct t_transport *t, const void *buf, uint32_t size)
{
    int r;
    uint8_t *b;
 
    for(b = (uint8_t *) buf; size > 0; size -= r, b += r) {
        r = t_memory_buffer_write(t, b, size);
        if (r < 0) {
            return r;
        }
    }

    return b - (uint8_t *)buf;
}

static int t_memory_buffer_flush(struct t_transport *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_memory_buffer_readEnd(struct t_transport *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_memory_buffer_writeEnd(struct t_transport *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_memory_buffer_borrow(struct t_transport *t, void **buf, uint32_t *len)
{
    struct t_memory_buffer *const mb = (struct t_memory_buffer *)t;

    if (t == NULL || buf == NULL || len == NULL) {
        return -EINVAL;
    }

    *len = MIN(*len, (uint32_t)mb->available_read(mb));
    if (*buf == NULL) {
        *buf = mb->rBase_;
    } else {
        memcpy(*buf, mb->rBase_, *len);
    }

    return *len;
}

static int t_memory_buffer_consume(struct t_transport *t, uint32_t len)
{
    struct t_memory_buffer *const mb = (struct t_memory_buffer *)t;

    if (t == NULL) {
        return -EINVAL;
    }

    assert(len <= (uint32_t)mb->available_read(mb));

    mb->rBase_ += len;

    return 0;
}

static const char *t_memory_buffer_getOrigin(struct t_transport *t)
{
    return (t == NULL) ? NULL : "memory";
}

static int t_memory_buffer_available_read(struct t_memory_buffer *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    assert(t->rBound_ >= t->rBase_);

    return t->rBound_ - t->rBase_;
}

static int t_memory_buffer_available_write(struct t_memory_buffer *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    assert(t->wBound_ >= t->wBase_);

    return t->wBound_ - t->wBase_;
}

int t_memory_buffer_init(struct t_memory_buffer *t, void *buffer, uint32_t size)
{
    if (t == NULL || buffer == NULL || size == 0) {
        return -EINVAL;
    }

    t->isOpen = t_memory_buffer_isOpen;
    t->peek = t_memory_buffer_peek;
    t->open = t_memory_buffer_open;
    t->close = t_memory_buffer_close;
    t->read = t_memory_buffer_read;
    t->readAll = t_memory_buffer_readAll;
    t->write = t_memory_buffer_write;
    t->writeAll = t_memory_buffer_writeAll;
    t->flush = t_memory_buffer_flush;
    t->readEnd = t_memory_buffer_readEnd;
    t->writeEnd = t_memory_buffer_writeEnd;
    t->borrow = t_memory_buffer_borrow;
    t->consume = t_memory_buffer_consume;
    t->getOrigin = t_memory_buffer_getOrigin;
    t->available_read = t_memory_buffer_available_read;
    t->available_write = t_memory_buffer_available_write;

    t->open_ = false;
    t->buffer_ = buffer;
    t->bufferSize_ = size;
    t->rBase_ = buffer;
    t->rBound_ = buffer;
    t->wBase_ = buffer;
    t->wBound_ = t->wBase_ + size;

    return 0;
}
