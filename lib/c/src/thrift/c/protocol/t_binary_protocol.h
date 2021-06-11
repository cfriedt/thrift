#ifndef THRIFT_C_T_BINARY_PROTOCOL_H_
#define THRIFT_C_T_BINARY_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

#include <thrift/c/protocol/t_protocol.h>
#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

struct t_binary_protocol {
    T_PROTOCOL_METHODS;

    struct t_transport *trans;
    const struct t_byte_order *bo;
};

/* TODO: add endianness support */
int t_binary_protocol_init(struct t_binary_protocol *p, struct t_transport *t, const struct t_byte_order *bo);

#ifdef __cplusplus
}
#endif

#endif /* THRIFT_C_T_PROTOCOL_H_ */
