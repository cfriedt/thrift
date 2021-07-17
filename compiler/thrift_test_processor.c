#include <errno.h>

#include <thrift/c/thrift.h>
#include <thrift/c/transport/t_transport.h>

#include "thrift_test_processor.h"

#include <pthread.h>
#include <stdio.h>
#define D(fmt, args...)                                                                            \
  printf("%p: %s(): %d: " fmt "\n", pthread_self(), __func__, __LINE__, ##args)

extern int t_dispatch_processor_init(struct t_dispatch_processor* proc,
                                     char* method_name_buffer,
                                     size_t method_name_buffer_size,
                                     size_t n_methods,
                                     const struct t_dispatch_method_processor_desc* methods);

static int thrift_test_processor_process_testVoid(struct t_dispatch_processor* proc,
                                                  uint32_t seqid,
                                                  struct t_protocol* in,
                                                  struct t_protocol* out) {

  int r;
  struct t_transport* xport;
  struct thrift_test_processor* const tdp = (struct thrift_test_processor*)proc;

  if (!(t_dispatch_processor_is_valid(proc) && t_protocol_is_valid(in)
        && t_protocol_is_valid(out))) {
    return -EINVAL;
  }

  // get underlying transport (for subsequent xport->read_end())
  xport = in->get_transport(in);
  if (!t_transport_is_valid(xport)) {
    return -EINVAL;
  }

  r = in->read_message_end(in);
  if (r < 0) {
    return r;
  }

  r = xport->read_end(xport);
  if (r < 0) {
    return r;
  }

  // handle the RPC message
  // normally, the return value would need to be populated,
  // any arguments of the RPC method would need to be populated,
  // exceptions might need to be thrown, etc.
  tdp->handler->testVoid();

  // get underlying transport (for subsequent xport->write_end())  xport = out->get_transport(out);
  if (!t_transport_is_valid(xport)) {
    return -EINVAL;
  }

  r = out->write_message_begin(out, "testVoid", T_REPLY, seqid);
  if (r < 0) {
    return r;
  }

  r = out->write_message_end(out);
  if (r < 0) {
    return r;
  }

  r = xport->write_end(xport);
  if (r < 0) {
    return r;
  }

  r = xport->flush(xport);
  if (r < 0) {
    return r;
  }

  return 0;
}

static const struct t_dispatch_method_processor_desc method_desc[] = {
    T_DISPATCH_PROCESSOR_DESC("testVoid", thrift_test_processor_process_testVoid),
};

static char method_name_buffer[1024];

int thrift_test_processor_init(struct thrift_test_processor* p,
                               struct thrift_test_handler* handler) {

  int r;
  struct t_dispatch_processor* const tdp = (struct t_dispatch_processor*)p;

  r = t_dispatch_processor_init(tdp, method_name_buffer, sizeof(method_name_buffer),
                                ARRAY_SIZE(method_desc), method_desc);
  if (r < 0) {
    return r;
  }

  p->handler = handler;
  return 0;
}
