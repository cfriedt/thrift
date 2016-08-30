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

#ifndef _THRIFT_TRANSPORT_THTTPSERVER_H_
#define _THRIFT_TRANSPORT_THTTPSERVER_H_ 1

#include <thrift/transport/TCoapTransport.h>

namespace apache {
namespace thrift {
namespace transport {

typedef struct coap_resource_initializer {



} coap_resource_initializer_t;

class TCoapServer : public TCoapTransport {
public:
  TCoapServer( boost::shared_ptr<TTransport> transport );

  virtual ~TCoapServer();

  virtual void flush();

  std::string getUri();
  std::string getMethod();

  //void requestResource( boost::shared_ptr<coap_resource_t> res );

protected:
  std::string uri;
  std::string method;

  //std::vector<boost::shared_ptr<coap_resource_t>> resource;

  //void handle_request( coap_context_t *context, coap_queue_t *node, uint8_t **buf, uint32_t *len );
};

/**
 * Wraps a transport into HTTP protocol
 */
class TCoapServerTransportFactory : public TTransportFactory {
public:
  TCoapServerTransportFactory() {}

  virtual ~TCoapServerTransportFactory() {}

  /**
   * Wraps the transport into a buffered one.
   */
  virtual boost::shared_ptr<TTransport> getTransport( boost::shared_ptr<TTransport> trans ) {
    return boost::shared_ptr<TTransport>( new TCoapServer( trans ) );
  }
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_THTTPSERVER_H_
