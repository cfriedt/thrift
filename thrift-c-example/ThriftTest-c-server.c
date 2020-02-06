#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thrift/c/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c/server/thrift_server.h>
#include <thrift/c/server/thrift_simple_server.h>
#include <thrift/c/transport/thrift_buffered_transports.h>
#include <thrift/c/transport/thrift_server_socket.h>
#include "ThriftTest_types.h"
#include "ThriftTest.h"

typedef struct _ThriftTestHandler ThriftTestHandler;
struct _ThriftTestHandler {
    void *(*getIf)(void *self, const char *interface_name);

    ThriftCObject thrift_object_interface;
    ThriftTest thrifttest_interface;
};

void ThriftTestHandler_ThriftCObject_unref( void *self ) {

    ThriftTestHandler *_self = thrift_c_container_of( self, ThriftTestHandler, thrift_object_interface );

    if ( _self->thrift_object_interface.reference_count ) {
        _self->thrift_object_interface.reference_count--;
    }

    if ( ! _self->thrift_object_interface.reference_count ) {

        if ( _self->thrift_object_interface.dynamically_allocated ) {
            free( _self );
        }
    }
}

static void *ThriftTestHandler_getIf( void *self, const char *interface_name ) {
    if ( NULL == self || NULL == interface_name ) {
        return NULL;
    }

    ThriftTestHandler *_self = (ThriftTestHandler *)self;

    if ( false ) {
    } else if ( 0 == strncmp( "ThriftCObject", interface_name, strlen( "ThriftCObject" )  ) ) {
        return & _self->thrift_object_interface;
    } else if ( 0 == strncmp( "ThriftTest", interface_name, strlen( "ThriftTest" )  ) ) {
        return & _self->thrifttest_interface;
    } else if ( 0 == strncmp( "ThriftTestHandler", interface_name, strlen( "ThriftTestHandler" )  ) ) {
        return _self;
    } else {
        return NULL;
    }
}

static void *ThriftTestHandler_ThriftCObject_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftTestHandler_getIf(
        thrift_c_container_of( self, ThriftTestHandler, thrift_object_interface ),
        interface_name
    );
}

static void *ThriftTestHandler_ThriftTest_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftTestHandler_getIf(
        thrift_c_container_of( self, ThriftTestHandler, thrifttest_interface ),
        interface_name
    );
}

static int ThriftTestHandler_ThriftTest_testVoid(ThriftTest *self) {
    (void) self;
    printf( "testVoid()\n" );
    return 0;
}

static int ThriftTestHandler_ThriftTest_testString(ThriftTest *self, char **ret, const char *thing) {
    (void) self;
    printf( "testString(\"%s\")\n", thing );
    *ret = (char *)thing;
    return 0;
}

static int ThriftTestHandler_ThriftTest_testBool(ThriftTest *self, bool *ret, const bool thing) {
    (void) self;
    printf( "testBool(\"%s\")\n", thing ? "true" : "false" );
    *ret = thing;
    return 0;
}

static int ThriftTestHandler_ThriftTest_testByte(ThriftTest *self, int8_t *ret, const int8_t thing) {
    (void) self;
    printf( "testByte(\"%d\")\n", thing );
    *ret = thing;
    return 0;
}

static int ThriftTestHandler_ThriftTest_testI32(ThriftTest *self, int32_t *ret, const int32_t thing) {
    (void) self;
    printf( "testByte(\"%d\")\n", thing );
    *ret = thing;
    return 0;
}

static int ThriftTestHandler_ThriftTest_testI64(ThriftTest *self, int64_t *ret, const int64_t thing) {
    (void) self;
    printf( "testByte(\"%" PRId64 "\")\n", thing );
    *ret = thing;
    return 0;
}

static int ThriftTestHandler_ThriftTest_testDouble(ThriftTest *self, double *ret, const double thing) {
    (void) self;
    printf( "testByte(\"%f\")\n", thing );
    *ret = thing;
    return 0;
}

