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

#ifndef _THRIFT_TRANSPORT_TUDPSOCKET_H_
#define _THRIFT_TRANSPORT_TUDPSOCKET_H_ 1

#include <thrift/transport/TSocket.h>

namespace apache {
namespace thrift {
namespace transport {

/**
 * UDP Socket implementation of the TTransport interface.
 *
 */
class TUdpSocket: TSocket {
public:
	/**
	 * Constructs a new socket.
	 */
	TUdpSocket();

	/**
	 * Constructs a new socket.
	 *
	 * @param host An IP address or hostname to send datagrams to
	 * @param port The port to send on
	 */
	TUdpSocket( const std::string& host, int port );

	/**
	 * Constructs a new Unix domain socket.
	 *
	 * @param path The Unix domain socket e.g. "/tmp/ThriftTest.binary.thrift"
	 */
	TUdpSocket( const std::string& path );

	/**
	 * Destroyes the socket object, closing it if necessary.
	 */
	virtual ~TUdpSocket();

	/**
	 * Reads from the underlying socket.
	 * \returns the number of bytes read or 0 indicates EOF
	 * \throws TTransportException of types:
	 *           INTERRUPTED means the socket was interrupted
	 *                       out of a blocking call
	 *           NOT_OPEN means the socket has been closed
	 *           TIMED_OUT means the receive timeout expired
	 *           UNKNOWN means something unexpected happened
	 */
	virtual uint32_t read( uint8_t* buf, uint32_t len );

	/**
	 * Writes to the underlying socket.  Loops until done or fail.
	 */
	virtual void write( const uint8_t* buf, uint32_t len );

	inline void setLinger( bool on, int linger ) {}
	inline void setConnTimeout( int ms ) {}
	inline void setKeepAlive( bool keepAlive ) {}
	static inline void setUseLowMinRto( bool useLowMinRto ) {}
	static inline bool getUseLowMinRto() { return false; }

	/**
	 * Constructor to create socket from file descriptor.
	 */
	TUdpSocket( THRIFT_SOCKET socket );

	/**
	 * Constructor to create socket from file descriptor that
	 * can be interrupted safely.
	 */
	TUdpSocket( THRIFT_SOCKET socket, boost::shared_ptr<THRIFT_SOCKET> interruptListener );

	/**
	 * Gets the source address of the most recently received datagram.
	 */
	sockaddr *getCachedSourceAddress( socklen_t *len );
protected:

	struct sockaddr_storage src_addr;
	socklen_t src_addr_len;

	virtual int recv( int sockfd, void *buf, size_t len, int flags ) {
		std::memset( & src_addr, 0, sizeof( src_addr) );
		src_addr_len = sizeof( src_addr );
		return ::recvfrom( sockfd, buf, len, flags, (struct sockaddr *) & src_addr, & src_addr_len );
	}

	virtual int send( int sockfd, const void *buf, size_t len, int flags ) {
		socklen_t dst_addr_len;
		struct sockaddr *dst_addr;
		dst_addr = getCachedAddress( & dst_addr_len );
		return ::sendto( sockfd, buf, len, flags, dst_addr, dst_addr_len );
	}

};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_TUDPSOCKET_H_
