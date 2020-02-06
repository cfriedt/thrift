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

#ifndef _THRIFT_SERVER_SOCKET_H
#define _THRIFT_SERVER_SOCKET_H

#include <stdint.h>
#include <sys/cdefs.h>

#include <thrift/c/thrift_c_object.h>
#include "thrift_server_transport.h"

__BEGIN_DECLS

/*! \file thrift_server_socket.h
 *  \brief Socket implementation of a Thrift server transport.  Implements the
 *         ThriftServerTransport class.
 */

typedef struct _ThriftServerSocket ThriftServerSocket;

/*!
 * Thrift ServerSocket instance.
 */
struct _ThriftServerSocket {
  void *(*getIf)(void *self, const char *interface_name);

  /* private */
  char *path;
  unsigned port;
  short backlog;
  int sd;
  uint8_t *buf;
  uint32_t buf_size;
  uint32_t buf_len;

  ThriftCObject thrift_object_interface;
  ThriftServerTransport thrift_server_transport_interface;
};

ThriftServerSocket *
ThriftServerSocket_new( const char *path, unsigned port, short backlog,
                        const uint8_t *buf, uint32_t buf_size );
int
ThriftServerSocket_init( ThriftServerSocket *server_socket, const char *path,
                         unsigned port, short backlog, const uint8_t *buf,
                         uint32_t buf_size );

/* define error/exception types */
typedef enum
{
  THRIFT_SERVER_SOCKET_ERROR_SOCKET,
  THRIFT_SERVER_SOCKET_ERROR_SETSOCKOPT,
  THRIFT_SERVER_SOCKET_ERROR_BIND,
  THRIFT_SERVER_SOCKET_ERROR_LISTEN,
  THRIFT_SERVER_SOCKET_ERROR_ACCEPT,
  THRIFT_SERVER_SOCKET_ERROR_CLOSE
} ThriftServerSocketError;

__END_DECLS

#endif
