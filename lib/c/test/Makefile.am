AUTOMAKE_OPTIONS = subdir-objects serial-tests nostdinc

check_PROGRAMS = \
    t_memory_buffer_test

t_memory_buffer_test_SOURCES = \
    t_memory_buffer_test.cpp
t_memory_buffer_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    $(BOOST_CPPFLAGS) \
    -I$(srcdir)/../src
t_memory_buffer_test_LDADD = \
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
