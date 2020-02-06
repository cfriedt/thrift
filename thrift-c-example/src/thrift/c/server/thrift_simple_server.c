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

#include <stdlib.h>
#include <string.h>

#include "thrift_simple_server.h"

void ThriftSimpleServer_ThriftCObject_unref( void *self ) {

    ThriftSimpleServer *_self = thrift_c_container_of( self, ThriftSimpleServer, thrift_object_interface );

    if ( _self->thrift_object_interface.reference_count ) {
        _self->thrift_object_interface.reference_count--;
    }

    if ( ! _self->thrift_object_interface.reference_count ) {

        thrift_c_object_unref( _self->processor );
        thrift_c_object_unref( _self->server_transport );
        thrift_c_object_unref( _self->input_transport_factory );
        thrift_c_object_unref( _self->output_transport_factory );
        thrift_c_object_unref( _self->input_protocol_factory );
        thrift_c_object_unref( _self->output_protocol_factory );

        if ( _self->thrift_object_interface.dynamically_allocated ) {
            free( _self );
        }
    }
}

static void *ThriftSimpleServer_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    ThriftSimpleServer *_self = (ThriftSimpleServer *) self;

    if ( false ) {
    } else if ( 0 == strncmp( "ThriftCObject", interface_name, strlen("ThriftCObject") ) ) {
        return & _self->thrift_object_interface;
    } else if ( 0 == strncmp( "ThriftServer", interface_name, strlen("ThriftServer") ) ) {
        return & _self->thrift_server_interface;
    } else if ( 0 == strncmp( "ThriftSimpleServer", interface_name, strlen("ThriftSimpleServer") ) ) {
        return _self;
    } else {
        return NULL;
    }
}

static void *ThriftSimpleServer_ThriftCObject_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftSimpleServer_getIf(
        thrift_c_container_of( self, ThriftSimpleServer, thrift_object_interface ),
        interface_name
    );
}

static void *ThriftSimpleServer_ThriftServer_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftSimpleServer_getIf(
        thrift_c_container_of( self, ThriftSimpleServer, thrift_server_interface ),
        interface_name
    );
}

static const ThriftSimpleServer simple_server_proto = {

    .getIf = ThriftSimpleServer_getIf,

    .processor = NULL,
    .server_transport = NULL,
    .input_transport_factory = NULL,
    .output_transport_factory = NULL,
    .input_protocol_factory = NULL,
    .output_protocol_factory = NULL,

    .thrift_object_interface = {
        .getIf = ThriftSimpleServer_ThriftCObject_getIf,
        .dynamically_allocated = false,
        .reference_count = 0,
        .unref = ThriftSimpleServer_ThriftCObject_unref,
    },

    .thrift_server_interface = {
        .getIf = ThriftSimpleServer_ThriftServer_getIf,
    },
};

ThriftSimpleServer *
ThriftSimpleServer_new( ThriftProcessor *processor,
                        ThriftServerTransport *server_transport,
                        ThriftTransportFactory *input_transport_factory,
                        ThriftTransportFactory *output_transport_factory,
                        ThriftProtocolFactory *input_protocol_factory,
                        ThriftProtocolFactory *output_protocol_factory)
{
    ThriftSimpleServer *simple_server;

    simple_server = (ThriftSimpleServer *) malloc( sizeof(ThriftSimpleServer) );

    if ( NULL == simple_server ) {
        return NULL;
    }

    if ( -1 == ThriftSimpleServer_init( simple_server, processor, server_transport,
            input_transport_factory, output_transport_factory,
            input_protocol_factory, output_protocol_factory ) ) {

        free( simple_server );
        return NULL;
    }

    simple_server->thrift_object_interface.dynamically_allocated = true;

    return simple_server;
}

int
ThriftSimpleServer_init( ThriftSimpleServer *simple_server,
                         ThriftProcessor *processor,
                         ThriftServerTransport *server_transport,
                         ThriftTransportFactory *input_transport_factory,
                         ThriftTransportFactory *output_transport_factory,
                         ThriftProtocolFactory *input_protocol_factory,
                         ThriftProtocolFactory *output_protocol_factory)
{
    if (
        false
        || NULL == simple_server
        || NULL == processor
        || NULL == server_transport
        || NULL == input_transport_factory
        || NULL == output_transport_factory
        || NULL == input_protocol_factory
        || NULL == output_protocol_factory
    ) {
        return -1;
    }

    memcpy( simple_server, & simple_server_proto, sizeof( *simple_server ) );

    simple_server->processor = processor;
    simple_server->server_transport = server_transport;
    simple_server->input_transport_factory = input_transport_factory;
    simple_server->output_transport_factory = output_transport_factory;
    simple_server->input_protocol_factory = input_protocol_factory;
    simple_server->output_protocol_factory = output_protocol_factory;

    thrift_c_object_ref( processor );
    thrift_c_object_ref( server_transport );
    thrift_c_object_ref( input_transport_factory );
    thrift_c_object_ref( output_transport_factory );
    thrift_c_object_ref( input_protocol_factory );
    thrift_c_object_ref( output_protocol_factory );

    thrift_c_object_ref( simple_server );

    return 0;
}
