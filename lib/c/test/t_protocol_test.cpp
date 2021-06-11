#include <cerrno>
#include <iomanip>
#include <vector>
#include <sstream>

#include <boost/endian.hpp>

#define BOOST_TEST_MODULE t_memory_buffer
#include <boost/test/unit_test.hpp>

#include "thrift/c/protocol/t_protocol.h"

using namespace std;

BOOST_AUTO_TEST_CASE(test_t_network_big_endian) {
    uint16_t x16(0xaabb);
    uint32_t x32(0xaabbccdd);
    uint64_t x64(0xaabbccddeeff0011);

    if (boost::endian::order::native == boost::endian::order::big) {        
        BOOST_CHECK_EQUAL(0xaabb, t_network_big_endian.toWire16(x16));
        BOOST_CHECK_EQUAL(0xaabbccdd, t_network_big_endian.toWire32(x32));
        BOOST_CHECK_EQUAL(0xaabbccddeeff0011, t_network_big_endian.toWire64(x64));
    } else {
        BOOST_CHECK_EQUAL(0xbbaa, t_network_big_endian.toWire16(x16));
        BOOST_CHECK_EQUAL(0xddccbbaa, t_network_big_endian.toWire32(x32));
        BOOST_CHECK_EQUAL(0x1100ffeeddccbbaa, t_network_big_endian.toWire64(x64));
    }
}

BOOST_AUTO_TEST_CASE(test_t_network_little_endian) {
    uint16_t x16(0xaabb);
    uint32_t x32(0xaabbccdd);
    uint64_t x64(0xaabbccddeeff0011);

    if (boost::endian::order::native == boost::endian::order::little) {        
        BOOST_CHECK_EQUAL(0xaabb, t_network_little_endian.toWire16(x16));
        BOOST_CHECK_EQUAL(0xaabbccdd, t_network_little_endian.toWire32(x32));
        BOOST_CHECK_EQUAL(0xaabbccddeeff0011, t_network_little_endian.toWire64(x64));
    } else {
        BOOST_CHECK_EQUAL(0xbbaa, t_network_little_endian.toWire16(x16));
        BOOST_CHECK_EQUAL(0xddccbbaa, t_network_little_endian.toWire32(x32));
        BOOST_CHECK_EQUAL(0x1100ffeeddccbbaa, t_network_little_endian.toWire64(x64));
    }
}
