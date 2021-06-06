#ifndef THRIFT_C_T_SERVER_TRANSPORT_H_
#define THRIFT_C_T_SERVER_TRANSPORT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <thrift/c/transport/t_transport.h>

#ifndef __cplusplus
extern "C" {
#endif

struct t_server_transport;
struct t_server_transport {
    int (*open)(struct t_server_transport *t);
    int (*listen)(struct t_server_transport *t);
    int (*accept)(struct t_server_transport *t, struct t_transport **client);
    int (*close)(struct t_server_transport *t);
};

#ifndef __cplusplus
}
#endif

#endif /* THRIFT_C_T_TRANSPORT_H_ */