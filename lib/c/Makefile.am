SUBDIRS = . test

pkgconfig_DATA = thrift_c.pc
pkgconfigdir = $(libdir)/pkgconfig

lib_LTLIBRARIES = libthrift_c.la

libthrift_c_la_SOURCES = \
    src/thrift/c/protocol/t_protocol.c \
    src/thrift/c/protocol/t_binary_protocol.c \
    src/thrift/c/server/t_server.c \
    src/thrift/c/server/t_simple_server.c \
    src/thrift/c/thrift.c \
    src/thrift/c/transport/t_buffer_transports.c \
    src/thrift/c/transport/t_socket.c \
    src/thrift/c/transport/t_transport.c

libthrift_c_la_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -I$(srcdir)/src

libthrift_c_la_CFLAGS = \
    $(AM_CFLAGS) \
    -Wall -Wextra -Werror \
    -Wno-incompatible-function-pointer-types
