#include <thrift/thrift-config.h>

#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/PlatformSocket.h>
#include <boost/shared_ptr.hpp>

namespace apache {
namespace thrift {
namespace transport {

class TGetAddrInfoWrapper {
public:
  TGetAddrInfoWrapper(const char* node, const char* service, const struct addrinfo* hints);

  virtual ~TGetAddrInfoWrapper();

  int init();
  const struct addrinfo* res();

private:
  const char* node_;
  const char* service_;
  const struct addrinfo* hints_;
  struct addrinfo* res_;
};

}
}
} // apache::thrift::transport
