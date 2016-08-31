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
class TUdpSocket: public TSocket {

public:
	/**
	 * Constructs a new socket.
	 */
	TUdpSocket();

	/**
	 * Constructs a new socket.
	 *
	 * @param host An IP address or hostname to send datagrams to
	 * @param port The port to send on. Server and client ports are
	 *        assumed equal.
	 */
	TUdpSocket( const std::string& host, int port );

	/**
	 * Constructs a new socket.
	 *
	 * @param host        An IP address or hostname to send datagrams to
	 * @param server_port The port to send on
	 * @param client_port The local port to receive on
	 */
	TUdpSocket( const std::string& host, int server_port, int client_port );

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
	 * Creates and opens the UNIX socket.
	 *
	 * @throws TTransportException If the socket could not bind to the client port
	 */
	virtual void open();

	inline void setNoDelay( bool on ) {
		THRIFT_UNUSED_VARIABLE( on );
	}
	inline void setLinger( bool on, int linger ) {
		THRIFT_UNUSED_VARIABLE( on );
		THRIFT_UNUSED_VARIABLE( linger );
	}
	inline void setConnTimeout( int ms ) {
		THRIFT_UNUSED_VARIABLE( ms );
	}
	inline void setKeepAlive( bool keepAlive ) {
		THRIFT_UNUSED_VARIABLE( keepAlive );
	}
	static inline void setUseLowMinRto( bool useLowMinRto ) {
		THRIFT_UNUSED_VARIABLE( useLowMinRto );
	}
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

	static std::string sockaddrToString( struct sockaddr *sa,  socklen_t len );

	uint32_t read( uint8_t* buf, uint32_t len );

	void flush();

	const uint8_t *borrow_virt( uint8_t *buf, uint32_t *len );
	void consume_virt( uint32_t len );

protected:

	int client_port;
	struct sockaddr_storage src_addr;
	socklen_t src_addr_len;

	int recv( int sockfd, void *buf, size_t len, int flags );
	int send( int sockfd, const void *buf, size_t len, int flags );

private:
	void bindLocalEndpoint();
	void saveRemoteEndpoint();

	bool read_called_once;
	int getWholeDatagram();

	static const int UDP_SIZE_MAX;

	std::vector<uint8_t> rx_buffer;
	std::vector<uint8_t> tx_buffer;

	uint32_t avail();
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_TUDPSOCKET_H_
