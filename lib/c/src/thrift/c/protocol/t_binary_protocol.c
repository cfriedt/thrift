#include <assert.h>
#include <errno.h>
#include <string.h>

#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/transport/t_transport.h"

#define VERSION_1 0x80010000
#define VERSION_MASK 0xffff0000

static int t_binary_protocol_write_message_begin(struct t_protocol* p,
                                                 const char* name,
                                                 enum t_message_type message_type,
                                                 uint32_t seq) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  int32_t version = VERSION_1 | message_type;
  uint32_t wsize = 0;
  int r;

  if (p == NULL || name == NULL || !t_message_type_is_valid(message_type)) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_i32(p, version);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_string(p, strlen(name), name);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_i32(p, seq);
  if (r < 0) {
    return r;
  }

  wsize += r;

  return wsize;
}

// this is used as a number of writeXXXEnd() implementations
static int t_binary_protocol_write_end_common(struct t_protocol* p) {
  if (p == NULL) {
    return -EINVAL;
  }

  return 0;
}

static int t_binary_protocol_write_struct_begin(struct t_protocol* p, const char* name) {
  (void)name;

  if (p == NULL) {
    return -EINVAL;
  }

  return 0;
}

static int t_binary_protocol_write_field_begin(struct t_protocol* p,
                                               const char* name,
                                               enum t_type field_type,
                                               int field_id) {
  (void)name;

  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t wsize = 0;
  int r;

  if (p == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_byte(p, field_type);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_i16(p, field_id);
  if (r < 0) {
    return r;
  }

  wsize += r;

  return wsize;
}

static int t_binary_protocol_write_field_stop(struct t_protocol* p) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t wsize = 0;
  int r;

  if (p == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_byte(p, T_STOP);
  if (r < 0) {
    return r;
  }

  wsize += r;

  return wsize;
}

static int t_binary_protocol_write_map_begin(struct t_protocol* p,
                                             enum t_type ktype,
                                             enum t_type vtype,
                                             uint32_t size) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t wsize = 0;
  int r;

  if (p == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_byte(p, ktype);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_byte(p, vtype);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_i32(p, size);
  if (r < 0) {
    return r;
  }

  wsize += r;

  return wsize;
}

// exactly the same implementation for List and Set
static int t_binary_protocol_write_list_begin(struct t_protocol* p,
                                              enum t_type etype,
                                              uint32_t size) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t wsize = 0;
  int r;

  if (p == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_byte(p, etype);
  if (r < 0) {
    return r;
  }

  wsize += r;

  r = p->write_i32(p, size);
  if (r < 0) {
    return r;
  }

  wsize += r;

  return wsize;
}

#define to8(x) x
#define to16(x) p_->byte_order->to_wire16(x)
#define to32(x) p_->byte_order->to_wire32(x)
#define to64(x) p_->byte_order->to_wire64(x)

#define D_WRITEI(n)                                                                                \
  static int t_binary_protocol_write_i##n(struct t_protocol* p, int##n##_t x) {                    \
    struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;                             \
    int r;                                                                                         \
                                                                                                   \
    if (p == NULL) {                                                                               \
      return -EINVAL;                                                                              \
    }                                                                                              \
                                                                                                   \
    assert(p_->trans != NULL);                                                                     \
    assert(t_byte_order_is_valid(p_->byte_order));                                                 \
                                                                                                   \
    x = to##n(x);                                                                                  \
                                                                                                   \
    r = p_->trans->write_all(p_->trans, &x, n / 8);                                                \
    if (r < 0) {                                                                                   \
      return r;                                                                                    \
    }                                                                                              \
                                                                                                   \
    return n / 8;                                                                                  \
  }

D_WRITEI(8);
D_WRITEI(16);
D_WRITEI(32);
D_WRITEI(64);

static inline int t_binary_protocol_write_double(struct t_protocol* p, double x) {
  union bitwise_cast {
    int64_t i64;
    double f64;
  } cast;

  cast.f64 = x;

  return t_binary_protocol_write_i64(p, cast.i64);
}

static int t_binary_protocol_write_string(struct t_protocol* p, uint32_t size, const char* s) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t wsize = 0;
  int r;

  if (p == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->write_i32(p, size);
  if (r < 0) {
    return r;
  }

  wsize += r;

  if (size > 0) {
    r = p_->trans->write_all(p_->trans, s, size);
    if (r < 0) {
      return r;
    }

    wsize += r;
  }

  return wsize;
}

static int t_binary_protocol_read_message_begin(struct t_protocol* p,
                                                uint32_t* len,
                                                char** name,
                                                enum t_message_type* type,
                                                uint32_t* seq) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
  uint32_t rsize = 0;
  int32_t version;
  int r;

  if (p == NULL || len == NULL || name == NULL || type == NULL || seq == NULL) {
    return -EINVAL;
  }

  assert(p_->trans != NULL);

  r = p->read_i32(p, &version);
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

  r = p->read_string(p, len, name);
  if (r < 0) {
    return r;
  }

  rsize += r;

  r = p->read_i32(p, (int32_t*)seq);
  if (r < 0) {
    return r;
  }

  rsize += r;

  return rsize;
}

