#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "thrift/c/thrift.h"
#include "thrift/c/transport/t_socket.h"

extern int t_transport_init(struct t_transport* t);

bool t_socket_is_open(struct t_transport* t) {
  struct t_socket* sock = (struct t_socket*)t;

  if (t == NULL) {
    return false;
  }

  return sock->sd != THRIFT_INVALID_SOCKET;
}

int t_socket_close(struct t_transport* t) {
  int r;
  struct t_socket* sock = (struct t_socket*)t;

  if (t == NULL) {
    return -EINVAL;
  }

  r = close(sock->sd);
  if (r < 0) {
    return -errno;
  }

  sock->sd = THRIFT_INVALID_SOCKET;
  return 0;
}

bool t_socket_peek(struct t_transport* t) {

  if (!t_transport_is_valid(t)) {
    return false;
  }

  if (!t_socket_is_open(t)) {
    return false;
  }

  return t->available_read(t) > 0;
}

int t_socket_open(struct t_transport* t) {
  int r;
  struct addrinfo* res = NULL;
  uint8_t af;
  union {
    struct sockaddr sa;
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr_un sun;
  } sa;
  struct t_socket* const sock = (struct t_socket*)t;

  if (!t_transport_is_valid(t)) {
    return -EINVAL;
  }

  if (sock->host == NULL) {
    return -EINVAL;
  }

  memset(&sa, 0, sizeof(sa));

  r = thrift_c_addr_family(sock->host);
  if (r < 0) {
    return r;
  }

  af = r;

  switch (af) {
  case AF_UNIX:
    sa.sun.sun_family = AF_UNIX;
    strncpy(sa.sun.sun_path, sock->path, sizeof(sa.sun.sun_path));
    sa.sun.sun_len = sizeof(sa.sun);
    break;

  case AF_INET:
  case AF_INET6:
    r = getaddrinfo(sock->host, NULL, NULL, &res);
    if (r != 0) {
      r = -EINVAL;
      goto out;
    }

    memcpy(&sa, res->ai_addr, res->ai_addrlen);
    // same offset for  sin_port and sin6_port
    sa.sa4.sin_port = htons(sock->port);
    freeaddrinfo(res);
    res = NULL;
    break;
  }

  r = socket(af, SOCK_STREAM, 0);
  if (r < 0) {
    r = -errno;
    goto out;
  }

  sock->sd = r;

  r = connect(sock->sd, &sa.sa, sa.sa.sa_len);
  if (r < 0) {
    r = -errno;
    goto close_sock_sd;
  }

  r = 0;
  goto out;

close_sock_sd:
  close(sock->sd);
  sock->sd = THRIFT_INVALID_SOCKET;

out:
  return r;
}

/* implements thrifstruct t_transport *_read */
int t_socket_read(struct t_transport* t, void* buf, uint32_t len) {
  struct t_socket* const sock = (struct t_socket*)t;

  if (t == NULL || buf == NULL) {
    return -EINVAL;
  }

  if (len == 0) {
    return 0;
  }

  return recv(sock->sd, buf, len, 0);
}

int t_socket_write(struct t_transport* t, const void* buf, const uint32_t len) {
  struct t_socket* const sock = (struct t_socket*)t;

  if (t == NULL || buf == NULL) {
    return -EINVAL;
  }

  if (len == 0) {
    return 0;
  }

  return send(sock->sd, buf, len, 0);
}

int t_socket_available_read(struct t_transport* t) {
  int r;
  int avail = 0;
  struct t_socket* const sock = (struct t_socket*)t;

  if (!t_socket_is_open(t)) {
    return 0;
  }

  r = ioctl(sock->sd, FIONREAD, &avail);
  if (r < 0) {
    return -errno;
  }

  return avail;
}

int t_socket_available_write(struct t_transport* t) {
  int avail = 0;

  if (!t_transport_is_valid(t)) {
    return -EINVAL;
  }

  if (!t_socket_is_open(t)) {
    return 0;
  }

  avail = 42;

  return avail;
}

static char* t_socket_get_origin(struct t_transport* t) {

  if (t == NULL) {
    return NULL;
  }

  return "";
}

int t_socket_init(struct t_socket* t, const char* host, uint16_t port) {
  int r;

  if (t == NULL || host == NULL) {
    return -EINVAL;
  }

  r = t_transport_init((struct t_transport*)t);
  if (r < 0) {
    return -errno;
  }

  t->is_open = t_socket_is_open;
  t->peek = t_socket_peek;
  t->open = t_socket_open;
  t->close = t_socket_close;
  t->read = t_socket_read;
  t->write = t_socket_write;
  t->get_origin = t_socket_get_origin;
  t->available_read = t_socket_available_read;
  t->available_write = t_socket_available_write;

  t->host = host;
  t->port = port;
  t->sd = THRIFT_INVALID_SOCKET;

  return 0;
}

int t_socket_init_path(struct t_socket* t, const char* path) {
  return t_socket_init(t, path, 0);
}

#include <stdio.h>
int t_socket_init_fd(struct t_socket* t, int fd) {
  int r;

  r = t_socket_init(t, "", 0);
  if (r < 0) {
    return r;
  }

  t->sd = fd;

  return 0;
}
