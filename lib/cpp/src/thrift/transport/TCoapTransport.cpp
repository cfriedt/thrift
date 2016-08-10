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

#include <iostream>
#include <sstream>

#include <boost/make_shared.hpp>

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TCoapTransport.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapTransport::TCoapTransport( boost::shared_ptr<TTransport> transport )
:
	transport_( transport ),
	origin_( "" )
{
	const std::string coap_ver = coap_package_version();
}

TCoapTransport::~TCoapTransport() {
	if ( NULL != coap_context.get() ) {
		coap_context_t *ctx = coap_context.get();
		coap_free_context( ctx );
	}
	coap_address.reset();
	coap_context.reset();
}

void TCoapTransport::open() {

	std::cout << "open() called" << std::endl;

	transport_->open();

	std::cout << "getting coap address..." << std::endl;

	coap_address = toCoapAddress( transport_ );

	std::cout << "got coap address!!" << std::endl;

	std::cout << "initializing coap context..." << std::endl;

	coap_context = boost::shared_ptr<coap_context_t>( coap_new_context( coap_address.get() ) );
	if ( NULL == coap_context.get() ) {
		throw std::runtime_error( "coap_new_context() failed" );
	}

	std::cout << "initialized coap context!!!" << std::endl;
}

uint32_t TCoapTransport::read( uint8_t* buf, uint32_t len ) {
	throw std::runtime_error( "TCoapTransport::read() not implemented" );
	return 0;
}

uint32_t TCoapTransport::readEnd() {
	//throw std::runtime_error( "TCoapTransport::readEnd() not implemented" );
	return 0;
}

void TCoapTransport::write( const uint8_t* buf, uint32_t len ) {
	//writeBuffer_.write( buf, len );
	throw std::runtime_error( "TCoapTransport::write() not implemented" );
}

const std::string TCoapTransport::getOrigin() {
	std::ostringstream oss;
	if ( !origin_.empty() ) {
		oss << origin_ << ", ";
	}
	oss << transport_->getOrigin();
	return oss.str();
}

void TCoapTransport::init() {
}

boost::shared_ptr<coap_context_t> TCoapTransport::getCoapContext() {
	return coap_context;
}

boost::shared_ptr<coap_address_t> TCoapTransport::toCoapAddress( boost::shared_ptr<TTransport> trans ) {

	sockaddr *sock_p;
	socklen_t sock_len;

	boost::shared_ptr<TSocket> socket_trans;
	boost::shared_ptr<coap_address_t> coap_address;

	coap_address = boost::make_shared<coap_address_t>();
	socket_trans = boost::static_pointer_cast<TSocket>( trans );

	sock_p = socket_trans->getCachedAddress( & sock_len );
	if ( NULL == sock_p ) {
		throw std::runtime_error( "TSocket::getCachedAddress() returned NULL!" );
	}

	switch( sock_len ) {

	case sizeof(sockaddr_in):

		std::memcpy( & coap_address->addr.sin, sock_p, sock_len );
		coap_address->addr.sin.sin_port = ntohs( coap_address->addr.sin.sin_port );
		coap_address->addr.sin.sin_addr.s_addr = ntohl( coap_address->addr.sin.sin_addr.s_addr );

		break;

	case sizeof(sockaddr_in6):

		std::memcpy( & coap_address->addr.sin6, sock_p, sock_len );
		coap_address->addr.sin6.sin6_port = ntohs( coap_address->addr.sin6.sin6_port );

		break;

	default:
		throw std::runtime_error( "unsupported socket length" );
		break;
	}

	return coap_address;
}

}
}
}
