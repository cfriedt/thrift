#pragma once

#include <thrift/c/processor/t_dispatch_processor.h>

#include "thrift_test_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

struct thrift_test_processor {
  T_PROCESSOR_METHODS;
  T_DISPATCH_PROCESSOR_METHODS;
  T_DISPATCH_PROCESSOR_FIELDS;
  struct thrift_test_handler* handler;
};

int thrift_test_processor_init(struct thrift_test_processor* p,
                               struct thrift_test_handler* handler);

#ifdef __cplusplus
}
#endif
