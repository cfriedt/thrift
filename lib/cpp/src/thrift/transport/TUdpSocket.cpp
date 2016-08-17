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

#include <iostream>

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
	client_port( port )
{
}

TUdpSocket::TUdpSocket( const string& host, int server_port, int client_port )
:
	TSocket::TSocket( host, server_port ),
	client_port( client_port )
{
}

TUdpSocket::TUdpSocket( const string& path )
:
	TSocket::TSocket( path )
{
}

TUdpSocket::TUdpSocket()
:
	TSocket::TSocket()
{
}

TUdpSocket::TUdpSocket(THRIFT_SOCKET socket)
:
	TSocket::TSocket( socket )
{
}

TUdpSocket::TUdpSocket( THRIFT_SOCKET socket, boost::shared_ptr<THRIFT_SOCKET> interruptListener )
:
	TSocket::TSocket( socket, interruptListener ),
	src_addr_len( 0 )
{
}

TUdpSocket::~TUdpSocket() {
  close();
}

void TUdpSocket::bindLocalEndpoint() {

#ifdef _WIN32
  TWinsockSingleton::create();
#endif // _WIN32

	if ( client_port < 0 || client_port > 0xFFFF ) {
		throw TTransportException( TTransportException::BAD_ARGS, "Specified port is invalid" );
	}

	struct addrinfo hints, *res, *res0;
	res = NULL;
	res0 = NULL;
	int error;
	char port[ sizeof( "65535" ) ];
	std::memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	sprintf( port, "%d", client_port );

	error = getaddrinfo( "localhost", port, &hints, &res0 );

#ifdef _WIN32
	if (error == WSANO_DATA) {
		hints.ai_flags &= ~AI_ADDRCONFIG;
		error = getaddrinfo(host_.c_str(), port, &hints, &res0);
	}
#endif

	if ( error ) {
		string errStr = "TSocket::open() getaddrinfo() " + getSocketInfo() + string( THRIFT_GAI_STRERROR( error ) );
		GlobalOutput( errStr.c_str() );
		close();
		throw TTransportException( TTransportException::NOT_OPEN, "Could not resolve host for client socket." );
	}

	for ( res = res0; res; res = res->ai_next ) {
		socket_ = ::socket( res->ai_family, res->ai_socktype, res->ai_protocol );
		if ( socket_ == THRIFT_INVALID_SOCKET ) {
			continue;
		}

#ifdef SO_NOSIGPIPE
		{
			int one = 1;
			setsockopt( socket_, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof( one ) );
		}
#endif

		int ret;
		ret = ::bind( socket_, res->ai_addr, static_cast<int>( res->ai_addrlen ) );
		// success case
		if ( ret != 0 ) {
			close();
			continue;
		}
		break;
	}

	if ( -1 == socket_ ) {
		int errno_copy = THRIFT_GET_SOCKET_ERROR;
		GlobalOutput.perror( "TSocket::open() " + getSocketInfo(), errno_copy );
		throw TTransportException( TTransportException::NOT_OPEN, "open() failed", errno_copy );
	}

	freeaddrinfo(res0);
}

void TUdpSocket::saveRemoteEndpoint() {

	if ( port_ < 0 || port_ > 0xFFFF ) {
		throw TTransportException( TTransportException::BAD_ARGS, "Specified port is invalid" );
	}

	struct addrinfo hints, *res, *res0;
	res = NULL;
	res0 = NULL;
	int error;
	char port[ sizeof( "65535" ) ];
	std::memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	sprintf( port, "%d", port_ );

	error = getaddrinfo( host_.c_str(), port, &hints, &res0 );

#ifdef _WIN32
	if (error == WSANO_DATA) {
		hints.ai_flags &= ~AI_ADDRCONFIG;
		error = getaddrinfo(host_.c_str(), port, &hints, &res0);
	}
#endif

	if ( error ) {
		string errStr = "TSocket::open() getaddrinfo() " + getSocketInfo() + string( THRIFT_GAI_STRERROR( error ) );
		GlobalOutput( errStr.c_str() );
		close();
		throw TTransportException( TTransportException::NOT_OPEN, "Could not resolve host for client socket." );
	}

	error = -1;
	for ( res = res0; res; res = res->ai_next ) {
		setCachedAddress( res->ai_addr, static_cast<socklen_t>( res->ai_addrlen ) );
		error = 0;
		break;
	}
	if ( -1 == error ) {
		throw TTransportException( TTransportException::NOT_OPEN, "unable to find route to host" );
	}

	freeaddrinfo(res0);
}


void TUdpSocket::open() {

  if (isOpen()) {
    return;
  }

  bindLocalEndpoint();

  saveRemoteEndpoint();
}



// We buffer the entire datagram internally, since UDP packets can be discarded
// if the entire message is not read in one call. Subsequent calls to recv() will
// access subsequent data units of the UDP message.
int TUdpSocket::recv( int sockfd, void *buf, size_t len, int flags ) {

	int r;
	int datagram_size;

	THRIFT_UNUSED_VARIABLE( flags );

	std::memset( &src_addr, 0, sizeof( src_addr ) );
	src_addr_len = sizeof( src_addr );

	datagram_size = datagram_buffer.size();
	if ( 0 == datagram_size ) {

		std::cout << "TUdpSocket::recv(): datagram buffer is empty, receiving a new one" << std::endl;

		// this is kind of best effort. ipv6 packets can get up to 64kB!!
		static uint8_t tbuf[ 4096 ];

		// get the amount of data in the UDP datagram
		r = ::recvfrom( sockfd, tbuf, sizeof( tbuf ), 0, (struct sockaddr *) & src_addr, & src_addr_len );
		if ( -1 == r || sizeof( tbuf ) == r ) {
			goto out;
		}
		datagram_size = r;

		std::cout << "TUdpSocket::recv(): received " << datagram_size << " bytes from " << sockaddrToString( (sockaddr *) & src_addr, src_addr_len ) << std::endl;

		datagram_buffer.reserve( datagram_size );
		std::memcpy( & datagram_buffer[ 0 ], & tbuf[ 0 ], datagram_size );
	}

	r = std::min( len, (size_t)datagram_size );
	std::memcpy( buf, & datagram_buffer[ 0 ], r );
	std::memmove( & datagram_buffer[ 0 ], & datagram_buffer[ r ], datagram_size - r );
	datagram_buffer.resize( datagram_size - r );

out:

	std::cout << "TUdpSocket::recv(): returning " << r << " bytes of " << len << " bytes requested" << std::endl;

	return r;
}

int TUdpSocket::send( int sockfd, const void *buf, size_t len, int flags ) {
	int r;
	socklen_t dst_addr_len;
	struct sockaddr *dst_addr;
	dst_addr = getCachedAddress( & dst_addr_len );

	std::cout << "TUdpSocket::send(): sending " << len << " bytes to " << getSocketInfo() << std::endl;

	r = ::sendto( sockfd, buf, len, flags, dst_addr, dst_addr_len );
	return r;
}


std::string TUdpSocket::sockaddrToString( struct sockaddr *sa,  socklen_t len )
{
	std::string r;

	int gni_r;

	char host[ NI_MAXHOST ];
	char service[ NI_MAXSERV ];

	gni_r = getnameinfo( sa, len, host, sizeof( host ), service, sizeof( service ), NI_NUMERICHOST | NI_NUMERICSERV );

	if ( 0 != gni_r ) {
		throw TTransportException( TTransportException::UNKNOWN, "could not convert sockaddr to string" );
	}

	r.append( host );
	r.append( ":" );
	r.append( service );

	return r;
}

}
}
} // apache::thrift::transport
