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

#ifndef THRIFT_C_OBJECT_H_
#define THRIFT_C_OBJECT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct _ThriftCObject ThriftCObject;

struct _ThriftCObject {
    void *(*getIf)(void *self, const char *interface_name);
    bool dynamically_allocated;
    size_t reference_count;
    void (*unref)(void *self);
};

#define thrift_c_container_of(ptr, type, member) \
    ((type *)((uint8_t *)ptr - offsetof(type, member)))

#define thrift_c_object_cast( type, ptr ) \
    ({ \
        void *_x = ptr; \
        type * _y = (type *) ((ThriftCObject *)_x)->getIf( _x, #type ); \
        _y; \
    })

#define thrift_c_object_ref( ptr ) \
        do { \
            ThriftCObject *_x = thrift_c_object_cast( ThriftCObject, ptr ); \
            _x->unref( _x ); \
        } while( 0 )

#define thrift_c_object_unref( ptr ) \
    do { \
        ThriftCObject *_x = thrift_c_object_cast( ThriftCObject, ptr ); \
        _x->unref( _x ); \
    } while( 0 )

__END_DECLS

#endif /* THRIFT_C_OBJECT_H_ */
