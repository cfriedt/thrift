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

#ifndef _THRIFT_PROTOCOL_H
#define _THRIFT_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include <thrift/c/transport/thrift_transport.h>

__BEGIN_DECLS

/*! \file thrift_protocol.h
 *  \brief Abstract class for Thrift protocol implementations.
 */

/**
 * Enumerated definition of the types that the Thrift protocol supports.
 * Take special note of the T_END type which is used specifically to mark
 * the end of a sequence of fields.
 */
typedef enum {
  T_STOP   = 0,
  T_VOID   = 1,
  T_BOOL   = 2,
  T_BYTE   = 3,
  T_I08    = 3,
  T_I16    = 6,
  T_I32    = 8,
  T_U64    = 9,
  T_I64    = 10,
  T_DOUBLE = 4,
  T_STRING = 11,
  T_UTF7   = 11,
  T_STRUCT = 12,
  T_MAP    = 13,
  T_SET    = 14,
  T_LIST   = 15,
  T_UTF8   = 16,
  T_UTF16  = 17
} ThriftType;

/**
 * Enumerated definition of the message types that the Thrift protocol
 * supports.
 */
typedef enum {
  T_CALL      = 1,
  T_REPLY     = 2,
  T_EXCEPTION = 3,
  T_ONEWAY    = 4
} ThriftMessageType;

typedef struct _ThriftProtocol ThriftProtocol;

/*!
 * Thrift Protocol interface
 */
struct _ThriftProtocol {

    void *(*getIf)(void *self, const char *interface_name);

    int (*writeMessageBegin) (ThriftProtocol *protocol, const char *name, const ThriftMessageType message_type, const int seqid);
    int (*writeMessageEnd) (ThriftProtocol *protocol);
    int (*writeStructBegin) (ThriftProtocol *protocol, const char *name);
    int (*writeStructEnd) (ThriftProtocol *protocol);
    int (*writeFieldBegin) (ThriftProtocol *protocol, const char *name, const ThriftType field_type, const int16_t field_id);
    int (*writeFieldEnd) (ThriftProtocol *protocol);
    int (*writeFieldStop) (ThriftProtocol *protocol);
    int (*writeMapBegin) (ThriftProtocol *protocol, const ThriftType key_type, const ThriftType value_type, const uint32_t size);
    int (*writeMapEnd) (ThriftProtocol *protocol);
    int (*writeListBegin) (ThriftProtocol *protocol, const ThriftType element_type, const uint32_t size);
    int (*writeListEnd) (ThriftProtocol *protocol);
    int (*writeSetBegin) (ThriftProtocol *protocol, const ThriftType element_type, const uint32_t size);
    int (*writeSetEnd) (ThriftProtocol *protocol);
    int (*writeBool) (ThriftProtocol *protocol, const bool value);
    int (*writeByte) (ThriftProtocol *protocol, const int8_t value);
    int (*writeI16) (ThriftProtocol *protocol, const int16_t value);
    int (*writeI32) (ThriftProtocol *protocol, const int value);
    int (*writeI64) (ThriftProtocol *protocol, const int64_t value);
    int (*writeDouble) (ThriftProtocol *protocol, const double value);
    int (*writeString) (ThriftProtocol *protocol, const char *str);
    int (*writeBinary) (ThriftProtocol *protocol, const void * buf, const uint32_t len);

    int (*readMessageBegin) (ThriftProtocol *thrift_protocol, char **name, ThriftMessageType *message_type, int *seqid);
    int (*readMessageEnd) (ThriftProtocol *protocol);
    int (*readStructBegin) (ThriftProtocol *protocol, char **name);
    int (*readStructEnd) (ThriftProtocol *protocol);
    int (*readFieldBegin) (ThriftProtocol *protocol, char **name, ThriftType *field_type, int16_t *field_id);
    int (*readFieldEnd) (ThriftProtocol *protocol);
    int (*readMapBegin) (ThriftProtocol *protocol, ThriftType *key_type, ThriftType *value_type, uint32_t *size);
    int (*readMapEnd) (ThriftProtocol *protocol);
    int (*readListBegin) (ThriftProtocol *protocol, ThriftType *element_type, uint32_t *size);
    int (*readListEnd) (ThriftProtocol *protocol);
    int (*readSetBegin) (ThriftProtocol *protocol, ThriftType *element_type, uint32_t *size);
    int (*readSetEnd) (ThriftProtocol *protocol);
    int (*readBool) (ThriftProtocol *protocol, bool *value);
    int (*readByte) (ThriftProtocol *protocol, int8_t *value);
    int (*readI16) (ThriftProtocol *protocol, int16_t *value);
    int (*readI32) (ThriftProtocol *protocol, int *value);
    int (*readI64) (ThriftProtocol *protocol, int64_t *value);
    int (*readDouble) (ThriftProtocol *protocol, double *value);
    int (*readString) (ThriftProtocol *protocol, char **str);
    int (*readBinary) (ThriftProtocol *protocol, void **buf, uint32_t *len);

  ThriftTransport *(*getTransport)(ThriftProtocol *protocol);
};

//int thrift_protocol_skip (ThriftProtocol *protocol, ThriftType type);

/* define error types */
typedef enum
{
  THRIFT_PROTOCOL_ERROR_UNKNOWN,
  THRIFT_PROTOCOL_ERROR_INVALID_DATA,
  THRIFT_PROTOCOL_ERROR_NEGATIVE_SIZE,
  THRIFT_PROTOCOL_ERROR_SIZE_LIMIT,
  THRIFT_PROTOCOL_ERROR_BAD_VERSION,
  THRIFT_PROTOCOL_ERROR_NOT_IMPLEMENTED,
  THRIFT_PROTOCOL_ERROR_DEPTH_LIMIT
} ThriftProtocolError;

__END_DECLS

#endif /* _THRIFT_PROTOCOL_H */
