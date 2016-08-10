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

#ifndef _THRIFT_TRANSPORT_THTTPTRANSPORT_H_
#define _THRIFT_TRANSPORT_THTTPTRANSPORT_H_ 1

#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TVirtualTransport.h>

extern "C" {

#define WITH_POSIX 1
#include <coap/coap.h>
#undef WITH_POSIX

}

namespace apache {
namespace thrift {
namespace transport {

class TCoapTransport : public TVirtualTransport<TCoapTransport> {
public:
  TCoapTransport( boost::shared_ptr<TTransport> transport );

  virtual ~TCoapTransport();

  void open() { transport_->open(); }

  bool isOpen() { return transport_->isOpen(); }

  bool peek() { return transport_->peek(); }

  void close() { transport_->close(); }

  uint32_t read(uint8_t* buf, uint32_t len);

  uint32_t readEnd();

  void write(const uint8_t* buf, uint32_t len);

  virtual void flush() = 0;

  virtual const std::string getOrigin();

  // coap specific

  boost::shared_ptr<coap_context_t> getCoapContext();

  static boost::shared_ptr<coap_address_t> toCoapAddress( boost::shared_ptr<TTransport> trans );

protected:
  boost::shared_ptr<TTransport> transport_;
  std::string origin_;

  TMemoryBuffer writeBuffer_;
  TMemoryBuffer readBuffer_;

  boost::shared_ptr<coap_address_t> coap_address;
  boost::shared_ptr<coap_context_t> coap_context;

  virtual void init();
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_THTTPCLIENT_H_
