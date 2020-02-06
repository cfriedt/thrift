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

#ifndef _THRIFT_TRANSPORT_H
#define _THRIFT_TRANSPORT_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/*! \file thrift_transport.h
 *  \brief Interface for Thrift transports.
 */

typedef struct _ThriftTransport ThriftTransport;

/*!
 * Thrift Transport interface
 */
struct _ThriftTransport {
  void *(*getIf)(void *self, const char *interface_name);
  bool (*isOpen) (ThriftTransport *transport);
  bool (*peek) (ThriftTransport *transport);
  bool (*open) (ThriftTransport *transport);
  bool (*close) (ThriftTransport *transport);
  int32_t (*read) (ThriftTransport *transport, void *buf, uint32_t len);
  bool (*readEnd) (ThriftTransport *transport);
  bool (*write) (ThriftTransport *transport, const void *buf, const uint32_t len);
  bool (*writeEnd) (ThriftTransport *transport);
  bool (*flush) (ThriftTransport *transport);
  int32_t (*readAll) (ThriftTransport *transport, void *buf, uint32_t len);
};

/* define error/exception types */
typedef enum
{
  THRIFT_TRANSPORT_ERROR_UNKNOWN,
  THRIFT_TRANSPORT_ERROR_HOST,
  THRIFT_TRANSPORT_ERROR_SOCKET,
  THRIFT_TRANSPORT_ERROR_CONNECT,
  THRIFT_TRANSPORT_ERROR_SEND,
  THRIFT_TRANSPORT_ERROR_RECEIVE,
  THRIFT_TRANSPORT_ERROR_CLOSE
} ThriftTransportError;

/* define macro for invalid socket */
#define THRIFT_INVALID_SOCKET (-1)

__END_DECLS

#endif /* _THRIFT_TRANSPORT_H */
