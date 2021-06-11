#include <assert.h>
#include <errno.h>
#include <string.h>

#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/transport/t_transport.h"

#define VERSION_1 0x80010000
#define VERSION_MASK 0xffff0000

static int t_binary_protocol_writeMessageBegin(struct t_protocol *p, const char *name, enum t_message_type message_type, uint32_t seq)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    int32_t version = VERSION_1 | message_type;
    uint32_t wsize = 0;
    int r;

    if (p == NULL || name == NULL || !t_message_type_is_valid(message_type)) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeI32(p, version);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeString(p, strlen(name), name);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeI32(p, seq); 
    if (r < 0) {
        return r;
    }

    wsize += r;

    return wsize;
}

// this is used as a number of writeXXXEnd() implementations
static int t_binary_protocol_writeEndCommon(struct t_protocol *p)
{
    if (p == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_binary_protocol_writeStructBegin(struct t_protocol *p, const char *name)
{
    (void) name;

    if (p == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_binary_protocol_writeFieldBegin(struct t_protocol *p, const char *name, enum t_type field_type, int field_id)
{
    (void) name;

    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t wsize = 0;
    int r;

    if (p == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeByte(p, field_type);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeI16(p, field_id);
    if (r < 0) {
        return r;
    }

    wsize += r;

    return wsize;
}

static int t_binary_protocol_writeFieldStop(struct t_protocol *p)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t wsize = 0;
    int r;

    if (p == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeByte(p, T_STOP);
    if (r < 0) {
        return r;
    }

    wsize += r;

    return wsize;
}

static int t_binary_protocol_writeMapBegin(struct t_protocol *p, enum t_type ktype, enum t_type vtype, uint32_t size)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t wsize = 0;
    int r;

    if (p == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeByte(p, ktype);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeByte(p, vtype);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeI32(p, size);
    if (r < 0) {
        return r;
    }

    wsize += r;

    return wsize;
}

// exactly the same implementation for List and Set
static int t_binary_protocol_writeListBegin(struct t_protocol *p, enum t_type etype, uint32_t size)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t wsize = 0;
    int r;

    if (p == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeByte(p, etype);
    if (r < 0) {
        return r;
    }

    wsize += r;

    r = p->writeI32(p, size);
    if (r < 0) {
        return r;
    }

    wsize += r;

    return wsize;
}

#define to8(x) x
#define to16(x) p_->bo->toWire16(x)
#define to32(x) p_->bo->toWire32(x)
#define to64(x) p_->bo->toWire64(x)

#define D_WRITEI(n) \
static int t_binary_protocol_writeI ## n (struct t_protocol *p, int ## n ## _t x) \
{ \
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p; \
    int r; \
\
    if (p == NULL) { \
        return -EINVAL; \
    } \
\
    assert(p_->trans != NULL); \
    assert(t_byte_order_is_valid(p_->bo)); \
\
    x = to ## n (x); \
\
    r = p_->trans->writeAll(p_->trans, &x, n / 8); \
    if (r < 0) { \
        return r; \
    } \
\
    return n / 8; \
}

D_WRITEI(8);
D_WRITEI(16);
D_WRITEI(32);
D_WRITEI(64);

static inline int t_binary_protocol_writeDouble(struct t_protocol *p, double x)
{
    union bitwise_cast {
        int64_t i64;
        double f64;
    } cast;

    cast.f64 = x;

    return t_binary_protocol_writeI64(p, cast.i64);
}

static int t_binary_protocol_writeString(struct t_protocol *p, uint32_t size, const char *s)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t wsize = 0;
    int r;

    if (p == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->writeI32(p, size);
    if (r < 0) {
        return r;
    }

    wsize += r;

    if (size > 0) {
        r = p_->trans->writeAll(p_->trans, s, size);
        if (r < 0) {
            return r;
        }

        wsize += r;
    }

    return wsize;
}

static int t_binary_protocol_readMessageBegin(struct t_protocol *p, uint32_t *len, char **name, enum t_message_type *type, uint32_t *seq)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    uint32_t rsize = 0;
    int32_t version;
    int r;

    if (p == NULL || len == NULL || name == NULL || type == NULL || seq == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    r = p->readI32(p, &version);
    if (r < 0) {
        return r;
    }

    rsize += r;

    *type = version & ~VERSION_MASK;
    version &= VERSION_MASK;

    if ((version & VERSION_MASK) != VERSION_1) {
        return -EBADMSG;
    }

    if (!t_message_type_is_valid(*type)) {
        return -EINVAL;
    }

    r = p->readString(p, len, name);
    if (r < 0) {
        return r;
    }

    rsize += r;

    r = p->readI32(p, (int32_t *)seq);
    if (r < 0) {
        return r;
    }

    rsize += r;

    return rsize;
}

// this is used as a number of readXXXEnd() implementations
static int t_binary_protocol_readEndCommon(struct t_protocol *p)
{
    if (p == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_binary_protocol_readStructBegin(struct t_protocol *p, uint32_t *len, char **name)
{
    (void) len;
    (void) name;

    if (p == NULL) {
        return -EINVAL;
    }

    return 0;
}

static int t_binary_protocol_readFieldBegin(struct t_protocol *p, uint32_t *len, char **name, enum t_type *field_type, uint16_t *field_id)
{
    int rsize = 0;
    int r;
    uint8_t byte;

    if (p == NULL || len == NULL || name == NULL || field_type == NULL || field_id == NULL) {
        return -EINVAL;
    }

    r = p->readByte(p, &byte);
    if (r < 0) {
        return r;
    }

    rsize += r;

    *field_type = byte;

    r = p->readI16(p, (int16_t *)field_id);
    if (r < 0) {
        return r;
    }

    rsize += r;

    return rsize;
}

static int t_binary_protocol_readMapBegin(struct t_protocol *p, enum t_type *ktype, enum t_type *vtype, uint32_t *size)
{
    int rsize = 0;
    int r;
    uint8_t byte;

    if (p == NULL || ktype == NULL || vtype == NULL || size == NULL) {
        return -EINVAL;
    }

    r = p->readByte(p, &byte);
    if (r < 0) {
        return r;
    }

    rsize += r;

    *ktype = byte;

    r = p->readByte(p, &byte);
    if (r < 0) {
        return r;
    }

    rsize += r;

    *vtype = byte;

    r = p->readI32(p, (int32_t *)size);
    if (r < 0) {
        return r;
    }

    rsize += r;

    return rsize;
}

// exactly the same implementation for List and Set
static int t_binary_protocol_readListBegin(struct t_protocol *p, enum t_type *etype, uint32_t *size)
{
    int rsize = 0;
    int r;
    uint8_t byte;

    if (p == NULL || etype == NULL || size == NULL) {
        return -EINVAL;
    }

    r = p->readByte(p, &byte);
    if (r < 0) {
        return r;
    }

    rsize += r;

    *etype = byte;

    r = p->readI32(p, (int32_t *)size);
    if (r < 0) {
        return r;
    }

    rsize += r;

    return rsize;
}

#define from8(x) x
#define from16(x) p_->bo->fromWire16(x)
#define from32(x) p_->bo->fromWire32(x)
#define from64(x) p_->bo->fromWire64(x)

#define D_READI(n) \
static int t_binary_protocol_readI ## n(struct t_protocol *p, int ## n ## _t *x) \
{ \
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p; \
    int r; \
\
    if (p == NULL || x == NULL) { \
        return -EINVAL; \
    } \
\
    assert(p_->trans != NULL); \
\
    r = p_->trans->readAll(p_->trans, x, n / 8); \
    if (r < 0) { \
        return r; \
    } \
\
    assert(t_byte_order_is_valid(p_->bo)); \
\
    *x = from ## n(*x); \
\
    return n / 8; \
}

D_READI(8);
D_READI(16);
D_READI(32);
D_READI(64);

static inline int t_binary_protocol_readBool(struct t_protocol *p, bool *x)
{
    int r;
    int8_t i8;

    r = t_binary_protocol_readI8(p, (x == NULL) ? NULL : &i8);
    if (1 == r) {
        *x = i8 != 0;
    }

    return r;
}

static inline int t_binary_protocol_readDouble(struct t_protocol *p, double *x)
{
    int r;

    union bitwise_cast {
        int64_t i64;
        double f64;
    } cast;

    r = t_binary_protocol_readI64(p, (x == NULL) ? NULL : &cast.i64);
    if (8 == r) {
        *x = cast.f64;
    }

    return r;
}

static int t_binary_protocol_readString(struct t_protocol *p, uint32_t *size, char **s)
{
    struct t_binary_protocol *const p_ = (struct t_binary_protocol *)p;
    int32_t ssize;
    int r;

    if (p == NULL || size == NULL || s == NULL) {
        return -EINVAL;
    }

    assert(p_->trans != NULL);

    if (*s == NULL) {
        // currently we do not malloc
        return -EINVAL;
    }

    // FIXME: this should actually be a borrow!!
    r = p->readI32(p, &ssize);
    if (r < 0) {
        return r;
    }

    assert(ssize >= 0);

    if ((uint32_t)ssize > *size) {
        *size = ssize;
        return -E2BIG;
    }

    *size = ssize;
    if (ssize == 0) {
        return 4;
    }

    if (ssize > 0) {
        r = p_->trans->readAll(p_->trans, *s, *size);
        if (r < 0) {
            return r;
        }
    }

    return 4 + *size;
}

int t_binary_protocol_init(struct t_binary_protocol *p, struct t_transport *t, const struct t_byte_order *bo)
{
    if (p == NULL || t == NULL) {
        return -EINVAL;
    }

    if (bo == NULL) {
        bo = &t_network_big_endian;
    }

    if (!t_byte_order_is_valid(bo)) {
        return -EINVAL;
    }

    p->writeMessageBegin = t_binary_protocol_writeMessageBegin;
    p->writeMessageEnd = t_binary_protocol_writeEndCommon;
    p->writeStructBegin = t_binary_protocol_writeStructBegin;
    p->writeStructEnd = t_binary_protocol_writeEndCommon;
    p->writeFieldBegin = t_binary_protocol_writeFieldBegin;
    p->writeFieldEnd = t_binary_protocol_writeEndCommon;
    p->writeFieldStop = t_binary_protocol_writeFieldStop;
    p->writeMapBegin = t_binary_protocol_writeMapBegin;
    p->writeMapEnd = t_binary_protocol_writeEndCommon;
    p->writeListBegin = t_binary_protocol_writeListBegin;
    p->writeListEnd = t_binary_protocol_writeEndCommon;
    p->writeSetBegin = t_binary_protocol_writeListBegin;
    p->writeSetEnd = t_binary_protocol_writeEndCommon;
    p->writeBool = (int (*)(struct t_protocol *, bool))t_binary_protocol_writeI8;
    p->writeByte = t_binary_protocol_writeI8;
    p->writeI16 = t_binary_protocol_writeI16;
    p->writeI32 = t_binary_protocol_writeI32;
    p->writeI64 = t_binary_protocol_writeI64;
    p->writeDouble = t_binary_protocol_writeDouble;
    p->writeString = t_binary_protocol_writeString;

    p->readMessageBegin = t_binary_protocol_readMessageBegin;
    p->readMessageEnd = t_binary_protocol_readEndCommon;
    p->readStructBegin = t_binary_protocol_readStructBegin;
    p->readStructEnd = t_binary_protocol_readEndCommon;
    p->readFieldBegin = t_binary_protocol_readFieldBegin;
    p->readFieldEnd = t_binary_protocol_readEndCommon;
    p->readMapBegin = t_binary_protocol_readMapBegin;
    p->readMapEnd = t_binary_protocol_readEndCommon;
    p->readListBegin = t_binary_protocol_readListBegin;
    p->readListEnd = t_binary_protocol_readEndCommon;
    p->readSetBegin = t_binary_protocol_readListBegin;
    p->readSetEnd = t_binary_protocol_readEndCommon;
    p->readBool = t_binary_protocol_readBool;
    p->readByte = t_binary_protocol_readI8;
    p->readI16 = t_binary_protocol_readI16;
    p->readI32 = t_binary_protocol_readI32;
    p->readI64 = t_binary_protocol_readI64;
    p->readDouble = t_binary_protocol_readDouble;
    p->readString = t_binary_protocol_readString;

    p->trans = t;
    p->bo = bo;

    return 0;
}