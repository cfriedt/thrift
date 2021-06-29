#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <thrift/c/transport/t_socket.h>

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
  struct addrinfo* res;
  struct addrinfo* ai;
  struct addrinfo hints = {};
  socklen_t sa_len;
  uint8_t af;
  union {
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr_un sun;
  } sa = {};
  struct t_socket* const sock = (struct t_socket*)t;

  if (!t_transport_is_valid(t)) {
    return -EINVAL;
  }

  if (sock->host == NULL) {
    return EINVAL;
  }

  // try to resolve a UNIX-domain socket (a.k.a. named pipe)
  if (sock->port == 0 && sock->path != NULL && sock->path[0] == '/') {
    sa.sun.sun_family = AF_UNIX;
    strncpy(sa.sun.sun_path, sock->path, sizeof(sa.sun.sun_path));
    sa.sun.sun_len = sizeof(sa.sun);
    af = AF_UNIX;
    sa_len = sizeof(sa.sun);

    goto makesocket;
  } else {

    hints.ai_flags = AI_V4MAPPED;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    r = getaddrinfo(sock->host, NULL, &hints, &res);
    if (r != 0) {
      r = -EHOSTDOWN;
      goto close_sock_sd;
    }

    // first, try to find an IPv6 address
    for (ai = res; ai != NULL; ai = ai->ai_next) {
      if (ai->ai_family == AF_INET6) {
        memcpy(&sa.sa6, ai->ai_addr, ai->ai_addrlen);
        af = AF_INET6;
        sa_len = sizeof(sa.sa6);
        sa.sa6.sin6_port = htons(sock->port);
        goto makesocket;
      }
    }

    for (ai = res; ai != NULL; ai = ai->ai_next) {
      if (ai->ai_family == AF_INET) {
        memcpy(&sa.sa4, ai->ai_addr, ai->ai_addrlen);
        af = AF_INET;
        sa_len = sizeof(sa.sa4);
        sa.sa6.sin6_port = htons(sock->port);
        goto makesocket;
      }
    }

    r = -EAFNOSUPPORT;
    goto free_res;
  }

makesocket:
  r = socket(af, SOCK_STREAM, 0);
  if (r < 0) {
    r = -errno;
    goto out;
  }

  sock->sd = r;

  r = connect(sock->sd, (struct sockaddr*)&sa, sa_len);
  if (r < 0) {
    r = -errno;
    goto close_sock_sd;
  }

  // success! have have opened our socket!
  r = 0;
  goto out;

free_res:
  freeaddrinfo(res);

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