/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <thrift/thrift-config.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#include <thrift/transport/PlatformSocket.h>
#include <thrift/transport/TUdpSocket.h>
#include <thrift/transport/TUdpServerSocket.h>
#include <thrift/transport/TGetAddrInfoWrapper.h>

extern void destroyer_of_fine_sockets(THRIFT_SOCKET* ssock);

namespace apache {
namespace thrift {
namespace transport {

using namespace ::std;

TUdpServerSocket::TUdpServerSocket(int port)
  : TServerSocket::TServerSocket(port), family( 0 ), protocol( 0 ), client_port( port ) {
}

TUdpServerSocket::TUdpServerSocket(int server_port, int client_port)
  : TServerSocket::TServerSocket(server_port), family( 0 ), protocol( 0 ), client_port( client_port ) {
}

TUdpServerSocket::TUdpServerSocket(int port, int sendTimeout, int recvTimeout)
  : TServerSocket::TServerSocket(port, sendTimeout, recvTimeout), family( 0 ), protocol( 0 ) {
}

TUdpServerSocket::TUdpServerSocket(const string& address, int port)
  : TServerSocket::TServerSocket(address, port), family( 0 ), protocol( 0 ) {
}

TUdpServerSocket::TUdpServerSocket(const string& path)
  : TServerSocket::TServerSocket(path), family( 0 ), protocol( 0 ) {
}

TUdpServerSocket::~TUdpServerSocket() {
  close();
}

void TUdpServerSocket::listen() {
  listening_ = true;
#ifdef _WIN32
  TWinsockSingleton::create();
#endif // _WIN32
  THRIFT_SOCKET sv[2];
  // Create the socket pair used to interrupt
  if (-1 == THRIFT_SOCKETPAIR(AF_LOCAL, SOCK_STREAM, 0, sv)) {
    GlobalOutput.perror("TUdpServerSocket::listen() socketpair() interrupt", THRIFT_GET_SOCKET_ERROR);
    interruptSockWriter_ = THRIFT_INVALID_SOCKET;
    interruptSockReader_ = THRIFT_INVALID_SOCKET;
  } else {
    interruptSockWriter_ = sv[1];
    interruptSockReader_ = sv[0];
  }

  // Create the socket pair used to interrupt all clients
  if (-1 == THRIFT_SOCKETPAIR(AF_LOCAL, SOCK_STREAM, 0, sv)) {
    GlobalOutput.perror("TUdpServerSocket::listen() socketpair() childInterrupt",
                        THRIFT_GET_SOCKET_ERROR);
    childInterruptSockWriter_ = THRIFT_INVALID_SOCKET;
    pChildInterruptSockReader_.reset();
  } else {
    childInterruptSockWriter_ = sv[1];
    pChildInterruptSockReader_
        = boost::shared_ptr<THRIFT_SOCKET>(new THRIFT_SOCKET(sv[0]), destroyer_of_fine_sockets);
  }

  // Validate port number
  if (port_ < 0 || port_ > 0xFFFF) {
    throw TTransportException(TTransportException::BAD_ARGS, "Specified port is invalid");
  }

  const struct addrinfo *res;
  int error;
  char port[sizeof("65535")];
  snprintf(port, sizeof(port), "%d", port_);

  struct addrinfo hints;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;

  // If address is not specified use wildcard address (NULL)
  TGetAddrInfoWrapper info(address_.empty() ? NULL : &address_[0], port, &hints);

  error = info.init();
  if (error) {
    GlobalOutput.printf("getaddrinfo %d: %s", error, THRIFT_GAI_STRERROR(error));
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "Could not resolve host for server socket.");
  }

  // Pick the ipv6 address first since ipv4 addresses can be mapped
  // into ipv6 space.
  for (res = info.res(); res; res = res->ai_next) {
    if (res->ai_family == AF_INET6 || res->ai_next == NULL)
      break;
  }

    serverSocket_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    family = res->ai_family;
    protocol = res->ai_protocol;

  if (serverSocket_ == THRIFT_INVALID_SOCKET) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    GlobalOutput.perror("TUdpServerSocket::listen() socket() ", errno_copy);
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "Could not create server socket.",
                              errno_copy);
  }

  // Set THRIFT_NO_SOCKET_CACHING to prevent 2MSL delay on accept
  int one = 1;
  if (-1 == setsockopt(serverSocket_,
                       SOL_SOCKET,
                       THRIFT_NO_SOCKET_CACHING,
                       cast_sockopt(&one),
                       sizeof(one))) {
// ignore errors coming out of this setsockopt on Windows.  This is because
// SO_EXCLUSIVEADDRUSE requires admin privileges on WinXP, but we don't
// want to force servers to be an admin.
#ifndef _WIN32
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    GlobalOutput.perror("TUdpServerSocket::listen() setsockopt() THRIFT_NO_SOCKET_CACHING ",
                        errno_copy);
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "Could not set THRIFT_NO_SOCKET_CACHING",
                              errno_copy);
#endif
  }

