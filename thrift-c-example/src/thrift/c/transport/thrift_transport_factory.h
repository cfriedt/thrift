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

#ifndef _THRIFT_TRANSPORT_FACTORY_H
#define _THRIFT_TRANSPORT_FACTORY_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include <src/thrift/c/transport/thrift_transport.h>

__BEGIN_DECLS

/*! \file thrift_transport_factory.h
 *  \brief Base class for Thrift Transport Factories.  Used by Thrift Servers
 *         to obtain a client transport from an existing transport.  The default
 *         implementation simply clones the provided transport.
 */

typedef struct _ThriftTransportFactory ThriftTransportFactory;

struct _ThriftTransportFactory
{
    void *(*getIf)(void *self, const char *interface_name);
    ThriftTransport *(*get_transport) (ThriftTransportFactory *factory,
                                     ThriftTransport *transport);
};

__END_DECLS

#endif /* _THRIFT_TRANSPORT_FACTORY_H */
