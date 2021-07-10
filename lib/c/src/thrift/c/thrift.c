#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "thrift.h"

int thrift_c_addr_family(const char* addr) {
  int r;
  struct addrinfo* res = NULL;

  if (addr == NULL || strlen(addr) == 0) {
    return -EINVAL;
  }

  if (addr[0] == '/') {
    return AF_UNIX;
  }

  r = getaddrinfo(addr, NULL, NULL, &res);
  if (r < 0) {
    return -EINVAL;
  }

  r = res->ai_addr->sa_family;
  freeaddrinfo(res);

  return r;
}
