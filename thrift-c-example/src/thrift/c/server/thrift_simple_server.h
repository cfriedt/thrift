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

#ifndef _THRIFT_SIMPLE_SERVER_H
#define _THRIFT_SIMPLE_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include <thrift/c/server/thrift_server.h>
#include <thrift/c/thrift_c_object.h>

__BEGIN_DECLS

/*! \file thrift_simple_server.h
 *  \brief A simple Thrift server, single-threaded.
 */

typedef struct _ThriftSimpleServer ThriftSimpleServer;

/**
 * Thrift Simple Server
 */
struct _ThriftSimpleServer {
    void *(*getIf)(void *, const char *interface_name);

    ThriftProcessor *processor;
    ThriftServerTransport *server_transport;
    ThriftTransportFactory *input_transport_factory;
    ThriftTransportFactory *output_transport_factory;
    ThriftProtocolFactory *input_protocol_factory;
    ThriftProtocolFactory *output_protocol_factory;

    ThriftCObject thrift_object_interface;
    ThriftServer thrift_server_interface;
};

ThriftSimpleServer *
ThriftSimpleServer_new( ThriftProcessor *processor,
                        ThriftServerTransport *server_transport,
                        ThriftTransportFactory *input_transport_factory,
                        ThriftTransportFactory *output_transport_factory,
                        ThriftProtocolFactory *input_protocol_factory,
                        ThriftProtocolFactory *output_protocol_factory);

int
ThriftSimpleServer_init( ThriftSimpleServer *simple_server,
                         ThriftProcessor *processor,
                         ThriftServerTransport *transport,
                         ThriftTransportFactory *input_transport_factory,
                         ThriftTransportFactory *output_transport_factory,
                         ThriftProtocolFactory *input_protocol_factory,
                         ThriftProtocolFactory *output_protocol_factory);

__END_DECLS

#endif /* _THRIFT_SIMPLE_SERVER_H */

