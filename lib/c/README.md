Thrift C Software Library

License
=======

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. See the NOTICE file
distributed with this work for additional information
regarding copyright ownership. The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied. See the License for the
specific language governing permissions and limitations
under the License.

Using Thrift with C
===================

While the C (Glib) library has existed for some time, there are some
situations where using Thrift without the overhead of Glib is desireable.
Namely, on microcontroller platforms with limited resources, or when the
LGPL-v2.0 license of Glib is not compatible with the application.

This C library only uses ISO C11 and aims to be as portable as
possible.

We also offer two options for memory allocation consistent with a number
of embedded C practices - static or dynamic - which are selectable via command
line parameters.

Dependencies
============

None. 

Breaking Changes
================

None.

Initial Implementation
======================

For the initial implementation, we are only aiming for correctness and are not
attempting to optimize for space or speed.

Also, for the initial implementation, we will be likely be using statically
allocated memory.

The probable approach here will involve creating a base object that should be
subclassed, and that will be "tcObject_t". Those types will be reference
counted and possess a possibly-NULL "dtor(tcObject_t *self)" destructor
method. That allows every destructor to be unique. We should also have an
'object_id" field, that gives the object it's ID. Rather than reuse object
id's, let's just keep incrementing them. Having an object_id, but also
ensuring pointer uniqueness of each active object, would allow us to have
multiple instances of the same type of object. It might also be useful to have
a flag that indicates if an object was statically or dynamically allocated.

We might want to increase the versatility of this object by including a couple
of pointer types so that it's easier to implement a list, set, map, or tree.

Each tcObject_t could be associated with up to 3 enumerated types. That way,
a single we can do things like map<string,i32>.

Furthermore, a convervative chunk of memory will be a part of every tcObject_t.

typedef struct tcObject tcObject_t;
struct tcObject {

	size_t refCount;

	// destructor
	void (*dtor)(tcObject_t *self);

	// flags
	struct {
		bool active : 1;
		bool dynamicAlloc : 1;
	};

	unsigned type1; /// used to encode the 
	unsigned type2;
	unsigned type3;

	// memory for simple, scalar data
	union {
		bool b;
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		double d;
		char str[64];
	};

	// memory for extended data (
	void *begin, *end;
};
