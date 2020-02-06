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

#include "thrift_binary_protocol.h"

void ThriftBinaryProtocol_ThriftCObject_unref( void *self ) {

    ThriftBinaryProtocol *_self = thrift_c_container_of( self, ThriftBinaryProtocol, thrift_object_interface );

    if ( _self->thrift_object_interface.reference_count ) {
        _self->thrift_object_interface.reference_count--;
    }

    if ( ! _self->thrift_object_interface.reference_count ) {

        thrift_c_object_unref( _self->trans );

        if ( _self->thrift_object_interface.dynamically_allocated ) {
            free( _self );
        }
    }
}

static void *ThriftBinaryProtocol_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    ThriftBinaryProtocol *_self = (ThriftBinaryProtocol *) self;

    if ( false ) {
    } else if ( 0 == strncmp( "ThriftCObject", interface_name, strlen("ThriftCObject") ) ) {
        return & _self->thrift_object_interface;
    } else if ( 0 == strncmp( "ThriftProtocol", interface_name, strlen("ThriftProtocol") ) ) {
        return & _self->thrift_protocol_interface;
    } else if ( 0 == strncmp( "ThriftBinaryProtocol", interface_name, strlen("ThriftBinaryProtocol") ) ) {
        return _self;
    } else {
        return NULL;
    }
}

static void *ThriftBinaryProtocol_ThriftCObject_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftBinaryProtocol_getIf(
        thrift_c_container_of( self, ThriftBinaryProtocol, thrift_object_interface ),
        interface_name
    );
}

static void *ThriftBinaryProtocol_ThriftProtocol_getIf( void *self, const char *interface_name ) {

    if ( NULL == interface_name || NULL == self ) {
        return NULL;
    }

    return ThriftBinaryProtocol_getIf(
        thrift_c_container_of( self, ThriftBinaryProtocol, thrift_protocol_interface ),
        interface_name
    );
}

