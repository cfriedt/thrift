#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cerrno>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#define BOOST_TEST_MODULE t_socket
#include <boost/test/unit_test.hpp>

#include "thrift/c/transport/t_socket.h"

using namespace std;

string to_string(const sockaddr* sa) {
  const sockaddr_in* const sa4 = (const sockaddr_in*)sa;
  const sockaddr_in6* const sa6 = (const sockaddr_in6*)sa;
  const sockaddr_un* const sun = (const sockaddr_un*)sa;
  char buf[INET6_ADDRSTRLEN];

  if (sa == nullptr) {
    return "<null>";
  }

  if (sa->sa_family == AF_UNIX) {
    return string(sun->sun_path, sun->sun_len);
  }

  if (sa->sa_family == AF_INET) {
    inet_ntop(AF_INET, &sa4->sin_addr, buf, sa4->sin_len);
    return string(buf) + ":" + to_string(ntohs(sa4->sin_port));
  }

  if (sa->sa_family == AF_INET6) {
    inet_ntop(AF_INET6, &sa6->sin6_addr, buf, sa6->sin6_len);
    return string(buf) + ":" + to_string(ntohs(sa6->sin6_port));
  }

  return "<unknown>";
}

class Harness {
public:
  Harness(string path) : path(path), port(0), server_sock(-1), server_fd(-1), client_fd(-1) {

    ready.lock();

    sockaddr_un sa = {};
    sa.sun_len = sizeof(sa);
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path.c_str(), sizeof(sa.sun_path));

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
      throw system_error(errno, system_category(), "failed to open path '" + string(path) + "'");
    }

    common((sockaddr*)&sa);
  }

  Harness(uint8_t af) : af(af), port(0), server_sock(-1), server_fd(-1), client_fd(-1) {

    ready.lock();

    sockaddr_in sa4 = {};
    sockaddr_in6 sa6 = {};
    sockaddr* sa;

    // in both AF_INET and AF_INET6 we just use an ephemeral port
    switch (af) {
    case AF_INET:
      sa4.sin_len = sizeof(sa4);
      sa4.sin_family = AF_INET;
      sa4.sin_addr.s_addr = INADDR_ANY;
      sa4.sin_len = sizeof(sa4);
      sa = (sockaddr*)&sa4;
      break;
    case AF_INET6:
      sa6.sin6_len = sizeof(sa6);
      sa6.sin6_family = AF_INET6;
      sa6.sin6_addr = in6addr_any;
      sa = (sockaddr*)&sa6;
      break;
    default:
      throw invalid_argument("not a valid address family: " + to_string((int)af));
      break;
    }

    server_sock = socket(af, SOCK_STREAM, 0);
    if (server_sock < 0) {
      throw system_error(errno, system_category(), "failed to open path '" + string(path) + "'");
    }

    common((sockaddr*)sa);
  }

  void common(sockaddr* sa) {
    int r;
    socklen_t len;

    if (sa->sa_family == AF_INET || sa->sa_family == AF_INET6) {
      r = 1;
      len = sizeof(r);
      r = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &r, len);
      if (r < 0) {
        throw system_error(errno, system_category(), "setsockopt");
      }
      // clog << "set SO_REUSEADDR on socket " << server_sock << endl;
    }

    r = ::bind(server_sock, sa, sa->sa_len);
    if (r < 0) {
      throw system_error(errno, system_category(), "bind");
    }

    if (sa->sa_family == AF_INET || sa->sa_family == AF_INET6) {
      r = getsockname(server_sock, sa, &len);
      if (r < 0) {
        throw system_error(errno, system_category(), "getsockname");
      }

      if (sa->sa_family == AF_INET) {
        port = ntohs(((sockaddr_in*)sa)->sin_port);
      } else {
        port = ntohs(((sockaddr_in6*)sa)->sin6_port);
      }
    }

    // clog << "bound socket " << server_sock << " to " << to_string(sa) << endl;

    r = listen(server_sock, 1);
    if (r < 0) {
      throw system_error(errno, system_category(), "listen");
    }

    // clog << "listening on fd " << server_sock << endl;

    accept_thread = thread([&]() {
      try {
        sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);
        // clog << "accepting connections on socket " << server_sock << endl;
        r = accept(server_sock, (sockaddr*)&client_addr, &len);
        if (r < 0) {
          perror("accept");
          throw system_error(errno, system_category(), "accept");
        }
        server_fd = r;
        // clog << "accepted connection from " << to_string((sockaddr*)&client_addr) << " as fd " <<
        // server_fd << endl;

        if (client_addr.ss_family == AF_INET || client_addr.ss_family == AF_INET6) {
          r = 1;
          len = sizeof(r);
          r = setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &r, len);
          if (r < 0) {
            throw system_error(errno, system_category(), "setsockopt");
          }
          // clog << "set TCP_NODELAY on socket " << server_fd << endl;
        }

        ready.unlock();
      } catch (exception& e) {
        cerr << e.what() << endl;
      }
    });
  }

  ~Harness() {
    // clog << "closing socket " << server_sock << endl;
    close(server_sock);
    // clog << "shutting down socket " << server_fd << endl;
    shutdown(server_fd, SHUT_RDWR);
    // clog << "closing socket " << client_fd << endl;
    close(client_fd);
    accept_thread.join();

    if (!path.empty()) {
      remove(path.c_str());
    }
  }

  string path;
  uint8_t af;
  uint16_t port;
  int server_sock;
  int server_fd;
  int client_fd;
  thread accept_thread;
  timed_mutex ready;
};

static struct t_socket sock;
static struct t_transport* const t = (struct t_transport*)&sock;

