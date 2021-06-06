#ifndef THRIFT_C_T_TRANSPORT_H_
#define THRIFT_C_T_TRANSPORT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct t_transport;

#define T_TRANSPORT_METHODS \
    int (*open)(struct t_transport *t); \
    int (*close)(struct t_transport *t); \
    bool (*isOpen)(struct t_transport *t); \
    int (*read)(struct t_transport *t, void *buf, uint32_t size); \
    int (*write)(struct t_transport *t, const void *buf, uint32_t size); \
    int (*flush)(struct t_transport *t); \
    int (*readEnd)(struct t_transport *t); \
    int (*writeEnd)(struct t_transport *t);

struct t_transport {
    T_TRANSPORT_METHODS;
};

#endif /* THRIFT_C_T_TRANSPORT_H_ */