#ifdef IPV6_V6ONLY
  if (res->ai_family == AF_INET6 && path_.empty()) {
    int zero = 0;
    if (-1 == setsockopt(serverSocket_,
                         IPPROTO_IPV6,
                         IPV6_V6ONLY,
                         cast_sockopt(&zero),
                         sizeof(zero))) {
      GlobalOutput.perror("TUdpServerSocket::listen() IPV6_V6ONLY ", THRIFT_GET_SOCKET_ERROR);
    }
  }
#endif // #ifdef IPV6_V6ONLY

  // Set NONBLOCK on the accept socket
  int flags = THRIFT_FCNTL(serverSocket_, THRIFT_F_GETFL, 0);
  if (flags == -1) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    GlobalOutput.perror("TUdpServerSocket::listen() THRIFT_FCNTL() THRIFT_F_GETFL ", errno_copy);
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "THRIFT_FCNTL() THRIFT_F_GETFL failed",
                              errno_copy);
  }

  if (-1 == THRIFT_FCNTL(serverSocket_, THRIFT_F_SETFL, flags | THRIFT_O_NONBLOCK)) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    GlobalOutput.perror("TUdpServerSocket::listen() THRIFT_FCNTL() THRIFT_O_NONBLOCK ", errno_copy);
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "THRIFT_FCNTL() THRIFT_F_SETFL THRIFT_O_NONBLOCK failed",
                              errno_copy);
  }

  // prepare the port information
  // we may want to try to bind more than once, since THRIFT_NO_SOCKET_CACHING doesn't
  // always seem to work. The client can configure the retry variables.
  int retries = 0;
  int errno_copy = 0;

    do {
      if (0 == ::bind(serverSocket_, res->ai_addr, static_cast<int>(res->ai_addrlen))) {
        break;
      }
      errno_copy = THRIFT_GET_SOCKET_ERROR;
      // use short circuit evaluation here to only sleep if we need to
    } while ((retries++ < retryLimit_) && (THRIFT_SLEEP_SEC(retryDelay_) == 0));

    // retrieve bind info
    if (port_ == 0 && retries <= retryLimit_) {
      struct sockaddr_storage sa;
      socklen_t len = sizeof(sa);
      std::memset(&sa, 0, len);
      if (::getsockname(serverSocket_, reinterpret_cast<struct sockaddr*>(&sa), &len) < 0) {
        errno_copy = THRIFT_GET_SOCKET_ERROR;
        GlobalOutput.perror("TUdpServerSocket::getPort() getsockname() ", errno_copy);
      } else {
        if (sa.ss_family == AF_INET6) {
          const struct sockaddr_in6* sin = reinterpret_cast<const struct sockaddr_in6*>(&sa);
          port_ = ntohs(sin->sin6_port);
        } else {
          const struct sockaddr_in* sin = reinterpret_cast<const struct sockaddr_in*>(&sa);
          port_ = ntohs(sin->sin_port);
        }
      }
    }

  // throw an error if we failed to bind properly
  if (retries > retryLimit_) {
    char errbuf[1024];
    if (!path_.empty()) {
      snprintf(errbuf, sizeof(errbuf), "TUdpServerSocket::listen() PATH %s", path_.c_str());
    } else {
      snprintf(errbuf, sizeof(errbuf), "TUdpServerSocket::listen() BIND %d", port_);
    }
    GlobalOutput(errbuf);
    close();
    throw TTransportException(TTransportException::NOT_OPEN,
                              "Could not bind",
                              errno_copy);
  }

  if (listenCallback_)
    listenCallback_(serverSocket_);

  // The socket is now listening!
}