// this is used as a number of readXXXEnd() implementations
static int t_binary_protocol_read_end_common(struct t_protocol* p) {
  if (p == NULL) {
    return -EINVAL;
  }

  return 0;
}

static int t_binary_protocol_read_struct_begin(struct t_protocol* p, uint32_t* len, char** name) {
  (void)len;
  (void)name;

  if (p == NULL) {
    return -EINVAL;
  }

  return 0;
}

static int t_binary_protocol_read_field_begin(struct t_protocol* p,
                                              uint32_t* len,
                                              char** name,
                                              enum t_type* field_type,
                                              uint16_t* field_id) {
  int rsize = 0;
  int r;
  uint8_t byte;

  if (p == NULL || len == NULL || name == NULL || field_type == NULL || field_id == NULL) {
    return -EINVAL;
  }

  r = p->read_byte(p, &byte);
  if (r < 0) {
    return r;
  }

  rsize += r;

  *field_type = byte;

  r = p->read_i16(p, (int16_t*)field_id);
  if (r < 0) {
    return r;
  }

  rsize += r;

  return rsize;
}

static int t_binary_protocol_read_map_begin(struct t_protocol* p,
                                            enum t_type* ktype,
                                            enum t_type* vtype,
                                            uint32_t* size) {
  int rsize = 0;
  int r;
  uint8_t byte;

  if (p == NULL || ktype == NULL || vtype == NULL || size == NULL) {
    return -EINVAL;
  }

  r = p->read_byte(p, &byte);
  if (r < 0) {
    return r;
  }

  rsize += r;

  *ktype = byte;

  r = p->read_byte(p, &byte);
  if (r < 0) {
    return r;
  }

  rsize += r;

  *vtype = byte;

  r = p->read_i32(p, (int32_t*)size);
  if (r < 0) {
    return r;
  }

  rsize += r;

  return rsize;
}

// exactly the same implementation for List and Set
static int t_binary_protocol_read_list_begin(struct t_protocol* p,
                                             enum t_type* etype,
                                             uint32_t* size) {
  int rsize = 0;
  int r;
  uint8_t byte;

  if (p == NULL || etype == NULL || size == NULL) {
    return -EINVAL;
  }

  r = p->read_byte(p, &byte);
  if (r < 0) {
    return r;
  }

  rsize += r;

  *etype = byte;

  r = p->read_i32(p, (int32_t*)size);
  if (r < 0) {
    return r;
  }

  rsize += r;

  return rsize;
}

#define from8(x) x
#define from16(x) p_->byte_order->from_wire16(x)
#define from32(x) p_->byte_order->from_wire32(x)
#define from64(x) p_->byte_order->from_wire64(x)

#define D_READI(n)                                                                                 \
  static int t_binary_protocol_read_i##n(struct t_protocol* p, int##n##_t* x) {                    \
    struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;                             \
    int r;                                                                                         \
                                                                                                   \
    if (p == NULL || x == NULL) {                                                                  \
      return -EINVAL;                                                                              \
    }                                                                                              \
                                                                                                   \
    assert(p_->trans != NULL);                                                                     \
                                                                                                   \
    r = p_->trans->read_all(p_->trans, x, n / 8);                                                  \
    if (r < 0) {                                                                                   \
      return r;                                                                                    \
    }                                                                                              \
                                                                                                   \
    assert(t_byte_order_is_valid(p_->byte_order));                                                 \
                                                                                                   \
    *x = from##n(*x);                                                                              \
                                                                                                   \
    return n / 8;                                                                                  \
  }

D_READI(8);
D_READI(16);
D_READI(32);
D_READI(64);

static inline int t_binary_protocol_read_bool(struct t_protocol* p, bool* x) {
  int r;
  int8_t i8;

  r = t_binary_protocol_read_i8(p, (x == NULL) ? NULL : &i8);
  if (1 == r) {
    *x = i8 != 0;
  }

  return r;
}

static inline int t_binary_protocol_read_double(struct t_protocol* p, double* x) {
  int r;

  union bitwise_cast {
    int64_t i64;
    double f64;
  } cast;

  r = t_binary_protocol_read_i64(p, (x == NULL) ? NULL : &cast.i64);
  if (8 == r) {
    *x = cast.f64;
  }

  return r;
}

static int t_binary_protocol_read_string(struct t_protocol* p, uint32_t* size, char** s) {
  struct t_binary_protocol* const p_ = (struct t_binary_protocol*)p;
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
  r = p->read_i32(p, &ssize);
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
    r = p_->trans->read_all(p_->trans, *s, *size);
    if (r < 0) {
      return r;
    }
  }

  return 4 + *size;
}

