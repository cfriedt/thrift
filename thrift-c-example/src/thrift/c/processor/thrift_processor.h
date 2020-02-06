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

#ifndef _THRIFT_PROCESSOR_H
#define _THRIFT_PROCESSOR_H

#include <sys/cdefs.h>

#include <thrift/c/thrift_c_object.h>
#include <thrift/c/protocol/thrift_protocol.h>

__BEGIN_DECLS

/*! \file thrift_processor.h
 *  \brief Abstract class for Thrift processors.
 */

typedef struct _ThriftProcessor ThriftProcessor;

/*!
 * Thrift Processor interface
 */
struct _ThriftProcessor {
    void *(*getIf)(void *self, const char *interface_name);
    bool (*process)(ThriftProcessor *processor, ThriftProtocol *in, ThriftProtocol *out);
};

__END_DECLS

#endif /* _THRIFT_PROCESSOR_H */
