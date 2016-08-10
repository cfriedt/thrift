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

#include <cstdlib>
#include <sstream>
#include <iostream>

#include <thrift/transport/TCoapServer.h>
#include <thrift/transport/TSocket.h>

extern "C" {

#ifndef ARRAY_SIZE
#define ARRAY_SIZE( x ) (int)( sizeof( x ) / sizeof( (x)[ 0 ] ) )
#endif // ARRAY_SIZE

}

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapServer::TCoapServer(boost::shared_ptr<TTransport> transport )
:
	TCoapTransport( transport )
{
}

TCoapServer::~TCoapServer() {
	resource.clear();
}

std::string TCoapServer::getUri() {
	return uri;
}

std::string TCoapServer::getMethod() {
	return method;
}

void TCoapServer::requestResource( boost::shared_ptr<coap_resource_t> req ) {
	coap_resource_t *res;
	res = coap_resource_init( req->uri.s, req->uri.length, req->flags );
	for( int j = 0; j < ARRAY_SIZE( req->handler ); j++ ) {
		if ( NULL == req->handler[ j ] ) {
			continue;
		}
		coap_register_handler( res, j, req->handler[ j ] );
		coap_add_resource( getCoapContext().get(), res );
	}
	resource.push_back( boost::shared_ptr<coap_resource_t>( res ) );
}

void TCoapServer::flush() {
	throw std::runtime_error( "TCoapServer::flush() not implemented" );
/*
  // Fetch the contents of the write buffer
  uint8_t* buf;
  uint32_t len;
  writeBuffer_.getBuffer(&buf, &len);

  // Construct the HTTP header
  std::ostringstream h;
  h << "HTTP/1.1 200 OK" << CRLF << "Date: " << getTimeRFC1123() << CRLF << "Server: Thrift/"
    << VERSION << CRLF << "Access-Control-Allow-Origin: *" << CRLF
    << "Content-Type: application/x-thrift" << CRLF << "Content-Length: " << len << CRLF
    << "Connection: Keep-Alive" << CRLF << CRLF;
  string header = h.str();

  // Write the header, then the data, then flush
  // cast should be fine, because none of "header" is under attacker control
  transport_->write((const uint8_t*)header.c_str(), static_cast<uint32_t>(header.size()));
  transport_->write(buf, len);
  transport_->flush();

  // Reset the buffer and header variables
  writeBuffer_.resetBuffer();
  readHeaders_ = true;
*/
}

}
}
} // apache::thrift::transport
