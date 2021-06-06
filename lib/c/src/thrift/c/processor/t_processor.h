#ifndef THRIFT_C_T_PROCESSOR_H_
#define THRIFT_C_T_PROCESSOR_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <thrift/c/protocol/t_protocol.h>

struct t_processor;
struct t_processor {
    int (*process)(struct t_processor *p, struct t_protocol *in, struct t_protocol *out);
};

#ifdef __cplusplus
}
#endif

#endif /* THRIFT_C_T_PROCESSOR_H_ */