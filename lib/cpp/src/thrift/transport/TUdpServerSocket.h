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

#ifndef _THRIFT_TRANSPORT_TUDPSERVERSOCKET_H_
#define _THRIFT_TRANSPORT_TUDPSERVERSOCKET_H_ 1

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>

namespace apache {
namespace thrift {
namespace transport {

/**
 * UDP Server socket implementation of TServerSocket.
 *
 * The only differences in TServerSocket and TUdpServerSocket are
 * a) their respective socktype_hint fields, and
 * b) TServerSocket uses listen(2) and accept(2), while TUdpServerSocket uses
 *    socket(2) and connect(2).
 *
 * "Rembember, if you connect() a datagram socket, you can then simply use
 *  send() and recv() for all your transactions. The socket itself is still
 *  a datagram socket and the packets still use UDP, but the socket interface
 *  will automatically add the destination and source information for you."
 *                                                                     --Beej
 */
class TUdpServerSocket : public TServerSocket {
public:
  /**
   * Constructor.
   *
   * @param server_port    Port number to bind to. The client port is assumed to be equal to server_port.
   */
  TUdpServerSocket(int port);

  /**
   * Constructor.
   *
   * @param server_port    Port number to bind to
   * @param client_port    Port number for response datagrams
   */
  TUdpServerSocket(int server_port, int client_port);

  /**
   * Constructor.
   *
   * @param port        Port number to bind to
   * @param sendTimeout Socket send timeout
   * @param recvTimeout Socket receive timeout
   */
  TUdpServerSocket(int port, int sendTimeout, int recvTimeout);

  /**
   * Constructor.
   *
   * @param address Address to bind to
   * @param port    Port number to bind to
   */
  TUdpServerSocket(const std::string& address, int port);

  /**
   * Constructor used for unix sockets.
   *
   * @param path Pathname for unix socket.
   */
  TUdpServerSocket(const std::string& path);

  virtual ~TUdpServerSocket();

  void listen();

protected:

  int client_port;

  boost::shared_ptr<TTransport> acceptImpl();
  boost::shared_ptr<TSocket> createSocket(THRIFT_SOCKET clientSocket);

  int family;
  int protocol;
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_TUDPSERVERSOCKET_H_
