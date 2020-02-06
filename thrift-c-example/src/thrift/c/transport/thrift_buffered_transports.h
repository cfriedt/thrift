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

#ifndef _THRIFT_BUFFERED_TRANSPORT_FACTORY_H
#define _THRIFT_BUFFERED_TRANSPORT_FACTORY_H

#include <sys/cdefs.h>

#include <thrift/c/thrift_c_object.h>
#include <thrift/c/transport/thrift_transport.h>
#include <thrift/c/transport/thrift_transport_factory.h>

__BEGIN_DECLS

/*! \file thrift_buffered_transport_factory.h
 *  \brief Wraps a transport with a ThriftBufferedTransport.
 */

#define THRIFT_C_BUFFERED_TRANSPORT_DEFAULT_BUFFER_SIZE 512 /* bytes */

typedef struct _ThriftBufferedTransport ThriftBufferedTransport;
struct _ThriftBufferedTransport {
    void *(*getIf)(void *self, const char *interface_name);

    uint8_t *buffer;
    size_t size;

    ThriftCObject object_interface;
    ThriftTransport transport_interface;
};
ThriftBufferedTransport *ThriftBufferedTransport_new( uint8_t *buffer, size_t size );
int ThriftBufferedTransport_init( ThriftBufferedTransport *transport, uint8_t *buffer, size_t size );

typedef struct _ThriftBufferedTransportFactory ThriftBufferedTransportFactory;
struct _ThriftBufferedTransportFactory {
    void *(*getIf)(void *self, const char *interface_name);

    ThriftCObject thrift_object_interface;
    ThriftTransportFactory transport_factory_interface;
};

int
ThriftBufferedTransportFactory_init(ThriftBufferedTransportFactory *factory);

ThriftBufferedTransportFactory *
ThriftBufferedTransportFactory_new(void);

__END_DECLS

#endif /* _THRIFT_BUFFERED_TRANSPORT_FACTORY_H */
