#ifndef THRIFT_C_T_BUFFERED_TRANSPORTS_H_
#define THRIFT_C_T_BUFFERED_TRANSPORTS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <thrift/c/transport/t_transport.h>

#ifdef __cplusplus
extern "C" {
#endif

#define T_BUFFERED_METHODS \
    int (*available_read)(struct t_memory_buffer *t); \
    int (*available_write)(struct t_memory_buffer *t)

#define T_BUFFERED_FIELDS \
    bool open_; \
    uint8_t *buffer_; \
    size_t bufferSize_; \
    uint8_t *rBase_; \
    uint8_t *rBound_; \
    uint8_t *wBase_; \
    uint8_t *wBound_

struct t_memory_buffer {
    T_TRANSPORT_METHODS;
    T_BUFFERED_METHODS;
    T_BUFFERED_FIELDS;
};

int t_memory_buffer_init(struct t_memory_buffer *t, void *buffer, size_t size);

struct t_buffered_transport {
    T_TRANSPORT_METHODS;
    T_BUFFERED_METHODS;
    T_BUFFERED_FIELDS;
    struct t_transport *trans;
};

int t_buffered_transport_init(struct t_buffered_transport *t, void *buffer, size_t size, struct t_transport *trans);

#ifdef __cplusplus
}
#endif

#endif /* THRIFT_C_T_BUFFERED_TRANSPORTS_H_ */
