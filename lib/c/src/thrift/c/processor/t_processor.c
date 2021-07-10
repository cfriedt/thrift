#include "thrift/c/processor/t_processor.h"

bool t_processor_is_valid(struct t_processor* p) {
  return !(p == NULL || p->process == NULL);
}