#include <errno.h>
#include <string.h>

#include "thrift/c/transport/t_transport.h"
#include "thrift/c/transport/t_buffered_transports.h"

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

static int t_memory_buffer_read(struct t_transport *t, void *buf, size_t size)
{
    struct t_memory_buffer *const mb = (struct t_memory_buffer *)t;

    if (t == NULL || buf == NULL) {
        return -EINVAL;
    }     

    size = MIN(size, mb->available_read(mb));
    memcpy(buf, mb->rBase_, size);
    mb->rBase_ += size;

    return size;
}

static int t_memory_buffer_write(struct t_transport *t, const void *buf, size_t size)
{
    struct t_memory_buffer *const trans = (struct t_memory_buffer *)t;

    if (t == NULL || buf == NULL) {
        return -EINVAL;
    }     

    size = MIN(size, trans->available_write(trans));
    memcpy(trans->wBase_, buf, size);
    trans->rBound_ += size;
    trans->wBase_ += size;

    return size;
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

static int t_memory_buffer_available_read(struct t_memory_buffer *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    return t->rBound_ - t->rBase_;
}

static int t_memory_buffer_available_write(struct t_memory_buffer *t)
{
    if (t == NULL) {
        return -EINVAL;
    }

    return t->wBound_ - t->wBase_;
}

int t_memory_buffer_init(struct t_memory_buffer *t, void *buffer, size_t size)
{
    if (t == NULL || buffer == NULL || size == 0) {
        return -EINVAL;
    }

    t->open = t_memory_buffer_open;
    t->close = t_memory_buffer_close;
    t->isOpen = t_memory_buffer_isOpen;
    t->read = t_memory_buffer_read;
    t->write = t_memory_buffer_write;
    t->flush = t_memory_buffer_flush;
    t->readEnd = t_memory_buffer_readEnd;
    t->writeEnd = t_memory_buffer_writeEnd;
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
