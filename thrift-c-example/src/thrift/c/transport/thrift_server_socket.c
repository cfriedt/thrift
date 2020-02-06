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

#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "thrift_server_socket.h"

void ThriftServerSocket_ThriftCObject_unref( void *self ) {

    ThriftServerSocket *_self = thrift_c_container_of( self, ThriftServerSocket, thrift_object_interface );

    if ( _self->thrift_object_interface.reference_count ) {
        _self->thrift_object_interface.reference_count--;
    }

    if ( ! _self->thrift_object_interface.reference_count ) {

        if ( _self->thrift_object_interface.dynamically_allocated ) {
            free( _self );
        }
    }
}

static void *ThriftServerSocket_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    ThriftServerSocket *_self = (ThriftServerSocket *) self;

    if ( false ) {
    } else if ( 0 == strncmp( "ThriftCObject", interface_name, strlen("ThriftCObject") ) ) {
        return & _self->thrift_object_interface;
    } else if ( 0 == strncmp( "ThriftServer", interface_name, strlen("ThriftServer") ) ) {
        return & _self->thrift_server_transport_interface;
    } else if ( 0 == strncmp( "ThriftServerSocket", interface_name, strlen("ThriftServerSocket") ) ) {
        return _self;
    } else {
        return NULL;
    }
}

static void *ThriftServerSocket_ThriftCObject_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftServerSocket_getIf(
        thrift_c_container_of( self, ThriftServerSocket, thrift_object_interface ),
        interface_name
    );
}

static void *ThriftServerSocket_ThriftServerTransport_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftServerSocket_getIf(
        thrift_c_container_of( self, ThriftServerSocket, thrift_server_transport_interface ),
        interface_name
    );
}

static bool ThriftServerSocket_ThriftServerTransport_listen( ThriftServerTransport *transport ) {
    (void) transport;
    return false;
}

static ThriftTransport *ThriftServerSocket_ThriftServerTransport_accept( ThriftServerTransport *transport ) {
    (void) transport;
    return NULL;
}

static bool ThriftServerSocket_ThriftServerTransport_close( ThriftServerTransport *transport ) {
    (void) transport;
    return false;
}

static const ThriftServerSocket thrift_server_socket_proto = {
    .getIf = ThriftServerSocket_getIf,

    .path = NULL,
    .backlog = 0,
    .sd = -1,
    .buf = NULL,
    .buf_size = 0,
    .buf_len = 0,

    .thrift_object_interface = {
        .getIf = ThriftServerSocket_ThriftCObject_getIf,
        .dynamically_allocated = false,
        .reference_count = 0,
        .unref = ThriftServerSocket_ThriftCObject_unref,
    },

    .thrift_server_transport_interface = {
        .getIf = ThriftServerSocket_ThriftServerTransport_getIf,
        .listen = ThriftServerSocket_ThriftServerTransport_listen,
        .accept = ThriftServerSocket_ThriftServerTransport_accept,
        .close = ThriftServerSocket_ThriftServerTransport_close,
    },
};

ThriftServerSocket *
ThriftServerSocket_new( const char *path, unsigned port, short backlog,
                        const uint8_t *buf, uint32_t buf_size )
{

    ThriftServerSocket *server_socket;

    server_socket = (ThriftServerSocket *) malloc( sizeof(*server_socket) );

    if ( NULL == server_socket ) {
        return NULL;
    }

    if ( -1 == ThriftServerSocket_init( server_socket, path, port, backlog,
                                        buf, buf_size ) )
    {
        free( server_socket );
        return NULL;
    }

    server_socket->thrift_object_interface.dynamically_allocated = true;

    return server_socket;
}

int
ThriftServerSocket_init( ThriftServerSocket *server_socket, const char *path,
                         unsigned port, short backlog, const uint8_t *buf,
                         uint32_t buf_size )
{

    const int yes = (int) true;

    int r = -1;

    if (
        false
        || NULL == server_socket
        || port > 65535
        || NULL == buf
        || 0 == buf_size
    ) {
        goto just_return;
    }

    int sd;
    bool is_af_unix = ( '/' == path[ 0 ] );

    if ( is_af_unix ) {

        r = socket( AF_UNIX, SOCK_STREAM, 0 );
        if ( -1 == r ) {
            goto just_return;
        }
        sd = r;

        struct sockaddr_un addr;
        memset(& addr, 0, sizeof(addr));

        if ( strlen( path ) >= sizeof( addr.sun_path ) ) {
            r = -1;
            goto close_socket;
        }

        strncpy( addr.sun_path, path, strlen( path ) );

        r = bind( sd, (struct sockaddr *) & addr, sizeof( addr ) );
        if ( -1 == r ) {
            goto close_socket;
        }

    } else {

        r = socket( AF_INET6, SOCK_STREAM, 0 );
        if ( -1 == r ) {
            goto just_return;
        }
        sd = r;

        r = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, & yes, sizeof( yes ) );
        if ( -1 == r ) {
            goto close_socket;
        }

        struct sockaddr_in6 addr;
        memset(& addr, 0, sizeof(addr));
        addr.sin6_family = AF_INET6;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons( port );

        r = bind( sd, (struct sockaddr *) & addr, sizeof( addr ) );
        if ( -1 == r ) {
            goto close_socket;
        }

    }

    memcpy( server_socket, & thrift_server_socket_proto,
            sizeof( *server_socket ) );

    server_socket->port = port;
    server_socket->path = (char *)path;
    server_socket->backlog = backlog;
    server_socket->buf = (uint8_t *)buf;
    server_socket->buf_size = buf_size;

    server_socket->sd = sd;

    r = 0;

    server_socket->thrift_object_interface.reference_count++;

    goto just_return;

close_socket:
    close( sd );

just_return:

    return r;
}
