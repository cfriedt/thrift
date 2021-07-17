#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#include "t_server_socket.h"
#include "t_socket.h"
#include "thrift/c/thrift.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

extern int t_server_transport_init(struct t_server_transport* t);
extern int t_socket_init_fd(struct t_socket* t, int fd);

#include <pthread.h>
#include <stdio.h>
#define D(fmt, args...)                                                                            \
  printf("%p: %s(): %d: " fmt "\n", pthread_self(), __func__, __LINE__, ##args)

static int t_server_socket_listen(struct t_server_transport* t) {
  struct t_server_socket* const tss = (struct t_server_socket*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  return listen(tss->sd, tss->backlog);
}

static struct t_socket temp;
static int t_server_socket_accept(struct t_server_transport* t, struct t_transport** xport) {
  int r;
  int client_fd;
  struct pollfd fds[2];
  struct t_socket* const ts = (struct t_socket*)&temp;
  struct t_server_socket* const tss = (struct t_server_socket*)t;
  static struct sockaddr_storage sa;
  socklen_t len;

  if (t == NULL || xport == NULL) {
    return -EINVAL;
  }

  fds[0].fd = tss->sd;
  fds[0].events = POLLIN;
  fds[1].fd = tss->cancel[1];
  fds[1].events = POLLIN;
  r = poll(fds, 2, -1);
  if (r < 0) {
    return -errno;
  }

  if ((fds[1].revents & POLLIN) != 0) {
    return -EINTR;
  }

  len = sizeof(sa);
  r = accept(tss->sd, (struct sockaddr*)&sa, &len);
  if (r < 0) {
    return -errno;
  }

  client_fd = r;
  r = t_socket_init_fd((struct t_socket*)ts, client_fd);
  if (r < 0) {
    goto close_sock;
  }

  *xport = (struct t_transport*)ts;

  r = 0;
  goto out;

close_sock:
  D("shutting down client socket");
  shutdown(client_fd, SHUT_RDWR);

out:
  return r;
}

int t_server_socket_close(struct t_server_transport* t) {
  struct t_server_socket* const tss = (struct t_server_socket*)t;

  if (!t_server_transport_is_valid(t)) {
    return -EINVAL;
  }

  t->interrupt(t);
  t->interrupt_children(t);

  shutdown(tss->sd, SHUT_RDWR);
  close(tss->cancel[0]);
  close(tss->cancel[1]);
  tss->sd = -1;
  tss->cancel[0] = tss->cancel[1] = -1;

  return 0;
}

int t_server_socket_interrupt(struct t_server_transport* t) {
  int r;
  struct t_server_socket* const tss = (struct t_server_socket*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  r = write(tss->cancel[0], "x", 1);
  if (r < 0) {
    return -errno;
  }

  // FIXME: should wait until ! listening

  return 0;
}

int t_server_socket_init_port(struct t_server_socket* t, uint16_t port) {
  return t_server_socket_init(t, "::", port);
}

int t_server_socket_init_path(struct t_server_socket* t, const char* path) {
  return t_server_socket_init(t, path, 0);
}

int t_server_socket_init(struct t_server_socket* t, const char* addr, uint16_t port) {

  int r;
  int af;
  union {
    struct sockaddr_in in;
    struct sockaddr_in6 in6;
    struct sockaddr_un un;
  } sa;
  struct addrinfo* res;
  socklen_t len;

  if (t == NULL || addr == NULL) {
    return -EINVAL;
  }

  r = t_server_transport_init((struct t_server_transport*)t);
  if (r < 0) {
    return r;
  }

  t->listen = t_server_socket_listen;
  t->accept = t_server_socket_accept;
  t->close = t_server_socket_close;
  t->interrupt = t_server_socket_interrupt;
  // TODO: look at a callback-based approach to handling concurrent connections
  // similar to what TServerSocket does
  // t->interrupt_children = t_server_socket_interrupt_children;

  t->sd = -1;
  t->backlog = 1;
  t->port = port;
  t->cancel[0] = t->cancel[1] = -1;

  memset(&sa, 0, sizeof(sa));

  af = thrift_c_addr_family(addr);
  if (af < 0) {
    return af;
  }

  switch (af) {
  case AF_UNIX:
    sa.un.sun_len = sizeof(sa.un);
    sa.un.sun_family = AF_UNIX;
    strncpy(sa.un.sun_path, addr, sizeof(sa.un.sun_path));
    break;
  case AF_INET:
  case AF_INET6:
    r = getaddrinfo(addr, NULL, NULL, &res);
    if (r < 0) {
      return -EINVAL;
    }
    memcpy(&sa, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    res = NULL;
    sa.in.sin_port = htons(port);
    break;
  default:
    break;
  }

  if (!(af == AF_UNIX || af == AF_INET || af == AF_INET6)) {
    return -EAFNOSUPPORT;
  }

  r = socket(af, SOCK_STREAM, 0);
  if (r < 0) {
    return -errno;
  }

  t->sd = r;

  if (af == AF_INET || af == AF_INET6) {
    r = 1;
    r = setsockopt(t->sd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(int));
    if (r < 0) {
      r = -errno;
      goto close_socket;
    }
  }

  r = bind(t->sd, (struct sockaddr*)&sa, sa.in.sin_len);
  if (r < 0) {
    r = -errno;
    goto close_socket;
  }

  if (port == 0 && (af == AF_INET || af == AF_INET6)) {
    len = sizeof(sa);
    r = getsockname(t->sd, (struct sockaddr*)&sa, &len);
    if (r < 0) {
      goto close_socket;
    }

    t->port = ntohs(sa.in.sin_port);
  }

  r = socketpair(AF_UNIX, SOCK_STREAM, 0, t->cancel);
  if (r < 0) {
    r = -errno;
    goto close_socket;
  }

  r = 0;
  goto out;

close_socket:
  close(t->sd);
  t->sd = -1;

out:
  return r;
}
