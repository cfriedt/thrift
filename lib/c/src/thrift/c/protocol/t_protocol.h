#ifndef THRIFT_C_T_PROTOCOL_H_
#define THRIFT_C_T_PROTOCOL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <thrift/c/protocol/t_enum.h>

struct t_byte_order {
  uint16_t (*toWire16)(uint16_t x);
  uint32_t (*toWire32)(uint32_t x);
  uint64_t (*toWire64)(uint64_t x);
  uint16_t (*fromWire16)(uint16_t x);
  uint32_t (*fromWire32)(uint32_t x);
  uint64_t (*fromWire64)(uint64_t x);
};

extern const struct t_byte_order t_network_big_endian;
extern const struct t_byte_order t_network_little_endian;

static inline bool t_byte_order_is_valid(const struct t_byte_order *bo)
{
    return !(false
        || bo->toWire16 == NULL
        || bo->toWire32 == NULL
        || bo->toWire64 == NULL
        || bo->fromWire16 == NULL
        || bo->fromWire32 == NULL
        || bo->fromWire64 == NULL
    );
}

#define T_PROTOCOL_METHODS \
    int (*writeMessageBegin)(struct t_protocol *p, const char *name, enum t_message_type message_type, uint32_t seq); \
    int (*writeMessageEnd)(struct t_protocol *p); \
    int (*writeStructBegin)(struct t_protocol *p, const char *name); \
    int (*writeStructEnd)(struct t_protocol *p); \
    int (*writeFieldBegin)(struct t_protocol *p, const char *name, enum t_type field_type, int field_id); \
    int (*writeFieldEnd)(struct t_protocol *p); \
    int (*writeFieldStop)(struct t_protocol *p); \
    int (*writeMapBegin)(struct t_protocol *p, enum t_type ktype, enum t_type vtype, uint32_t size); \
    int (*writeMapEnd)(struct t_protocol *p); \
    int (*writeListBegin)(struct t_protocol *p, enum t_type etype, uint32_t size); \
    int (*writeListEnd)(struct t_protocol *p); \
    int (*writeSetBegin)(struct t_protocol *p, enum t_type etype, uint32_t size); \
    int (*writeSetEnd)(struct t_protocol *p); \
    int (*writeBool)(struct t_protocol *p, bool b); \
    int (*writeByte)(struct t_protocol *p, uint8_t b); \
    int (*writeI16)(struct t_protocol *p, int16_t s); \
    int (*writeI32)(struct t_protocol *p, int32_t w); \
    int (*writeI64)(struct t_protocol *p, int64_t l); \
    int (*writeDouble)(struct t_protocol *p, double d); \
    int (*writeString)(struct t_protocol *p, uint32_t size, const char *s); \
    \
    int (*readMessageBegin)(struct t_protocol *p, char **name, enum t_message_type *message_type, uint32_t *seq); \
    int (*readMessageEnd)(struct t_protocol *p); \
    int (*readStructBegin)(struct t_protocol *p, uint32_t *len, char **name); \
    int (*readStructEnd)(struct t_protocol *p); \
    int (*readFieldBegin)(struct t_protocol *p, uint32_t *len, char **name, enum t_type *field_type, uint16_t *field_id); \
    int (*readFieldEnd)(struct t_protocol *p); \
    int (*readMapBegin)(struct t_protocol *p, enum t_type *ktype, enum t_type *vtype, uint32_t *size); \
    int (*readMapEnd)(struct t_protocol *p); \
    int (*readListBegin)(struct t_protocol *p, enum t_type *etype, uint32_t *size); \
    int (*readListEnd)(struct t_protocol *p); \
    int (*readSetBegin)(struct t_protocol *p, enum t_type *etype, uint32_t *size); \
    int (*readSetEnd)(struct t_protocol *p); \
    int (*readBool)(struct t_protocol *p, bool *b); \
    int (*readByte)(struct t_protocol *p, uint8_t *b); \
    int (*readI16)(struct t_protocol *p, int16_t *s); \
    int (*readI32)(struct t_protocol *p, int32_t *w); \
    int (*readI64)(struct t_protocol *p, int64_t *l); \
    int (*readDouble)(struct t_protocol *p, double *d); \
    int (*readString)(struct t_protocol *p, uint32_t *size, char **s);

struct t_protocol;
struct t_protocol {
  T_PROTOCOL_METHODS;
};

#endif /* THRIFT_C_T_PROTOCOL_H_ */
