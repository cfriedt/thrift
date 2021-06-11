#include <cerrno>
#include <iomanip>
#include <vector>
#include <sstream>

#define BOOST_TEST_MODULE t_binary_protocol
#include <boost/test/unit_test.hpp>

#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/transport/t_buffer_transports.h"

using namespace std;

static vector<uint8_t> buffer(1024);
static t_memory_buffer _transport = {};
static t_transport *t = (t_transport *)&_transport;
static t_binary_protocol _protocol = {};
static t_protocol *p = (t_protocol *)&_protocol;
static const string hi = "Hello, world!";

string to_string(const vector<uint8_t>& v)
{
    stringstream ss;

    for(size_t i = 0, n = v.size(); i < n; ++i) {
        if (isprint(v[i])) {
            ss << " " << v[i];
        } else {
            ss << hex << setw(2) << setfill('0') << int(v[i]);
        }

        if (i < n - 1) {
            ss << ", ";
        }
    }

    return ss.str();
}

ostream& operator<<(ostream& os, const vector<uint8_t>& v)
{
    return os << v;
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_init) {
    BOOST_CHECK_EQUAL(-EINVAL, t_binary_protocol_init(nullptr, t, &t_network_big_endian));
    BOOST_CHECK_EQUAL(-EINVAL, t_binary_protocol_init(&_protocol, nullptr, &t_network_big_endian));
    BOOST_CHECK_EQUAL(0, t_binary_protocol_init(&_protocol, t, &t_network_little_endian));
    BOOST_CHECK_EQUAL(_protocol.bo, &t_network_little_endian);

    BOOST_CHECK_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));
    BOOST_CHECK(_protocol.writeMessageBegin != nullptr);
    BOOST_CHECK(_protocol.writeMessageEnd != nullptr);
    BOOST_CHECK(_protocol.writeStructBegin != nullptr);
    BOOST_CHECK(_protocol.writeStructEnd != nullptr);
    BOOST_CHECK(_protocol.writeFieldBegin != nullptr);
    BOOST_CHECK(_protocol.writeFieldEnd != nullptr);
    BOOST_CHECK(_protocol.writeFieldStop != nullptr);
    BOOST_CHECK(_protocol.writeMapBegin != nullptr);
    BOOST_CHECK(_protocol.writeMapEnd != nullptr);
    BOOST_CHECK(_protocol.writeListBegin != nullptr);
    BOOST_CHECK(_protocol.writeListEnd != nullptr);
    BOOST_CHECK(_protocol.writeSetBegin != nullptr);
    BOOST_CHECK(_protocol.writeSetEnd != nullptr);
    BOOST_CHECK(_protocol.writeBool != nullptr);
    BOOST_CHECK(_protocol.writeByte != nullptr);
    BOOST_CHECK(_protocol.writeI16 != nullptr);
    BOOST_CHECK(_protocol.writeI32 != nullptr);
    BOOST_CHECK(_protocol.writeI64 != nullptr);
    BOOST_CHECK(_protocol.writeDouble != nullptr);
    BOOST_CHECK(_protocol.writeString != nullptr);
    BOOST_CHECK(_protocol.readMessageBegin != nullptr);
    BOOST_CHECK(_protocol.readMessageEnd != nullptr);
    BOOST_CHECK(_protocol.readStructBegin != nullptr);
    BOOST_CHECK(_protocol.readStructEnd != nullptr);
    BOOST_CHECK(_protocol.readFieldBegin != nullptr);
    BOOST_CHECK(_protocol.readFieldEnd != nullptr);
    BOOST_CHECK(_protocol.readMapBegin != nullptr);
    BOOST_CHECK(_protocol.readMapEnd != nullptr);
    BOOST_CHECK(_protocol.readListBegin != nullptr);
    BOOST_CHECK(_protocol.readListEnd != nullptr);
    BOOST_CHECK(_protocol.readSetBegin != nullptr);
    BOOST_CHECK(_protocol.readSetEnd != nullptr);
    BOOST_CHECK(_protocol.readBool != nullptr);
    BOOST_CHECK(_protocol.readByte != nullptr);
    BOOST_CHECK(_protocol.readI16 != nullptr);
    BOOST_CHECK(_protocol.readI32 != nullptr);
    BOOST_CHECK(_protocol.readI64 != nullptr);
    BOOST_CHECK(_protocol.readDouble != nullptr);
    BOOST_CHECK(_protocol.readString != nullptr);
    BOOST_CHECK_EQUAL(_protocol.trans, t);
    BOOST_CHECK_EQUAL(_protocol.bo, &t_network_big_endian);
}