BOOST_AUTO_TEST_CASE(test_t_socket_init) {

  BOOST_CHECK_EQUAL(-EINVAL, t_socket_init(nullptr, "", 0));
  BOOST_CHECK_EQUAL(-EINVAL, t_socket_init(&sock, nullptr, 42));

  BOOST_CHECK_EQUAL(0, t_socket_init(&sock, "", 0));
  BOOST_CHECK_EQUAL(true, t_transport_is_valid(t));

  BOOST_CHECK_EQUAL(sock.sd, THRIFT_INVALID_SOCKET);
  BOOST_CHECK_EQUAL(sock.host, "");
  BOOST_CHECK_EQUAL(sock.port, 0);
}

BOOST_AUTO_TEST_CASE(test_t_socket_is_open) {
  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "", 0));

  BOOST_CHECK_EQUAL(false, t->is_open(nullptr));

  BOOST_CHECK_EQUAL(false, t->is_open(t));
  // all that is required is to have a file descriptor not equal to -1
  sock.sd = 42;
  BOOST_CHECK_EQUAL(true, t->is_open(t));
}

BOOST_AUTO_TEST_CASE(test_t_socket_peek) {
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(false, t->peek(nullptr));

  BOOST_CHECK_EQUAL(false, t->peek(t));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
  h.ready.try_lock_for(chrono::milliseconds(1000));
  h.client_fd = sock.sd;

  string s("Hello, t_socket world!");
  BOOST_REQUIRE_EQUAL(s.size(), send(h.server_fd, s.c_str(), s.size(), 0));
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(h.client_fd, &rfds);
  struct timeval timeout = {1, 0};
  select(h.client_fd + 1, &rfds, nullptr, nullptr, &timeout);

  BOOST_CHECK_EQUAL(true, t->peek(t));
}

BOOST_AUTO_TEST_CASE(test_t_socket_open) {
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(-EINVAL, t->open(nullptr));

  BOOST_CHECK_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
}

BOOST_AUTO_TEST_CASE(test_t_socket_close) {

  string sun_path(tmpnam(nullptr));
  Harness h(sun_path);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, sun_path.c_str(), h.port));

  BOOST_CHECK_EQUAL(-EINVAL, t->close(nullptr));

  BOOST_CHECK(0 != t->close(t));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);

  BOOST_CHECK_EQUAL(0, t->close(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET == sock.sd);
}

BOOST_AUTO_TEST_CASE(test_t_socket_read) {
  string s("Hello, t_socket world!");
  vector<uint8_t> buf(s.size());
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(-EINVAL, t->read(nullptr, &buf.front(), buf.size()));
  BOOST_CHECK_EQUAL(-EINVAL, t->read(t, nullptr, buf.size()));
  BOOST_CHECK_EQUAL(0, t->read(t, &buf.front(), 0));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
  h.ready.try_lock_for(chrono::milliseconds(1000));
  h.client_fd = sock.sd;

  BOOST_REQUIRE_EQUAL(s.size(), send(h.server_fd, s.c_str(), s.size(), 0));
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(h.client_fd, &rfds);
  struct timeval timeout = {1, 0};
  select(h.client_fd + 1, &rfds, nullptr, nullptr, &timeout);

  BOOST_CHECK_EQUAL(s.size(), t->read(t, &buf.front(), buf.size()));
  BOOST_CHECK_EQUAL(0, memcmp(s.c_str(), &buf.front(), s.size()));
}

BOOST_AUTO_TEST_CASE(test_t_socket_write) {
  string s("Hello, t_socket world!");
  vector<uint8_t> buf(s.size());
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(-EINVAL, t->write(nullptr, &buf.front(), buf.size()));
  BOOST_CHECK_EQUAL(-EINVAL, t->write(t, nullptr, buf.size()));
  BOOST_CHECK_EQUAL(0, t->write(t, &buf.front(), 0));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
  h.ready.try_lock_for(chrono::milliseconds(1000));
  h.client_fd = sock.sd;

  BOOST_REQUIRE_EQUAL(s.size(), t->write(t, s.c_str(), s.size()));
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(h.server_fd, &rfds);
  struct timeval timeout = {1, 0};
  select(h.server_fd + 1, &rfds, nullptr, nullptr, &timeout);

  BOOST_CHECK_EQUAL(s.size(), recv(h.server_fd, &buf.front(), buf.size(), 0));
  BOOST_CHECK_EQUAL(0, memcmp(s.c_str(), &buf.front(), s.size()));
}

BOOST_AUTO_TEST_CASE(test_t_socket_available_read) {
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(0, t->available_read(nullptr));

  BOOST_CHECK_EQUAL(0, t->available_read(t));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
  h.ready.try_lock_for(chrono::milliseconds(1000));
  h.client_fd = sock.sd;

  string s("Hello, t_socket world!");
  BOOST_REQUIRE_EQUAL(s.size(), send(h.server_fd, s.c_str(), s.size(), 0));
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(h.client_fd, &rfds);
  struct timeval timeout = {1, 0};
  select(h.client_fd + 1, &rfds, nullptr, nullptr, &timeout);

  BOOST_CHECK_EQUAL(s.size(), t->available_read(t));
}

BOOST_AUTO_TEST_CASE(test_t_socket_available_write) {
  Harness h(AF_INET);

  BOOST_REQUIRE_EQUAL(0, t_socket_init(&sock, "127.0.0.1", h.port));

  BOOST_CHECK_EQUAL(-EINVAL, t->available_write(nullptr));

  BOOST_CHECK_EQUAL(0, t->available_write(t));

  BOOST_REQUIRE_EQUAL(0, t->open(t));
  BOOST_REQUIRE(THRIFT_INVALID_SOCKET != sock.sd);
  h.ready.try_lock_for(chrono::milliseconds(1000));
  h.client_fd = sock.sd;

  BOOST_CHECK(t->available_write(t) > 0);
}
