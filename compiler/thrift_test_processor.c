#include <errno.h>
#include <string.h>

#include <thrift/c/thrift.h>
#include <thrift/c/transport/t_transport.h>

#include "thrift_test_processor.h"

extern int t_dispatch_processor_init(struct t_dispatch_processor* proc,
                                     char* method_name_buffer,
                                     size_t method_name_buffer_size,
                                     size_t n_methods,
                                     const struct t_dispatch_method_processor_desc* methods);

struct ThriftTest_testVoid_args {};
struct ThriftTest_testVoid_result {};

static int ThriftTest_testVoid_args_read(struct t_protocol* in,
                                         struct ThriftTest_testVoid_args* args) {
  (void)args;

  int r;
  int bytes_read = 0;
  enum t_type ftype;
  int16_t fid;
  char* fname = NULL;
  uint32_t fname_len = 0;

  // read arguments
  D("calling read_struct_begin()");
  char* sname = NULL;
  uint32_t sname_len = 0;
  r = in->read_struct_begin(in, &sname_len, &sname);
  if (r < 0) {
    E("read_struct_begin_failed(): %s", strerror(-r));
    return r;
  }

  bytes_read += r;

  while (true) {
    D("calling read_field_begin()");
    r = in->read_field_begin(in, &fname_len, &fname, &ftype, &fid);
    if (r < 0) {
      E("read_field_begin() failed: %s", strerror(-r));
      return r;
    }

    bytes_read += r;

    if (ftype == T_STOP) {
      D("received t_type T_STOP");
      break;
    }

    D("calling skip(%d)", ftype);
    r = in->skip(in, ftype);
    if (r < 0) {
      E("skip(%d) failed: %s", ftype, strerror(-r));
      return r;
    }

    bytes_read += r;

    D("calling read_field_end()");
    r = in->read_field_end(in);
    if (r < 0) {
      E("read_field_end() failed: %s", strerror(-r));
      return r;
    }

    bytes_read += r;
  }

  r = in->read_struct_end(in);
  if (r < 0) {
    E("read_struct_end() failed: %s", strerror(-r));
    return r;
  }

  bytes_read += r;

  return bytes_read;
}

static int ThriftTest_testVoid_result_write(struct t_protocol* out,
                                            struct ThriftTest_testVoid_result* result) {
  (void)result;

  int r;
  int bytes_written = 0;

  D("calling write_struct_begin()");
  r = out->write_struct_begin(out, "ThriftTest_testVoid_result");
  if (r < 0) {
    E("write_struct_begin() failed: %s", strerror(-r));
    return r;
  }

  bytes_written += r;

  r = out->write_field_stop(out);
  if (r < 0) {
    E("write_field_stop() failed: %s", strerror(-r));
    return r;
  }

  bytes_written += r;

  r = out->write_struct_end(out);
  if (r < 0) {
    E("write_struct_end() failed: %s", strerror(-r));
    return r;
  }

  bytes_written += r;

  return bytes_written;
}

static int thrift_test_processor_process_testVoid(struct t_dispatch_processor* proc,
                                                  uint32_t seqid,
                                                  struct t_protocol* in,
                                                  struct t_protocol* out) {

  int r;
  struct t_transport* xport;
  struct ThriftTest_testVoid_args args = {};
  struct ThriftTest_testVoid_result result = {};
  struct thrift_test_processor* const tdp = (struct thrift_test_processor*)proc;

  if (!(t_dispatch_processor_is_valid(proc) && t_protocol_is_valid(in) && t_protocol_is_valid(out)
        && t_transport_is_valid(in->get_transport(in))
        && t_transport_is_valid(out->get_transport(out)))) {
    return -EINVAL;
  }

  r = ThriftTest_testVoid_args_read(in, &args);
  if (r < 0) {
    E("ThriftTest_testVoid_args_read() failed: %s", strerror(-r));
    return r;
  }

  r = in->read_message_end(in);
  if (r < 0) {
    return r;
  }

  xport = in->get_transport(in);
  r = xport->read_end(xport);
  if (r < 0) {
    return r;
  }

  // handle the RPC message
  // normally, the return value would need to be populated,
  // any arguments of the RPC method would need to be populated,
  // exceptions might need to be thrown, etc.
  tdp->handler->testVoid();

  D("calling write_message_begin()");
  r = out->write_message_begin(out, "testVoid", T_REPLY, seqid);
  if (r < 0) {
    E("write_message_begin() failed: %s", strerror(-r));
    return r;
  }

  D("calling ThriftTest_testVoid_result_write()");
  r = ThriftTest_testVoid_result_write(out, &result);
  if (r < 0) {
    E("ThriftTest_testVoid_result_write() failed: %s", strerror(-r));
    return r;
  }

  D("calling write_message_end()");
  r = out->write_message_end(out);
  if (r < 0) {
    E("write_message_end() failed: %s", strerror(-r));
    return r;
  }

  D("calling write_end()");
  xport = in->get_transport(in);
  r = xport->write_end(xport);
  if (r < 0) {
    E("write_end() failed: %s", strerror(-r));
    return r;
  }

  D("calling flush()");
  r = xport->flush(xport);
  if (r < 0) {
    E("flush() failed: %s", strerror(-r));
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