boost::shared_ptr<TTransport> TUdpServerSocket::acceptImpl() {
  if (serverSocket_ == THRIFT_INVALID_SOCKET) {
    throw TTransportException(TTransportException::NOT_OPEN, "TServerSocket not open");
  }

  struct THRIFT_POLLFD fds[2];

  int maxEintrs = 5;
  int numEintrs = 0;

  while (true) {
    std::memset(fds, 0, sizeof(fds));
    fds[0].fd = serverSocket_;
    fds[0].events = THRIFT_POLLIN;
    if (interruptSockReader_ != THRIFT_INVALID_SOCKET) {
      fds[1].fd = interruptSockReader_;
      fds[1].events = THRIFT_POLLIN;
    }
    /*
      TODO: if THRIFT_EINTR is received, we'll restart the timeout.
      To be accurate, we need to fix this in the future.
     */
    int ret = THRIFT_POLL(fds, 2, accTimeout_);

    if (ret < 0) {
      // error cases
      if (THRIFT_GET_SOCKET_ERROR == THRIFT_EINTR && (numEintrs++ < maxEintrs)) {
        // THRIFT_EINTR needs to be handled manually and we can tolerate
        // a certain number
        continue;
      }
      int errno_copy = THRIFT_GET_SOCKET_ERROR;
      GlobalOutput.perror("TServerSocket::acceptImpl() THRIFT_POLL() ", errno_copy);
      throw TTransportException(TTransportException::UNKNOWN, "Unknown", errno_copy);
    } else if (ret > 0) {
      // Check for an interrupt signal
      if (interruptSockReader_ != THRIFT_INVALID_SOCKET && (fds[1].revents & THRIFT_POLLIN)) {
        int8_t buf;
        if (-1 == recv(interruptSockReader_, cast_sockopt(&buf), sizeof(int8_t), 0)) {
          GlobalOutput.perror("TServerSocket::acceptImpl() recv() interrupt ",
                              THRIFT_GET_SOCKET_ERROR);
        }
        throw TTransportException(TTransportException::INTERRUPTED);
      }

      // Check for the actual server socket being ready
      if (fds[0].revents & THRIFT_POLLIN) {
        break;
      }
    } else {
      GlobalOutput("TServerSocket::acceptImpl() THRIFT_POLL 0");
      throw TTransportException(TTransportException::UNKNOWN);
    }
  }

  char arbitrary_buffer[ 1 ];

  struct sockaddr_storage client_address;
  socklen_t client_address_size = sizeof(client_address);

  int recvfrom_r = ::recvfrom(serverSocket_, arbitrary_buffer, sizeof( arbitrary_buffer ), MSG_PEEK, (struct sockaddr*)&client_address, (socklen_t*)&client_address_size);
  if ( -1 == recvfrom_r ) {
	  GlobalOutput.perror("TServerSocket::acceptImpl() recvfrom()", THRIFT_GET_SOCKET_ERROR);
      throw TTransportException(TTransportException::UNKNOWN);
  }

  THRIFT_SOCKET clientSocket;

  clientSocket = dup( serverSocket_ );

  if ( clientSocket == THRIFT_INVALID_SOCKET) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    GlobalOutput.perror("TServerSocket::acceptImpl() ::recvfrom() ", errno_copy);
    throw TTransportException(TTransportException::UNKNOWN, "accept()", errno_copy);
  }

  // Make sure client socket is blocking
  int flags = THRIFT_FCNTL(clientSocket, THRIFT_F_GETFL, 0);
  if (flags == -1) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    ::THRIFT_CLOSESOCKET(clientSocket);
    GlobalOutput.perror("TServerSocket::acceptImpl() THRIFT_FCNTL() THRIFT_F_GETFL ", errno_copy);
    throw TTransportException(TTransportException::UNKNOWN,
                              "THRIFT_FCNTL(THRIFT_F_GETFL)",
                              errno_copy);
  }

  if (-1 == THRIFT_FCNTL(clientSocket, THRIFT_F_SETFL, flags & ~THRIFT_O_NONBLOCK)) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    ::THRIFT_CLOSESOCKET(clientSocket);
    GlobalOutput
        .perror("TServerSocket::acceptImpl() THRIFT_FCNTL() THRIFT_F_SETFL ~THRIFT_O_NONBLOCK ",
                errno_copy);
    throw TTransportException(TTransportException::UNKNOWN,
                              "THRIFT_FCNTL(THRIFT_F_SETFL)",
                              errno_copy);
  }

  switch( client_address.ss_family ) {
  case AF_INET:
	  ( (struct sockaddr_in *) & client_address )->sin_port = htons( client_port );
	  break;
  case AF_INET6:
	  ( (struct sockaddr_in6 *) & client_address )->sin6_port = htons( client_port );
	  break;
  }

  /*
  int connect_r = ::connect( clientSocket, (struct sockaddr *) & client_address, client_address_size );
  if (-1 == connect_r ) {
    int errno_copy = THRIFT_GET_SOCKET_ERROR;
    ::THRIFT_CLOSESOCKET(clientSocket);
    GlobalOutput.perror("TServerSocket::acceptImpl() connect() ", errno_copy);
    throw TTransportException(TTransportException::UNKNOWN, "connect() failed", errno_copy);
  }
  */

  boost::shared_ptr<TSocket> client = createSocket(clientSocket);
  if (sendTimeout_ > 0) {
    client->setSendTimeout(sendTimeout_);
  }
  if (recvTimeout_ > 0) {
    client->setRecvTimeout(recvTimeout_);
  }
  if (keepAlive_) {
    client->setKeepAlive(keepAlive_);
  }

  client->setCachedAddress((sockaddr*)&client_address, client_address_size);

  if (acceptCallback_)
    acceptCallback_(clientSocket);

  return client;
}

boost::shared_ptr<TSocket> TUdpServerSocket::createSocket(THRIFT_SOCKET clientSocket) {
  if (interruptableChildren_) {
	return boost::shared_ptr<TSocket>( (TSocket *) new TUdpSocket( clientSocket, pChildInterruptSockReader_ ) );
  } else {
	return boost::shared_ptr<TSocket>( (TSocket *) new TUdpSocket( clientSocket ) );
  }
}

}
}
} // apache::thrift::transport
