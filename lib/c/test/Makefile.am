AUTOMAKE_OPTIONS = subdir-objects serial-tests nostdinc

BUILT_SOURCES =
CLEANFILES =
noinst_LTLIBRARIES =
check_PROGRAMS =

check_PROGRAMS += \
    bmp_alloc_test
bmp_alloc_test_SOURCES = \
    bmp_alloc_test.cpp
bmp_alloc_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
bmp_alloc_test_LDADD = \
  $(top_builddir)/lib/c/libthrift_c.la \
  $(BOOST_TEST_LDADD) \
  $(BOOST_SYSTEM_LDADD) \
  /opt/homebrew/Cellar/boost/1.76.0/lib/libboost_thread-mt.a

check_PROGRAMS += \
    t_buffer_transports_test
t_buffer_transports_test_SOURCES = \
    t_buffer_transports_test.cpp
t_buffer_transports_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
t_buffer_transports_test_LDADD = \
  $(top_builddir)/lib/c/libthrift_c.la \
  $(BOOST_TEST_LDADD) \
  $(BOOST_SYSTEM_LDADD) \
  /opt/homebrew/Cellar/boost/1.76.0/lib/libboost_thread-mt.a

check_PROGRAMS += t_socket_test
t_socket_test_SOURCES = t_socket_test.cpp
t_socket_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
t_socket_test_LDADD = \
  $(top_builddir)/lib/c/libthrift_c.la \
  $(BOOST_TEST_LDADD) \
  $(BOOST_SYSTEM_LDADD) \
  /opt/homebrew/Cellar/boost/1.76.0/lib/libboost_thread-mt.a

check_PROGRAMS += t_protocol_test
t_protocol_test_SOURCES = t_protocol_test.cpp
t_protocol_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
t_protocol_test_LDADD = \
  $(top_builddir)/lib/c/libthrift_c.la \
  $(BOOST_TEST_LDADD) \
  $(BOOST_SYSTEM_LDADD) \
  /opt/homebrew/Cellar/boost/1.76.0/lib/libboost_thread-mt.a

check_PROGRAMS += t_binary_protocol_test
t_binary_protocol_test_SOURCES = t_binary_protocol_test.cpp
t_binary_protocol_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
t_binary_protocol_test_LDADD = \
  $(top_builddir)/lib/c/libthrift_c.la \
  $(BOOST_TEST_LDADD) \
  $(BOOST_SYSTEM_LDADD) \
  /opt/homebrew/Cellar/boost/1.76.0/lib/libboost_thread-mt.a

TESTS_ENVIRONMENT= \
	BOOST_TEST_LOG_SINK=tests.xml \
	BOOST_TEST_LOG_LEVEL=test_suite \
	BOOST_TEST_LOG_FORMAT=XML

TESTS = \
	$(check_PROGRAMS)
