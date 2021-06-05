# Thrift Language Binding HowTo

## Introduction

These notes should be fairly portable, language agnostic, and should be of
some assistance to anyone looking to port to add new language bindings.

## Theory

For a good overview, give the
[original Thrift whitepaper](https://thrift.apache.org/static/files/thrift-20070401.pdf)
a read over. In short, thrift works by providing common, per-language runtime
libraries for performing the boilerplate send / receive, encode / decode
layers, and generates object-specific send / receive, encode / decode layers
before build time. By leveraging code generation, a significant amount of
developer time is spared.

This may be a bit hand-wavy, but each language binding uses the same 
fundamental design patterns for encode and decode stages.

E.g. encode `int8_t`, `double`, `string` and so on. Use a factory pattern
for this and that and a common interface for all transports.

TODO: fix the above description

## Disclaimer

> Possibly to the detrement of the author and / or the reader, the language
> binding being added as an example is ISO C. The reason that could be
> detremental, is that C provides many opportunities to shoot oneself in the
> foot, is absolutely unforgiving, is not inherently an object-oriented
> programming language, does not have include built-in data structures,
> includes few algorithms in the standard library, and so on and so forth.
> You have been warned..

### Motivation

The main motivation for creating creating Thrift language bindings in C
is that communications are hard; there are many nuances to reliable
communication, and particularly in C, boilerplate code is a great
place for bugs to hang out. Additionally, many operating system kernels  are
written in C; having a protocol generator written in ISO C will both help make
developers more efficient and reduce the likelihood that bugs will hang
out in places where they really, really should not be.
Furthermore, OS kernels are increasingly required to interface to high- and
low-level software that demands more data, faster, and with a richer and richer contexts.

## Strategy

> Premature optimization is the root of all evil -- Donald Knuth

Note, the following is just one of many possible ways to
proceed. The logic and order that worked well for C follows.

Consider for a moment, that the initial goal here should be *compatibility*
rather than any kind of optimization. If the test suite passes and no 
systematic problems exist, users can begin to use the new language bindings
*sooner*. There is plenty of time after that point to optimize for space
and / or speed and if interfaces are designed well enough, optimizing should
be relatively straight forward.

We're going to use the test suite as a checklist - i.e. use test-driven
development.

### Tranports and Protocols

The very first transport that will be added is one based on TMemoryBuffer
and the first protocol will be TBinaryProtocol. Likely the next transport
that should be added directly after that should be the TSocketTransport.

### Procedure

1. Create a skeleton directory and file structure
2. Add interface definitions for each of TTransport, TServerTransport,
   TProtocol, TProcessor, etc
3. Transpose the *encode* test suite from C++ to C
4. Incrementally improve the C encoder test suite by filling out stubs
5. Test the C encoder with the Python decoder for compatibility
6. Transpose the *decode* test suite from C++ to C
7. Incrementally improve the decoder test suite by filling out stubs
8. Test the C decoder with the Python encoder for compatibility
9.  Test the C encoder with the C decoder
10. Create a tutorial entry as an example of using the new bindings

# Subdirectories and Files

```
compiler/cpp/src/thrift/generate/
└── t_c_generator.cc
lib/c
├── CMakeLists.txt
├── Makefile.am
├── README.md
├── src
│   └── thrift
│       └── c
│           ├── processor
│           │   └── t_protcessor.h
│           ├── protocol
│           │   └── t_protocol.h
│           ├── server
│           │   └── t_server.h
│           ├── transport
│           │   ├── t_server_transport.h
│           │   └── t_transport.h
│           └── thrift.c
├── test
│   ├── CMakeLists.txt
│   └── Makefile.am
└── thrift_c.pc.in
test/c
├── CMakeLists.txt
└── Makefile.am
tutorial/c
└── Makefile.am
```

# Encoder Test Suite

Luckily, the native C++ test framework can also be used to test C code.

