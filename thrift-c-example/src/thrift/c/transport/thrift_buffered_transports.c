#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "thrift_buffered_transports.h"

void ThriftBufferedTransportFactory_ThriftCObject_unref( void *self ) {

    ThriftBufferedTransportFactory *_self = thrift_c_container_of( self, ThriftBufferedTransportFactory, thrift_object_interface );

    if ( _self->thrift_object_interface.reference_count ) {
        _self->thrift_object_interface.reference_count--;
    }

    if ( ! _self->thrift_object_interface.reference_count ) {

        if ( _self->thrift_object_interface.dynamically_allocated ) {
            free( _self );
        }
    }
}

static void *ThriftBufferedTransportFactory_getIf(void *self, const char *interface_name) {
    if ( NULL == self || NULL == interface_name ) {
        return NULL;
    }

    ThriftBufferedTransportFactory *_self = (ThriftBufferedTransportFactory *)self;

    if ( false ) {
    } else if ( 0 == strncmp( "ThriftBufferedTransportFactory", interface_name, strlen( "ThriftBufferedTransportFactory" ) ) ) {
            return _self;
    } else if ( 0 == strncmp( "ThriftTransportFactory", interface_name, strlen( "ThriftTransportFactory" ) ) ) {
            return & _self->transport_factory_interface;
    } else if ( 0 == strncmp( "ThriftCObject", interface_name, strlen( "ThriftCObject" ) ) ) {
            return & _self->thrift_object_interface;
    } else {
        return NULL;
    }
}

static void *ThriftBufferedTransportFactory_ThriftCObject_getIf(void *self, const char *interface_name) {
    if ( NULL == self || NULL == interface_name ) {
        return NULL;
    }

    return ThriftBufferedTransportFactory_getIf(
        thrift_c_container_of( self, ThriftBufferedTransportFactory, thrift_object_interface ),
        interface_name
    );
}

static void *ThriftBufferedTransportFactory_ThriftTransportFactory_getIf(void *self, const char *interface_name) {
    if ( NULL == self || NULL == interface_name ) {
        return NULL;
    }

    return ThriftBufferedTransportFactory_getIf(
        thrift_c_container_of( self, ThriftBufferedTransportFactory, transport_factory_interface ),
        interface_name
    );
}

static ThriftTransport *ThriftBufferedTransportFactory_ThriftTransportFactory_get_transport( ThriftTransportFactory *self, ThriftTransport *transport ) {
    ThriftBufferedTransportFactory *_self = thrift_c_container_of( self, ThriftBufferedTransportFactory, transport_factory_interface );
}

static const ThriftBufferedTransportFactory thrift_buffered_transport_factory_proto = {
    .getIf = ThriftBufferedTransportFactory_getIf,

    .thrift_object_interface = {
        .getIf = ThriftBufferedTransportFactory_ThriftCObject_getIf,
        .dynamically_allocated = false,
        .reference_count = 0,
        .unref = ThriftBufferedTransportFactory_ThriftCObject_unref,
    },

    .transport_factory_interface = {
        .getIf = ThriftBufferedTransportFactory_ThriftTransportFactory_getIf,

    },
};

ThriftBufferedTransportFactory *ThriftBufferedTransportFactory_new( void ) {

    ThriftBufferedTransportFactory *factory = (ThriftBufferedTransportFactory *) malloc( sizeof( ThriftBufferedTransportFactory ) );

    if ( NULL == factory ) {
        return NULL;
    }

    if ( -1 == ThriftBufferedTransportFactory_init( factory ) ) {
        free( factory );
        return NULL;
    }

    return NULL;
}

int ThriftBufferedTransportFactory_init( ThriftBufferedTransportFactory *factory ) {

    if ( NULL == factory ) {
        return -1;
    }

    memcpy( factory, & thrift_buffered_transport_factory_proto, sizeof( *factory ) );

    return 0;
}
