SUBDIRS = . test

pkgconfig_DATA = thrift_c.pc
pkgconfigdir = $(libdir)/pkgconfig

lib_LTLIBRARIES = libthrift_c.la

libthrift_c_la_SOURCES = thrift/c/transport/t_transport.c \
                         thrift/c/transport/t_buffer_transports.c \
                         thrift/c/transport/t_socket.c \
                         thrift/c/server/t_simple_server.c \
                         thrift/c/server/t_server.c \
                         thrift/c/protocol/t_protocol.c \
                         thrift/c/protocol/t_binary_protocol.c \
                         thrift/c/thrift.c

libthrift_c_la_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -I$(srcdir)/src

libthrift_c_la_CFLAGS = \
    $(AM_CFLAGS) \
    -Wall -Wextra -Werror \
    -Wno-incompatible-function-pointer-types

include_thriftdir = $(includedir)/thrift/c
#include_thrift_HEADERS = \
#                         $(top_builddir)/config.h \
#                         src/thrift/c/thrift.h \
#                         src/thrift/c/thrift_application_exception.h \
#                         src/thrift/c/thrift_struct.h \
#                         src/thrift/c/thrift_configuration.h

include_processordir = $(include_thriftdir)/processor
include_processor_HEADERS = src/thrift/c/processor/t_processor.h

include_protocoldir = $(include_thriftdir)/protocol
include_protocol_HEADERS = src/thrift/c/protocol/t_binary_protocol.h \
                           src/thrift/c/protocol/t_enum.h \
                           src/thrift/c/protocol/t_protocol.h \
                           src/thrift/c/protocol/t_protocol_factory.h

include_serverdir = $(include_thriftdir)/server
include_server_HEADERS = src/thrift/c/server/t_server.h \
                         src/thrift/c/server/t_simple_server.h

include_transportdir = $(include_thriftdir)/transport
include_transport_HEADERS = src/thrift/c/transport/t_buffer_transports.h \
                            src/thrift/c/transport/t_server_transport.h \
                            src/thrift/c/transport/t_socket.h \
                            src/thrift/c/transport/t_transport.h \
                            src/thrift/c/transport/t_transport_factory.h