/*
 * W R I T E
 */

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeMessageBegin) {
    fill(buffer.begin(), buffer.end(), 0);
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeMessageBegin(nullptr, "foo", T_CALL, 42));
    BOOST_CHECK_EQUAL(-EINVAL, p->writeMessageBegin(p, nullptr, T_CALL, 42));
    BOOST_CHECK_EQUAL(-EINVAL, p->writeMessageBegin(p, "foo", (enum t_message_type)42, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(15, p->writeMessageBegin(p, "foo", T_CALL, 42));

    vector<uint8_t> expected {0x80, 0x01, 0x00, T_CALL, 0x00, 0x00, 0x00, 3, 'f', 'o', 'o', 0, 0, 0, 42};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 15);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeMessageEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeMessageEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeMessageEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeStructBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeStructBegin(nullptr, "foo"));
    BOOST_CHECK_EQUAL(0, p->writeStructBegin(p, nullptr));
    BOOST_CHECK_EQUAL(0, p->writeStructBegin(p, "foo"));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeStructEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeStructEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeStructEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeFieldBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeFieldBegin(nullptr, "foo", T_BYTE, 42));
    BOOST_CHECK_EQUAL(3, p->writeFieldBegin(p, nullptr, T_BYTE, 42));
    BOOST_CHECK_EQUAL(3, p->writeFieldBegin(p, "foo", (enum t_type)42, 0xffffffff));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(3, p->writeFieldBegin(p, "foo", T_BYTE, 0xaabb));

    vector<uint8_t> expected {T_BYTE, 0xaa, 0xbb};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 3);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeFieldEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeFieldEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeFieldEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeFieldStop) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeFieldStop(nullptr));
    BOOST_CHECK_EQUAL(1, p->writeFieldStop(p));

    vector<uint8_t> expected {T_STOP};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeMapBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeMapBegin(nullptr, T_BYTE, T_BYTE, 42));
    BOOST_CHECK_EQUAL(6, p->writeMapBegin(p, (enum t_type)42, T_BYTE, 42));
    BOOST_CHECK_EQUAL(6, p->writeMapBegin(p, T_BYTE, (enum t_type)42, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(6, p->writeMapBegin(p, T_BYTE, T_BYTE, 0xaabbccdd));

    vector<uint8_t> expected {T_BYTE, T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 6);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeMapEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeMapEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeMapEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeListBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeListBegin(nullptr, T_BYTE, 42));
    BOOST_CHECK_EQUAL(5, p->writeListBegin(p, (enum t_type)42, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(5, p->writeListBegin(p, T_BYTE, 0xaabbccdd));

    vector<uint8_t> expected {T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeListEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeListEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeListEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeSetBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeSetBegin(nullptr, T_BYTE, 42));
    BOOST_CHECK_EQUAL(5, p->writeSetBegin(p, (enum t_type)42, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(5, p->writeSetBegin(p, T_BYTE, 0xaabbccdd));

    vector<uint8_t> expected {T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeSetEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeSetEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->writeMapEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeBool) {
    vector<uint8_t> expected, actual;

    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeBool(nullptr, true));

    // true-ish
    BOOST_CHECK_EQUAL(1, p->writeBool(p, 42));
    // should be corrected to '1'
    expected = vector<uint8_t>{1};
    actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

    // true
    // reset memory buffer
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(1, p->writeBool(p, true));
    expected = vector<uint8_t>{1};
    actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

    // false
    // reset memory buffer
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(1, p->writeBool(p, false));
    expected = vector<uint8_t>{0};
    actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeByte) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeByte(nullptr, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(1, p->writeByte(p, 42));

    vector<uint8_t> expected {42};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeI16) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeI16(nullptr, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(2, p->writeI16(p, 0xaabb));

    vector<uint8_t> expected {0xaa, 0xbb};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 2);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeI32) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeI32(nullptr, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(4, p->writeI32(p, 0xaabbccdd));

    vector<uint8_t> expected {0xaa, 0xbb, 0xcc, 0xdd};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 4);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeI64) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeI64(nullptr, 42));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(8, p->writeI64(p, 0xaabbccddeeff0011));

    vector<uint8_t> expected {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 8);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeDouble) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeDouble(nullptr, M_PI));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(8, p->writeDouble(p, M_PI));

    vector<uint8_t> expected {0x40, 0x09, 0x21, 0xfb, 0x54, 0x44, 0x2d, 0x18};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 8);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_writeString) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->writeString(nullptr, -1, "foo"));
    BOOST_CHECK_EQUAL(4, p->writeString(p, 0, "foo"));

    // reset memory buffer to the beginning to validate contents
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(7, p->writeString(p, 3, "foo"));

    vector<uint8_t> expected {0, 0, 0, 3, 'f', 'o', 'o'};
    vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 7);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

