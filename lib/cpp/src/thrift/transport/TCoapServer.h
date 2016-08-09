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

class TCoapServer : public TCoapTransport {
public:
  TCoapServer(boost::shared_ptr<TTransport> transport);

  virtual ~TCoapServer();

  virtual void flush();

  std::string getUri();
  std::string getMethod();

protected:
  void readHeaders();
  virtual void parseHeader(char* header);
  virtual bool parseStatusLine(char* status);
  std::string getTimeRFC1123();

  std::string uri;
  std::string method;
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
  virtual boost::shared_ptr<TTransport> getTransport(boost::shared_ptr<TTransport> trans) {
    return boost::shared_ptr<TTransport>(new TCoapServer(trans));
  }
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_THTTPSERVER_H_
