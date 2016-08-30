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

namespace apache {
namespace thrift {
namespace transport {

class TCoapTransport : public TVirtualTransport<TCoapTransport> {
public:
  TCoapTransport( boost::shared_ptr<TTransport> transport );

  virtual ~TCoapTransport();

  void open() { return transport_->open(); }

  bool isOpen() { return transport_->isOpen(); }

  bool peek() { return transport_->peek(); }

  void close() { transport_->close(); }

  uint32_t read(uint8_t* buf, uint32_t len);

  void write(const uint8_t* buf, uint32_t len);

  virtual void flush() = 0;

  virtual const std::string getOrigin();

  void getLastToken( uint8_t *val, uint8_t *len );

protected:

  boost::shared_ptr<TTransport> transport_;
  std::string origin_;

  TMemoryBuffer writeBuffer_;
  TMemoryBuffer readBuffer_;

  uint64_t last_token_;
  uint8_t last_token_len_;

  static uint32_t transportAvail( boost::shared_ptr<TTransport> transport_ );
};

}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_THTTPCLIENT_H_