/*
 * R E A D
 */

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readMessageBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    // const char **name, enum t_message_type *message_type, uint32_t *seq
    char name_[64];
    char *name = name_;
    uint32_t len = sizeof(name_);
    enum t_message_type type;
    uint32_t seq;

    BOOST_CHECK_EQUAL(-EINVAL, p->readMessageBegin(nullptr, &len, &name, &type, &seq));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMessageBegin(p, nullptr, &name, &type, &seq));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMessageBegin(p, &len, &name, nullptr, &seq));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMessageBegin(p, &len, &name, &type, nullptr));

    buffer[0] = 0x80;
    buffer[1] = 0x01;
    buffer[2] = 0x00;
    buffer[3] = T_CALL;
    buffer[4] = 0x00;
    buffer[5] = 0x00;
    buffer[6] = 0x00;
    buffer[7] = 3;
    buffer[8] = 'f';
    buffer[9] = 'o';
    buffer[10] = 'o';
    buffer[11] = 0;
    buffer[12] = 0;
    buffer[13] = 0;
    buffer[14] = 42;
    _transport.rBase_ = _transport.buffer_;
    _transport.rBound_ = _transport.rBase_ + 15;
    _transport.wBase_ = _transport.rBound_;
    _transport.wBound_ = _transport.buffer_ + 1024;

    BOOST_CHECK_EQUAL(15, p->readMessageBegin(p, &len, &name, &type, &seq));

    BOOST_CHECK_EQUAL(0, strncmp("foo", name, 3));
    BOOST_CHECK_EQUAL(3, len);
    BOOST_CHECK_EQUAL(T_CALL, type);
    BOOST_CHECK_EQUAL(42, seq);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readMessageEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readMessageEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readMessageEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readStructBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readStructBegin(nullptr, nullptr, nullptr));
    BOOST_CHECK_EQUAL(0, p->readStructBegin(p, nullptr, nullptr));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readStructEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readStructEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readStructEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readFieldBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    char name_[64];
    char *name = name_;
    uint32_t len = sizeof(name_);
    enum t_type field_type;
    uint16_t field_id;

    BOOST_CHECK_EQUAL(-EINVAL, p->readFieldBegin(nullptr, &len, &name, &field_type, &field_id));

    buffer[0] = T_BYTE;
    buffer[1] = 0xaa;
    buffer[2] = 0xbb;
    _transport.rBase_ = _transport.buffer_;
    _transport.rBound_ = _transport.rBase_ + 3;
    _transport.wBase_ = _transport.rBound_;

    BOOST_CHECK_EQUAL(3, p->readFieldBegin(p, &len, &name, &field_type, &field_id));

    BOOST_CHECK_EQUAL(T_BYTE, field_type);
    BOOST_CHECK_EQUAL(0xaabb, field_id);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readFieldEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readFieldEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readFieldEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readMapBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    enum t_type ktype;
    enum t_type vtype;
    uint32_t size;

    BOOST_CHECK_EQUAL(-EINVAL, p->readMapBegin(nullptr, &ktype, &vtype, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMapBegin(p, nullptr, &vtype, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMapBegin(p, &ktype, nullptr, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readMapBegin(p, &ktype, &vtype, nullptr));

    buffer[0] = T_BYTE;
    buffer[1] = T_BYTE;
    buffer[2] = 0xaa;
    buffer[3] = 0xbb;
    buffer[4] = 0xcc;
    buffer[5] = 0xdd;
    _transport.rBase_ = _transport.buffer_;
    _transport.rBound_ = _transport.rBase_ + 6;
    _transport.wBase_ = _transport.rBound_;

    BOOST_CHECK_EQUAL(6, p->readMapBegin(p, &ktype, &vtype, &size));

    BOOST_CHECK_EQUAL(T_BYTE, ktype);
    BOOST_CHECK_EQUAL(T_BYTE, vtype);
    BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readMapEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readMapEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readMapEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readListBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    enum t_type etype;
    uint32_t size;

    BOOST_CHECK_EQUAL(-EINVAL, p->readListBegin(nullptr, &etype, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readListBegin(p, nullptr, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readListBegin(p, &etype, nullptr));

    buffer[0] = T_BYTE;
    buffer[1] = 0xaa;
    buffer[2] = 0xbb;
    buffer[3] = 0xcc;
    buffer[4] = 0xdd;
    _transport.rBase_ = _transport.buffer_;
    _transport.rBound_ = _transport.rBase_ + 5;
    _transport.wBase_ = _transport.rBound_;

    BOOST_CHECK_EQUAL(5, p->readListBegin(p, &etype, &size));

    BOOST_CHECK_EQUAL(T_BYTE, etype);
    BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readListEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readListEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readListEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readSetBegin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    enum t_type etype;
    uint32_t size;

    BOOST_CHECK_EQUAL(-EINVAL, p->readSetBegin(nullptr, &etype, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readSetBegin(p, nullptr, &size));
    BOOST_CHECK_EQUAL(-EINVAL, p->readSetBegin(p, &etype, nullptr));

    buffer[0] = T_BYTE;
    buffer[1] = 0xaa;
    buffer[2] = 0xbb;
    buffer[3] = 0xcc;
    buffer[4] = 0xdd;
    _transport.rBase_ = _transport.buffer_;
    _transport.rBound_ = _transport.rBase_ + 5;
    _transport.wBase_ = _transport.rBound_;

    BOOST_CHECK_EQUAL(5, p->readSetBegin(p, &etype, &size));

    BOOST_CHECK_EQUAL(T_BYTE, etype);
    BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readSetEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    BOOST_CHECK_EQUAL(-EINVAL, p->readSetEnd(nullptr));
    BOOST_CHECK_EQUAL(0, p->readSetEnd(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readBool) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    bool val;
    BOOST_CHECK_EQUAL(-EINVAL, p->readBool(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readBool(p, nullptr));

    val = false;
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    buffer[0] = true;
    _transport.rBound_++;
    _transport.wBase_++;
    BOOST_CHECK_EQUAL(1, p->readBool(p, &val));
    BOOST_CHECK_EQUAL(true, val);

    // read true-ish
    val = false;
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    buffer[0] = 42;
    _transport.rBound_++;
    _transport.wBase_++;
    BOOST_CHECK_EQUAL(1, p->readBool(p, &val));
    BOOST_CHECK_EQUAL(true, val);
    BOOST_CHECK_EQUAL(1, *((int8_t *)&val));

    // read false
    val = true;
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    buffer[0] = false;
    _transport.rBound_++;
    _transport.wBase_++;
    BOOST_CHECK_EQUAL(1, p->readBool(p, &val));
    BOOST_CHECK_EQUAL(false, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readByte) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    uint8_t val;
    BOOST_CHECK_EQUAL(-EINVAL, p->readByte(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readByte(p, nullptr));

    val = 0;
    buffer[0] = 42;
    _transport.rBound_++;
    _transport.wBase_++;
    BOOST_CHECK_EQUAL(1, p->readByte(p, &val));
    BOOST_CHECK_EQUAL(42, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readI16) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    int16_t val;
    BOOST_CHECK_EQUAL(-EINVAL, p->readI16(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readI16(p, nullptr));

    val = 0;
    buffer[0] = 0xaa;
    buffer[1] = 0xbb;
    _transport.rBound_ += 2;
    _transport.wBase_ += 2;
    BOOST_CHECK_EQUAL(2, p->readI16(p, &val));
    BOOST_CHECK_EQUAL(int16_t(0xaabb), val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readI32) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    int32_t val;
    BOOST_CHECK_EQUAL(-EINVAL, p->readI32(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readI32(p, nullptr));

    val = 0;
    buffer[0] = 0xaa;
    buffer[1] = 0xbb;
    buffer[2] = 0xcc;
    buffer[3] = 0xdd;
    _transport.rBound_ += 4;
    _transport.wBase_ += 4;
    BOOST_CHECK_EQUAL(4, p->readI32(p, &val));
    BOOST_CHECK_EQUAL(0xaabbccdd, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readI64) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    int64_t val;
    BOOST_CHECK_EQUAL(-EINVAL, p->readI64(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readI64(p, nullptr));

    val = 0;
    buffer[0] = 0x01;
    buffer[1] = 0x23;
    buffer[2] = 0x45;
    buffer[3] = 0x67;
    buffer[4] = 0x89;
    buffer[5] = 0xab;
    buffer[6] = 0xcd;
    buffer[7] = 0xef;
    _transport.rBound_ += 8;
    _transport.wBase_ += 8;
    BOOST_CHECK_EQUAL(8, p->readI64(p, &val));
    BOOST_CHECK_EQUAL(0x0123456789abcdef, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readDouble) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    double val = 0;
    BOOST_CHECK_EQUAL(-EINVAL, p->readDouble(nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readDouble(p, nullptr));

    val = 0;
    buffer[0] = 0x40;
    buffer[1] = 0x09;
    buffer[2] = 0x21;
    buffer[3] = 0xfb;
    buffer[4] = 0x54;
    buffer[5] = 0x44;
    buffer[6] = 0x2d;
    buffer[7] = 0x18;
    _transport.rBound_ += 8;
    _transport.wBase_ += 8;
    BOOST_CHECK_EQUAL(8, p->readDouble(p, &val));
    BOOST_CHECK_EQUAL(M_PI, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_readString) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

    char val_[64];
    char *val = val_;
    uint32_t len = sizeof(val_);
    BOOST_CHECK_EQUAL(-EINVAL, p->readString(nullptr, &len, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readString(p, nullptr, &val));
    BOOST_CHECK_EQUAL(-EINVAL, p->readString(p, &len, nullptr));

    memset(val_, 0, sizeof(val_));
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 3;
    buffer[4] = 'f';
    buffer[5] = 'o';
    buffer[6] = 'o';
    _transport.rBound_ += 7;
    _transport.wBase_ += 7;
    
    BOOST_CHECK_EQUAL(7, p->readString(p, &len, &val));
    BOOST_CHECK_EQUAL(len, 3);
    BOOST_CHECK_EQUAL(0, strncmp(val, "foo", 3));

    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    memset(val_, 0, sizeof(val_));
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;
    _transport.rBound_ += 4;
    _transport.wBase_ += 4;
    
    val = val_;
    len = sizeof(val_);
    BOOST_CHECK_EQUAL(4, p->readString(p, &len, &val));
    BOOST_CHECK_EQUAL(len, 0);

    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    memset(val_, 0, sizeof(val_));
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 3;
    buffer[4] = 'f';
    buffer[5] = 'o';
    buffer[6] = 'o';
    _transport.rBound_ += 7;
    _transport.wBase_ += 7;
    
    // in the future, we may support dynamic allocation of strings
    // but not at this time.
    val = NULL;
    len = sizeof(val_);
    BOOST_CHECK_EQUAL(-EINVAL, p->readString(p, &len, &val));


    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    memset(val_, 0, sizeof(val_));
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 42;
    _transport.rBound_ += 46;
    _transport.wBase_ += 46;

    // If readString returns -E2BIG it's a bit of a problem.
    // The current Thrift API does not really account for when
    // the string buffer is not large enough to read the string
    // after the size has been read. There should be a 'borrowI32'
    // to support this. Without dynamic allocation, it's also
    // not possible to recover from this error. However, with
    // dynmamic allocation, the buffer could be reallocated to
    // the right size and then read byte by byte.
    //
    // The code generator should get around this by setting
    // aside a string buffer of a maximum size. In fact, that's
    // the C++ API does that as well. That way, if an incoming
    // string exceeds the maximum length, you know immediately
    // that the transport has been corrupted or something malicious
    // is happening.
    val = val_;
    len = 8;
    BOOST_CHECK_EQUAL(-E2BIG, p->readString(p, &len, &val));
    BOOST_CHECK_EQUAL(len, 42);
}
