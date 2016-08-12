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

#ifndef SOCK_DGRAM
#define SOCK_DGRAM 2
#endif

#include <thrift/transport/TUdpSocket.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

/**
 * TUdpSocket implementation.
 *
 */

TUdpSocket::TUdpSocket( const string& host, int port )
:
	TSocket::TSocket( host, port ),
	src_addr_len( 0 )
{
	socktype_hint = SOCK_DGRAM;
}

TUdpSocket::TUdpSocket( const string& path )
:
	TSocket::TSocket( path ),
	src_addr_len( 0 )
{
	socktype_hint = SOCK_DGRAM;
}

TUdpSocket::TUdpSocket()
:
	TSocket::TSocket(),
	src_addr_len( 0 )
{
	socktype_hint = SOCK_DGRAM;
}

TUdpSocket::TUdpSocket(THRIFT_SOCKET socket)
:
	TSocket::TSocket( socket ),
	src_addr_len( 0 )
{
	socktype_hint = SOCK_DGRAM;
}

TUdpSocket::TUdpSocket( THRIFT_SOCKET socket, boost::shared_ptr<THRIFT_SOCKET> interruptListener )
:
	TSocket::TSocket( socket, interruptListener ),
	src_addr_len( 0 )
{
	socktype_hint = SOCK_DGRAM;
}

TUdpSocket::~TUdpSocket() {
  close();
}

}
}
} // apache::thrift::transport