static int ThriftBinaryProtocol_ThriftProtocol_writeMessageBegin(ThriftProtocol *protocol, const char *name, const ThriftMessageType message_type, const int seqid) {
    (void) protocol;
    (void) name;
    (void) message_type;
    (void) seqid;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeMessageEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeStructBegin(ThriftProtocol *protocol, const char *name) {
    (void) protocol;
    (void) name;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeStructEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeFieldBegin(ThriftProtocol *protocol, const char *name, const ThriftType field_type, const int16_t field_id) {
    (void) protocol;
    (void) name;
    (void) field_type;
    (void) field_id;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeFieldEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeFieldStop(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeMapBegin(ThriftProtocol *protocol, const ThriftType key_type, const ThriftType value_type, const uint32_t size) {
    (void) protocol;
    (void) key_type;
    (void) value_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeMapEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeListBegin(ThriftProtocol *protocol, const ThriftType element_type, const uint32_t size) {
    (void) protocol;
    (void) element_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeListEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeSetBegin(ThriftProtocol *protocol, const ThriftType element_type, const uint32_t size) {
    (void) protocol;
    (void) element_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeSetEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeBool(ThriftProtocol *protocol, const bool value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeByte(ThriftProtocol *protocol, const int8_t value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeI16(ThriftProtocol *protocol, const int16_t value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeI32(ThriftProtocol *protocol, const int32_t value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeI64(ThriftProtocol *protocol, const int64_t value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeDouble(ThriftProtocol *protocol, const double value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeString(ThriftProtocol *protocol, const char *str) {
    (void) protocol;
    (void) str;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_writeBinary(ThriftProtocol *protocol, const void * buf, const uint32_t len) {
    (void) protocol;
    (void) buf;
    (void) len;
    return -1;
}

static int ThriftBinaryProtocol_ThriftProtocol_readMessageBegin(ThriftProtocol *protocol, char **name, ThriftMessageType *message_type, int *seqid) {
    (void) protocol;
    (void) name;
    (void) message_type;
    (void) seqid;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readMessageEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readStructBegin(ThriftProtocol *protocol, char **name) {
    (void) protocol;
    (void) name;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readStructEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readFieldBegin(ThriftProtocol *protocol, char **name, ThriftType *field_type, int16_t *field_id) {
    (void) protocol;
    (void) name;
    (void) field_type;
    (void) field_id;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readFieldEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readMapBegin(ThriftProtocol *protocol, ThriftType *key_type, ThriftType *value_type, uint32_t *size) {
    (void) protocol;
    (void) key_type;
    (void) value_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readMapEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readListBegin(ThriftProtocol *protocol, ThriftType *element_type, uint32_t *size) {
    (void) protocol;
    (void) element_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readListEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readSetBegin(ThriftProtocol *protocol, ThriftType *element_type, uint32_t *size) {
    (void) protocol;
    (void) element_type;
    (void) size;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readSetEnd(ThriftProtocol *protocol) {
    (void) protocol;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readBool(ThriftProtocol *protocol, bool *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readByte(ThriftProtocol *protocol, int8_t *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readI16(ThriftProtocol *protocol, int16_t *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readI32(ThriftProtocol *protocol, int *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readI64(ThriftProtocol *protocol, int64_t *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readDouble(ThriftProtocol *protocol, double *value) {
    (void) protocol;
    (void) value;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readString(ThriftProtocol *protocol, char **str) {
    (void) protocol;
    (void) str;
    return -1;
}
static int ThriftBinaryProtocol_ThriftProtocol_readBinary(ThriftProtocol *protocol, void **buf, uint32_t *len) {
    (void) protocol;
    (void) buf;
    (void) len;
    return -1;
}

static ThriftTransport *ThriftBinaryProtocol_ThriftProtocol_getTransport(ThriftProtocol *protocol) {
    ThriftBinaryProtocol *binary_protocol = thrift_c_object_cast( ThriftBinaryProtocol, protocol );
    return binary_protocol->trans;
}

static const ThriftBinaryProtocol binary_protocol_proto = {

    .getIf = ThriftBinaryProtocol_getIf,

    .trans = NULL,

    .thrift_object_interface = {
        .getIf = ThriftBinaryProtocol_ThriftCObject_getIf,
        .dynamically_allocated = false,
        .reference_count = 0,
        .unref = ThriftBinaryProtocol_ThriftCObject_unref,
    },

    .thrift_protocol_interface = {
        .getIf = ThriftBinaryProtocol_ThriftProtocol_getIf,
        .writeMessageBegin = ThriftBinaryProtocol_ThriftProtocol_writeMessageBegin,
        .writeMessageEnd = ThriftBinaryProtocol_ThriftProtocol_writeMessageEnd,
        .writeStructBegin = ThriftBinaryProtocol_ThriftProtocol_writeStructBegin,
        .writeStructEnd = ThriftBinaryProtocol_ThriftProtocol_writeStructEnd,
        .writeFieldBegin = ThriftBinaryProtocol_ThriftProtocol_writeFieldBegin,
        .writeFieldEnd = ThriftBinaryProtocol_ThriftProtocol_writeFieldEnd,
        .writeFieldStop = ThriftBinaryProtocol_ThriftProtocol_writeFieldStop,
        .writeMapBegin = ThriftBinaryProtocol_ThriftProtocol_writeMapBegin,
        .writeMapEnd = ThriftBinaryProtocol_ThriftProtocol_writeMapEnd,
        .writeListBegin = ThriftBinaryProtocol_ThriftProtocol_writeListBegin,
        .writeListEnd = ThriftBinaryProtocol_ThriftProtocol_writeListEnd,
        .writeSetBegin = ThriftBinaryProtocol_ThriftProtocol_writeSetBegin,
        .writeSetEnd = ThriftBinaryProtocol_ThriftProtocol_writeSetEnd,
        .writeBool = ThriftBinaryProtocol_ThriftProtocol_writeBool,
        .writeByte = ThriftBinaryProtocol_ThriftProtocol_writeByte,
        .writeI16 = ThriftBinaryProtocol_ThriftProtocol_writeI16,
        .writeI32 = ThriftBinaryProtocol_ThriftProtocol_writeI32,
        .writeI64 = ThriftBinaryProtocol_ThriftProtocol_writeI64,
        .writeDouble = ThriftBinaryProtocol_ThriftProtocol_writeDouble,
        .writeString = ThriftBinaryProtocol_ThriftProtocol_writeString,
        .writeBinary = ThriftBinaryProtocol_ThriftProtocol_writeBinary,
        .readMessageBegin = ThriftBinaryProtocol_ThriftProtocol_readMessageBegin,
        .readMessageEnd = ThriftBinaryProtocol_ThriftProtocol_readMessageEnd,
        .readStructBegin = ThriftBinaryProtocol_ThriftProtocol_readStructBegin,
        .readStructEnd = ThriftBinaryProtocol_ThriftProtocol_readStructEnd,
        .readFieldBegin = ThriftBinaryProtocol_ThriftProtocol_readFieldBegin,
        .readFieldEnd = ThriftBinaryProtocol_ThriftProtocol_readFieldEnd,
        .readMapBegin = ThriftBinaryProtocol_ThriftProtocol_readMapBegin,
        .readMapEnd = ThriftBinaryProtocol_ThriftProtocol_readMapEnd,
        .readListBegin = ThriftBinaryProtocol_ThriftProtocol_readListBegin,
        .readListEnd = ThriftBinaryProtocol_ThriftProtocol_readListEnd,
        .readSetBegin = ThriftBinaryProtocol_ThriftProtocol_readSetBegin,
        .readSetEnd = ThriftBinaryProtocol_ThriftProtocol_readSetEnd,
        .readBool = ThriftBinaryProtocol_ThriftProtocol_readBool,
        .readByte = ThriftBinaryProtocol_ThriftProtocol_readByte,
        .readI16 = ThriftBinaryProtocol_ThriftProtocol_readI16,
        .readI32 = ThriftBinaryProtocol_ThriftProtocol_readI32,
        .readI64 = ThriftBinaryProtocol_ThriftProtocol_readI64,
        .readDouble = ThriftBinaryProtocol_ThriftProtocol_readDouble,
        .readString = ThriftBinaryProtocol_ThriftProtocol_readString,
        .readBinary = ThriftBinaryProtocol_ThriftProtocol_readBinary,
        .getTransport = ThriftBinaryProtocol_ThriftProtocol_getTransport,
    },
};

ThriftBinaryProtocol *
ThriftBinaryProtocol_new( ThriftTransport *trans )
{
    ThriftBinaryProtocol *protocol;

    protocol = (ThriftBinaryProtocol *) malloc( sizeof(ThriftBinaryProtocol) );

    if ( NULL == protocol ) {
        return NULL;
    }

    if ( -1 == ThriftBinaryProtocol_init( protocol, trans ) ) {
        free( protocol );
        return NULL;
    }

    protocol->thrift_object_interface.dynamically_allocated = true;

    return protocol;
}

int
ThriftBinaryProtocol_init( ThriftBinaryProtocol *protocol,
                         ThriftTransport *trans )
{
    if ( NULL == trans ) {
        return -1;
    }

    memcpy( protocol, & binary_protocol_proto, sizeof( *protocol ) );

    protocol->trans = trans;

    thrift_c_object_ref( trans );

    thrift_c_object_ref( protocol );

    return 0;
}
