#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <thrift/c/protocol/t_protocol.h>

#define T_PROCESSOR_METHODS                                                                        \
  int (*process)(struct t_processor * p, struct t_protocol * in, struct t_protocol * out)

struct t_processor;
struct t_processor {
  T_PROCESSOR_METHODS;
};

bool t_processor_is_valid(struct t_processor* p);

#ifdef __cplusplus
}
#endif