static const ThriftTestHandler thrifttest_handler_proto = {
    .getIf = ThriftTestHandler_getIf,

    .thrift_object_interface = {
        .getIf = ThriftTestHandler_ThriftCObject_getIf,
        .unref = ThriftTestHandler_ThriftCObject_unref,
    },

    .thrifttest_interface = {
        .getIf = ThriftTestHandler_ThriftTest_getIf,
        .testVoid = ThriftTestHandler_ThriftTest_testVoid,
        .testString = ThriftTestHandler_ThriftTest_testString,
        .testBool = ThriftTestHandler_ThriftTest_testBool,
        .testByte = ThriftTestHandler_ThriftTest_testByte,
        .testI32 = ThriftTestHandler_ThriftTest_testI32,
        .testI64 = ThriftTestHandler_ThriftTest_testI64,
        .testDouble = ThriftTestHandler_ThriftTest_testDouble,
    },
};

static int ThriftTestHandler_init(ThriftTestHandler *handler) {
    if ( NULL == handler ) {
        return -1;
    }

    memcpy( handler, & thrifttest_handler_proto, sizeof( *handler ) );

    handler->thrift_object_interface.reference_count++;

    return 0;
}

static ThriftTestHandler *ThriftTestHandler_new(void) {
    ThriftTestHandler *handler = (ThriftTestHandler *) malloc( sizeof(ThriftTestHandler) );
    if ( NULL == handler ) {
        return NULL;
    }

    if ( -1 == ThriftTestHandler_init( handler ) ) {
        free( handler );
        return NULL;
    }

    handler->thrift_object_interface.dynamically_allocated = true;

    return handler;
}

int main(int argc, char **argv) {

    (void) argc;
    (void) argv;

    static uint8_t server_socket_buffer[ 256 ];

    ThriftTest *handler;

    ThriftProcessor *processor;
    ThriftServer *server;
    ThriftServerTransport *server_transport;
    ThriftTransportFactory *transport_factory;
    ThriftProtocolFactory *protocol_factory;

    int exit_status = 0;

    /* Create an instance of our handler, which provides the service's
       methods' implementation */
    handler = thrift_c_object_cast(
        ThriftTest,
        ThriftTestHandler_new()
    );

    /* Create an instance of the service's processor, automatically
       generated by the Thrift compiler, which parses incoming messages
       and dispatches them to the appropriate method in the handler */
    processor = thrift_c_object_cast(
        ThriftProcessor,
        ThriftTestProcessor_new( handler )
    );

    /* Create our server socket, which binds to the specified port and
       listens for client connections */
    server_transport = thrift_c_object_cast(
        ThriftServerTransport,
        ThriftServerSocket_new(
            "localhost",
            9090,
            1,
            server_socket_buffer,
            sizeof( server_socket_buffer )
        )
    );

    /* Create our transport factory, used by the server to wrap "raw"
       incoming connections from the client (in this case with a
       ThriftBufferedTransport to improve performance) */
    transport_factory = thrift_c_object_cast(
        ThriftTransportFactory,
        ThriftBufferedTransportFactory_new()
    );

    /* Create our protocol factory, which determines which wire protocol
       the server will use (in this case, Thrift's binary protocol) */
    protocol_factory = thrift_c_object_cast(
        ThriftProtocolFactory,
        ThriftBinaryProcotolFactory_new()
    );

    /* Create the server itself */
    server = thrift_c_object_cast(
        ThriftServer,
        ThriftSimpleServer_new(
            processor,
            server_transport,
            transport_factory,
            transport_factory,
            protocol_factory,
            protocol_factory
         )
     );

    /* Start the server, which will run until its stop method is invoked
       (from within the SIGINT handler, in this case) */
    puts ("Starting the server...");
    server->serve( server );

    puts ("done.");

    thrift_c_object_unref(server);
    thrift_c_object_unref(transport_factory);
    thrift_c_object_unref(protocol_factory);
    thrift_c_object_unref(server_transport);

    thrift_c_object_unref(processor);
    thrift_c_object_unref(handler);

    return exit_status;
}
