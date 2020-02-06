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

#ifndef _THRIFT_SERVER_TRANSPORT_H
#define _THRIFT_SERVER_TRANSPORT_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include "../../../../src/thrift/c/transport/thrift_transport.h"

__BEGIN_DECLS

/*! \file thrift_server_transport.h
 *  \brief Interface for Thrift server transports.
 */
typedef struct _ThriftServerTransport ThriftServerTransport;

/*!
 * Thrift Transport interface
 */
struct _ThriftServerTransport
{
    void *(*getIf)(void *self, const char *interface_name);
    bool (*listen) (ThriftServerTransport *transport);
    ThriftTransport *(*accept) (ThriftServerTransport *transport);
    bool (*close) (ThriftServerTransport *transport);
};

__END_DECLS

#endif /* _THRIFT_SERVER_TRANSPORT_H */