int t_binary_protocol_init(struct t_binary_protocol* protocol,
                           struct t_transport* transport,
                           const struct t_byte_order* byte_order) {

  if (protocol == NULL || !t_transport_is_valid(transport)) {
    return -EINVAL;
  }

  if (byte_order == NULL) {
    byte_order = &t_network_big_endian;
  }

  if (!t_byte_order_is_valid(byte_order)) {
    return -EINVAL;
  }

  protocol->write_message_begin = t_binary_protocol_write_message_begin;
  protocol->write_message_end = t_binary_protocol_write_end_common;
  protocol->write_struct_begin = t_binary_protocol_write_struct_begin;
  protocol->write_struct_end = t_binary_protocol_write_end_common;
  protocol->write_field_begin = t_binary_protocol_write_field_begin;
  protocol->write_field_end = t_binary_protocol_write_end_common;
  protocol->write_field_stop = t_binary_protocol_write_field_stop;
  protocol->write_map_begin = t_binary_protocol_write_map_begin;
  protocol->write_map_end = t_binary_protocol_write_end_common;
  protocol->write_list_begin = t_binary_protocol_write_list_begin;
  protocol->write_list_end = t_binary_protocol_write_end_common;
  protocol->write_set_begin = t_binary_protocol_write_list_begin;
  protocol->write_set_end = t_binary_protocol_write_end_common;
  protocol->write_bool = (int (*)(struct t_protocol*, bool))t_binary_protocol_write_i8;
  protocol->write_byte = t_binary_protocol_write_i8;
  protocol->write_i16 = t_binary_protocol_write_i16;
  protocol->write_i32 = t_binary_protocol_write_i32;
  protocol->write_i64 = t_binary_protocol_write_i64;
  protocol->write_double = t_binary_protocol_write_double;
  protocol->write_string = t_binary_protocol_write_string;

  protocol->read_message_begin = t_binary_protocol_read_message_begin;
  protocol->read_message_end = t_binary_protocol_read_end_common;
  protocol->read_struct_begin = t_binary_protocol_read_struct_begin;
  protocol->read_struct_end = t_binary_protocol_read_end_common;
  protocol->read_field_begin = t_binary_protocol_read_field_begin;
  protocol->read_field_end = t_binary_protocol_read_end_common;
  protocol->read_map_begin = t_binary_protocol_read_map_begin;
  protocol->read_map_end = t_binary_protocol_read_end_common;
  protocol->read_list_begin = t_binary_protocol_read_list_begin;
  protocol->read_list_end = t_binary_protocol_read_end_common;
  protocol->read_set_begin = t_binary_protocol_read_list_begin;
  protocol->read_set_end = t_binary_protocol_read_end_common;
  protocol->read_bool = t_binary_protocol_read_bool;
  protocol->read_byte = t_binary_protocol_read_i8;
  protocol->read_i16 = t_binary_protocol_read_i16;
  protocol->read_i32 = t_binary_protocol_read_i32;
  protocol->read_i64 = t_binary_protocol_read_i64;
  protocol->read_double = t_binary_protocol_read_double;
  protocol->read_string = t_binary_protocol_read_string;

  protocol->trans = transport;
  protocol->byte_order = byte_order;

  return protocol->trans->open(protocol->trans);
}

static int t_binary_protocol_factory_put_protocol(struct t_protocol_factory* factory,
                                                  struct t_protocol* protocol) {

  struct t_binary_protocol* const bp = (struct t_binary_protocol*)protocol;

  if (factory == NULL || protocol == NULL) {
    return -EINVAL;
  }

  return bp->trans->close(bp->trans);
}

static int t_binary_protocol_factory_get_protocol(struct t_protocol_factory* factory,
                                                  struct t_transport* transport,
                                                  struct t_protocol** protocol) {

  struct t_binary_protocol_factory* const bpf = (struct t_binary_protocol_factory*)factory;
  if (factory == NULL || protocol == NULL) {
    return -EINVAL;
  }

  return t_binary_protocol_init((struct t_binary_protocol*)*protocol, transport, bpf->byte_order);
}

int t_binary_protocol_factory_init_with_byte_order(struct t_binary_protocol_factory* factory,
                                                   const struct t_byte_order* byte_order) {
  if (factory == NULL || !t_byte_order_is_valid(byte_order)) {
    return -EINVAL;
  }

  factory->byte_order = byte_order;
  factory->get_protocol = t_binary_protocol_factory_get_protocol;
  factory->put_protocol = t_binary_protocol_factory_put_protocol;

  return 0;
}

int t_binary_protocol_factory_init(struct t_binary_protocol_factory* factory) {
  return t_binary_protocol_factory_init_with_byte_order(factory, &t_network_big_endian);
}