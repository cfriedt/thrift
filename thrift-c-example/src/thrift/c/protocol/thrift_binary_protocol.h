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

#ifndef THRIFT_BINARY_PROTOCOL_H_
#define THRIFT_BINARY_PROTOCOL_H_

#include <sys/cdefs.h>

#include <thrift/c/protocol/thrift_protocol.h>
#include <thrift/c/transport/thrift_transport.h>
#include <thrift/c/thrift_c_object.h>

__BEGIN_DECLS

/*! \file thrift_binary_protocol.h
 *  \brief Binary protocol implementation of a Thrift protocol.  Implements the
 *         ThriftProtocol interface.
 */

/* version numbers */
#define THRIFT_BINARY_PROTOCOL_VERSION_1 0x80010000
#define THRIFT_BINARY_PROTOCOL_VERSION_MASK 0xffff0000

typedef struct _ThriftBinaryProtocol ThriftBinaryProtocol;

/*!
 * Thrift Binary Protocol instance.
 */
struct _ThriftBinaryProtocol {
    void *(*getIf)( void *self, const char *interface_name );

    ThriftTransport *trans;

    ThriftCObject thrift_object_interface;
    ThriftProtocol thrift_protocol_interface;
};

ThriftBinaryProtocol *ThriftBinaryProtocol_new( ThriftTransport *trans );
int ThriftBinaryProtocol_init( ThriftBinaryProtocol *protocol, ThriftTransport *trans );

__END_DECLS

#endif /* THRIFT_BINARY_PROTOCOL_H_ */
