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

#include <thrift/transport/TCoapTransport.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapTransport::TCoapTransport( boost::shared_ptr<TTransport> transport )
:
	transport_( transport ),
	origin_( "" ),
	coap_address( toCoapAddress( transport ) )
{
	const std::string coap_ver = coap_package_version();
	init();
}

TCoapTransport::~TCoapTransport() {
	if ( NULL != coap_context.get() ) {
		coap_context_t *ctx = coap_context.get();
		coap_free_context( ctx );
	}
	coap_address.reset();
	coap_context.reset();
}

uint32_t TCoapTransport::read( uint8_t* buf, uint32_t len ) {
	throw std::runtime_error( "TCoapTransport::read() not implemented" );
	return 0;
}

uint32_t TCoapTransport::readEnd() {
	throw std::runtime_error( "TCoapTransport::readEnd() not implemented" );
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
	coap_context = boost::shared_ptr<coap_context_t>( coap_new_context( coap_address.get() ) );
	if ( NULL == coap_context.get() ) {
		throw std::runtime_error( "coap_new_context() failed" );
	}
}

boost::shared_ptr<coap_context_t> TCoapTransport::getCoapContext() {
	return coap_context;
}

boost::shared_ptr<coap_address_t> TCoapTransport::toCoapAddress( boost::shared_ptr<TTransport> trans ) {
	throw std::runtime_error( "TCoapTransport::toCoapAddress() not implemented" );
	//return boost::shared_ptr<coap_address_t>( NULL );
}

}
}
}
