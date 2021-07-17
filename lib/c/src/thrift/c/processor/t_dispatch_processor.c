#include <errno.h>
#include <string.h>

#include "t_dispatch_processor.h"

static int t_dispatch_processor_process(struct t_processor* proc,
                                        struct t_protocol* in,
                                        struct t_protocol* out) {

  int r;
  uint32_t seqid;
  uint32_t fname_len;
  enum t_message_type mtype;
  struct t_dispatch_processor* const tdp = (struct t_dispatch_processor*)proc;

  if (proc == NULL || in == NULL || out == NULL) {
    return -EINVAL;
  }

  fname_len = tdp->method_name_buffer_size;
  r = in->read_message_begin(in, &fname_len, &tdp->method_name_buffer, &mtype, &seqid);
  char* const fname = tdp->method_name_buffer;

  if (mtype != T_CALL && mtype != T_ONEWAY) {
    return -EINVAL;
  }

  return tdp->dispatch(tdp, in, out, fname, fname_len, seqid);
}

static int t_dispatch_processor_dispatch(struct t_dispatch_processor* proc,
                                         struct t_protocol* in,
                                         struct t_protocol* out,
                                         const char* fname,
                                         uint32_t fname_len,
                                         uint32_t seqid) {

  size_t i;
  t_dispatch_method_processor processor = NULL;

  if (proc == NULL || in == NULL || out == NULL || fname == NULL) {
    return -EINVAL;
  }

  for (i = 0; i < proc->n_processors; ++i) {
    if (fname_len == proc->processors[i].method_name_size
        && strncmp(proc->processors[i].method_name, fname, fname_len) == 0) {
      processor = proc->processors[i].method_processor;
      break;
    }
  }

  if (processor == NULL) {
    // in->skip(in, ::T_STRUCT);
    in->read_message_end(in);
    // in->get_transport()->readEnd();
    // ::apache::thrift::TApplicationException
    //     x(::apache::thrift::TApplicationException::UNKNOWN_METHOD,
    //       "Invalid method name: '" + fname + "'");
    // out->write_message_begin(out, fname, ::T_EXCEPTION, seqid);
    // x.write(oprot);
    out->write_message_end(out);
    // oprot->getTransport()->writeEnd();
    // oprot->getTransport()->flush();
    return 0;
  }

  return processor(proc, seqid, in, out);
}

int t_dispatch_processor_init(struct t_dispatch_processor* proc,
                              char* method_name_buffer,
                              size_t method_name_buffer_size,
                              size_t n_processors,
                              struct t_dispatch_method_processor_desc* processors) {

  if (proc == NULL || (n_processors != 0 && processors == NULL)) {
    return -EINVAL;
  }

  proc->process = t_dispatch_processor_process;
  proc->dispatch = t_dispatch_processor_dispatch;
  proc->method_name_buffer = method_name_buffer;
  proc->method_name_buffer_size = method_name_buffer_size;
  proc->n_processors = n_processors;
  proc->processors = processors;

  return 0;
}

bool t_dispatch_processor_is_valid(struct t_dispatch_processor* p) {
  return !(p == NULL || p->process == NULL || p->dispatch == NULL || p->method_name_buffer == NULL
           || p->processors == NULL);
}
