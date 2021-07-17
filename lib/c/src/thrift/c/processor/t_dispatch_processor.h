#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <thrift/c/processor/t_processor.h>

struct t_dispatch_processor;

typedef int (*t_dispatch_method_processor)(struct t_dispatch_processor* proc,
                                           uint32_t seqid,
                                           struct t_protocol* in,
                                           struct t_protocol* out);

struct t_dispatch_method_processor_desc {
  const char* method_name;
  uint32_t method_name_size;
  t_dispatch_method_processor method_processor;
};

#define T_DISPATCH_PROCESSOR_DESC(name, func)                                                      \
  { .method_name = name, .method_name_size = sizeof(name) - 1, .method_processor = func }

#define T_DISPATCH_PROCESSOR_METHODS                                                               \
  int (*dispatch)(struct t_dispatch_processor * proc, struct t_protocol * in,                      \
                  struct t_protocol * out, const char* fname, uint32_t fname_len, uint32_t seqid)

#define T_DISPATCH_PROCESSOR_FIELDS                                                                \
  char* method_name_buffer;                                                                        \
  size_t method_name_buffer_size;                                                                  \
  size_t n_processors;                                                                             \
  struct t_dispatch_method_processor_desc* processors

struct t_dispatch_processor {
  T_PROCESSOR_METHODS;
  T_DISPATCH_PROCESSOR_METHODS;
  T_DISPATCH_PROCESSOR_FIELDS;
};

bool t_dispatch_processor_is_valid(struct t_dispatch_processor* p);

#ifdef __cplusplus
}
#endif
