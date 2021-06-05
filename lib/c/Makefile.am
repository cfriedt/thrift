SUBDIRS = . test

pkgconfig_DATA = thrift_c.pc
pkgconfigdir = $(libdir)/pkgconfig

lib_LTLIBRARIES = libthrift_c.la
libthrift_c_la_SOURCES = src/thrift/c/thrift.c

libthrift_c_la_CFLAGS = \
    $(AM_CFLAGS) \
    -Wall -Wextra -Werror
