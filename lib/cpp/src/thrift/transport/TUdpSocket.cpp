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

const int TUdpSocket::UDP_SIZE_MAX = ( 1 << ( sizeof( in_port_t ) * 8 ) ) - 1;

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

uint32_t TUdpSocket::read( uint8_t* buf, uint32_t len ) {
	uint32_t r;

	/*
	 * On some systems, UDP messages must be read all at once, using one call
	 * to recvfrom(2), rather than a few bytes at a time. On such systems, if
	 * the datagram is not read in its entirety, then the unread remainder is
	 * discarded.
	 *
	 * The rest of Thrift is under the impression that it can read a few bytes
	 * at a time, which is incompatible with the systems mentioned above.
	 *
	 * Therefore, TUdpSocket internally buffers each datagram received.
	 *
	 * However, as a result of internal buffering, TUdpSocket is unable to
	 * rely on TSocket::read(), since that method polls a socket that
	 * that eventually has no data, prior to receiving an entire Thrift
	 * message. So we use TSocket::read() *only* if a datagram has not been
	 * read. We keep track of this with the boolean variable `read_called_once'.
	 *
	 * Subsequently, after an entire buffered datagram is read, we reset
	 * read_called_once to false;
	 */

	if ( ! read_called_once ) {
		r = TSocket::read( buf, len );
		read_called_once = true;
		return r;
	}

	int recv_r;

	recv_r = static_cast<int>( recv( socket_, cast_sockopt( buf ), len, 0 ) );
	if ( recv_r < 0 ) {
		int errno_copy = THRIFT_GET_SOCKET_ERROR;
		GlobalOutput.perror( "TUdpSocket::read() recv() " + getSocketInfo(), errno_copy );
		throw TTransportException( TTransportException::UNKNOWN, "recv() failed" );
	}

	r = recv_r;

	return r;
}

int TUdpSocket::getWholeDatagram() {
	int r;
	int sz;
	uint8_t *buf;

	// iteratively peek for a larger and larger message size until we know its exact size
	for(
		sz = 16,
			buf = new uint8_t[ sz ];
		sz < UDP_SIZE_MAX;
		sz *= 2,
			delete buf,
			buf = new uint8_t[ sz ]
   ) {
		src_addr_len = sizeof( src_addr );

		r = ::recvfrom( socket_, buf, sz, MSG_PEEK, (struct sockaddr *) & src_addr, & src_addr_len );

		if ( 0 == r ) {
			goto out;
		}

		if ( -1 == r ) {
			goto out;
		}

		if ( r < sz ) {
			break;
		}
	}

	if ( sz > UDP_SIZE_MAX ) {
		r = -1;
		goto out;
	}

	// finally, read the message out of the socket
	r = ::recvfrom( socket_, buf, sz, 0, (struct sockaddr *) & src_addr, & src_addr_len );

	if ( -1 != r ) {
		rx_buffer.clear();
		rx_buffer.insert( rx_buffer.end(), (uint8_t *) buf, (uint8_t *)buf + r );
	}

out:
	delete buf;
	return r;
}


int TUdpSocket::recv( int sockfd, void *buf, size_t len, int flags ) {

	int r;
	int datagram_size;

	THRIFT_UNUSED_VARIABLE( sockfd );
	THRIFT_UNUSED_VARIABLE( flags );

	datagram_size = rx_buffer.size();
	if ( 0 == datagram_size ) {

		r = getWholeDatagram();
		if ( 0 == r || -1 == r ) {
			goto out;
		}

		datagram_size = rx_buffer.size();
	}

	r = std::min( len, (size_t)datagram_size );
	std::memcpy( buf, & rx_buffer[ 0 ], r );
	std::memmove( & rx_buffer[ 0 ], & rx_buffer[ r ], datagram_size - r );
	rx_buffer.resize( datagram_size - r );

	datagram_size = rx_buffer.size();

	// see TUdpSocket::read() comments
	if ( 0 == datagram_size ) {
		read_called_once = false;
	}

out:
	return r;
}

int TUdpSocket::send( int sockfd, const void *buf, size_t len, int flags ) {

	THRIFT_UNUSED_VARIABLE( sockfd );
	THRIFT_UNUSED_VARIABLE( flags );

	int r;

	tx_buffer.insert( tx_buffer.end(), (uint8_t *) buf, (uint8_t *)buf + len );

	r = len;

	return r;
}

void TUdpSocket::flush() {

	int r;
	unsigned len;

	socklen_t dst_addr_len;
	struct sockaddr *dst_addr;

	std::string sockaddr_str;

	len = tx_buffer.size();
	if ( 0 == len ) {
		return;
	}

	dst_addr = getCachedAddress( & dst_addr_len );
	sockaddr_str = sockaddrToString( dst_addr, dst_addr_len );

	r = ::sendto( socket_, & tx_buffer[ 0 ], len, 0, dst_addr, dst_addr_len );
	if ( -1 == r ) {
	    int errno_copy = THRIFT_GET_SOCKET_ERROR;
	    GlobalOutput.perror("TUdpSocket::flush() sendto(): " + sockaddr_str, errno_copy);
	    throw TTransportException(TTransportException::UNKNOWN, "flush() sendto()", errno_copy);
	}

	tx_buffer.clear();
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

	// Follow RFC3986 http://www.ietf.org/rfc/rfc3986.txt
	// (except obviously we are lacking the protocol:// portion)
	switch( sa->sa_family ) {
	case AF_INET:
		r.append( host );
		r.append( ":" );
		r.append( service );
		break;

	case AF_INET6:
		r.append( "[" );
		r.append( host );
		r.append( "]" );
		r.append( ":" );
		r.append( service );
		break;

	default:
		break;
	}

	return r;
}

}
}
} // apache::thrift::transport
