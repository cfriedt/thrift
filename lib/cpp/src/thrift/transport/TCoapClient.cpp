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

#include <limits>
#include <cstdlib>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <thrift/transport/TCoapClient.h>
#include <thrift/transport/TSocket.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapClient::TCoapClient( boost::shared_ptr<TTransport> transport, std::string host, std::string path )
:
	TCoapTransport( transport ),
	host_( host ),
	path_( path )
{
}

TCoapClient::TCoapClient( string host, int port, string path)
:
	TCoapTransport( boost::shared_ptr<TTransport>( new TSocket( host, port ) ) ),
    host_(host),
    path_(path)
{
}

TCoapClient::~TCoapClient() {
}

void TCoapClient::flush() {
/*
  // Fetch the contents of the write buffer
  uint8_t* buf;
  uint32_t len;
  writeBuffer_.getBuffer(&buf, &len);

  // Construct a CoAP header
  // XXX: @CF: TODO

//  if (header.size() > (std::numeric_limits<uint32_t>::max)()) {
//    throw TTransportException("Header too big");
//  }
  // Write the header, then the data, then flush
  //transport_->write((const uint8_t*)header.c_str(), static_cast<uint32_t>(header.size()));
  //transport_->write(buf, len);
  //transport_->flush();

  // Reset the buffer and header variables
  //writeBuffer_.resetBuffer();
*/
}
}
}
} // apache::thrift::transport
