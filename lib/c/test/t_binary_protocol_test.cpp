#include <cerrno>
#include <iomanip>
#include <sstream>
#include <vector>

#define BOOST_TEST_MODULE t_binary_protocol
#include <boost/test/unit_test.hpp>

#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/transport/t_buffer_transports.h"

using namespace std;

static vector<uint8_t> buffer(1024);
static t_memory_buffer memory_buffer = {};
static t_transport* t = (t_transport*)&memory_buffer;
static t_binary_protocol _protocol = {};
static t_protocol* p = (t_protocol*)&_protocol;
static const string hi = "Hello, world!";

string to_string(const vector<uint8_t>& v) {
  stringstream ss;

  for (size_t i = 0, n = v.size(); i < n; ++i) {
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

ostream& operator<<(ostream& os, const vector<uint8_t>& v) {
  return os << v;
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_init) {
  BOOST_CHECK_EQUAL(-EINVAL, t_binary_protocol_init(nullptr, t, &t_network_big_endian));
  BOOST_CHECK_EQUAL(-EINVAL, t_binary_protocol_init(&_protocol, nullptr, &t_network_big_endian));
  BOOST_CHECK_EQUAL(0, t_binary_protocol_init(&_protocol, t, &t_network_little_endian));
  BOOST_CHECK_EQUAL(_protocol.byte_order, &t_network_little_endian);

  BOOST_CHECK_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));
  BOOST_CHECK_EQUAL(true, t_protocol_is_valid((struct t_protocol*)&_protocol));
  BOOST_CHECK_EQUAL(_protocol.trans, t);
  BOOST_CHECK_EQUAL(_protocol.byte_order, &t_network_big_endian);
}

/*
 * W R I T E
 */

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_message_begin) {
  fill(buffer.begin(), buffer.end(), 0);
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_message_begin(nullptr, "foo", T_CALL, 42));
  BOOST_CHECK_EQUAL(-EINVAL, p->write_message_begin(p, nullptr, T_CALL, 42));
  BOOST_CHECK_EQUAL(-EINVAL, p->write_message_begin(p, "foo", (enum t_message_type)42, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(15, p->write_message_begin(p, "foo", T_CALL, 42));

  vector<uint8_t> expected{0x80, 0x01, 0x00, T_CALL, 0x00, 0x00, 0x00, 3,
                           'f',  'o',  'o',  0,      0,    0,    42};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 15);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_message_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_message_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_message_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_struct_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_struct_begin(nullptr, "foo"));
  BOOST_CHECK_EQUAL(0, p->write_struct_begin(p, nullptr));
  BOOST_CHECK_EQUAL(0, p->write_struct_begin(p, "foo"));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_struct_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_struct_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_struct_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_field_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_field_begin(nullptr, "foo", T_BYTE, 42));
  BOOST_CHECK_EQUAL(3, p->write_field_begin(p, nullptr, T_BYTE, 42));
  BOOST_CHECK_EQUAL(3, p->write_field_begin(p, "foo", (enum t_type)42, 0xffffffff));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(3, p->write_field_begin(p, "foo", T_BYTE, 0xaabb));

  vector<uint8_t> expected{T_BYTE, 0xaa, 0xbb};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_field_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_field_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_field_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_field_stop) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_field_stop(nullptr));
  BOOST_CHECK_EQUAL(1, p->write_field_stop(p));

  vector<uint8_t> expected{T_STOP};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_map_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_map_begin(nullptr, T_BYTE, T_BYTE, 42));
  BOOST_CHECK_EQUAL(6, p->write_map_begin(p, (enum t_type)42, T_BYTE, 42));
  BOOST_CHECK_EQUAL(6, p->write_map_begin(p, T_BYTE, (enum t_type)42, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(6, p->write_map_begin(p, T_BYTE, T_BYTE, 0xaabbccdd));

  vector<uint8_t> expected{T_BYTE, T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 6);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_map_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_map_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_map_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_list_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_list_begin(nullptr, T_BYTE, 42));
  BOOST_CHECK_EQUAL(5, p->write_list_begin(p, (enum t_type)42, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(5, p->write_list_begin(p, T_BYTE, 0xaabbccdd));

  vector<uint8_t> expected{T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 5);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_list_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_list_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_list_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_set_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_set_begin(nullptr, T_BYTE, 42));
  BOOST_CHECK_EQUAL(5, p->write_set_begin(p, (enum t_type)42, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(5, p->write_set_begin(p, T_BYTE, 0xaabbccdd));

  vector<uint8_t> expected{T_BYTE, 0xaa, 0xbb, 0xcc, 0xdd};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 5);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_set_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_set_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->write_map_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_bool) {
  vector<uint8_t> expected, actual;

  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_bool(nullptr, true));

  // true-ish
  BOOST_CHECK_EQUAL(1, p->write_bool(p, 42));
  // should be corrected to '1'
  expected = vector<uint8_t>{1};
  actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

  // true
  // reset memory buffer
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(1, p->write_bool(p, true));
  expected = vector<uint8_t>{1};
  actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

  // false
  // reset memory buffer
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(1, p->write_bool(p, false));
  expected = vector<uint8_t>{0};
  actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_byte) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_byte(nullptr, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(1, p->write_byte(p, 42));

  vector<uint8_t> expected{42};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_i16) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_i16(nullptr, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(2, p->write_i16(p, 0xaabb));

  vector<uint8_t> expected{0xaa, 0xbb};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 2);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_i32) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_i32(nullptr, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(4, p->write_i32(p, 0xaabbccdd));

  vector<uint8_t> expected{0xaa, 0xbb, 0xcc, 0xdd};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 4);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_i64) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_i64(nullptr, 42));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(8, p->write_i64(p, 0xaabbccddeeff0011));

  vector<uint8_t> expected{0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 8);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_double) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_double(nullptr, M_PI));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(8, p->write_double(p, M_PI));

  vector<uint8_t> expected{0x40, 0x09, 0x21, 0xfb, 0x54, 0x44, 0x2d, 0x18};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 8);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_write_string) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->write_string(nullptr, -1, "foo"));
  BOOST_CHECK_EQUAL(4, p->write_string(p, 0, "foo"));

  // reset memory buffer to the beginning to validate contents
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(7, p->write_string(p, 3, "foo"));

  vector<uint8_t> expected{0, 0, 0, 3, 'f', 'o', 'o'};
  vector<uint8_t> actual = vector<uint8_t>(buffer.begin(), buffer.begin() + 7);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

/*
 * R E A D
 */

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_message_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  // const char **name, enum t_message_type *message_type, uint32_t *seq
  char name_[64];
  char* name = name_;
  uint32_t len = sizeof(name_);
  enum t_message_type type;
  uint32_t seq;

  BOOST_CHECK_EQUAL(-EINVAL, p->read_message_begin(nullptr, &len, &name, &type, &seq));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_message_begin(p, nullptr, &name, &type, &seq));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_message_begin(p, &len, &name, nullptr, &seq));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_message_begin(p, &len, &name, &type, nullptr));

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
  memory_buffer.rBase_ = memory_buffer.buffer_;
  memory_buffer.rBound_ = memory_buffer.rBase_ + 15;
  memory_buffer.wBase_ = memory_buffer.rBound_;
  memory_buffer.wBound_ = memory_buffer.buffer_ + 1024;

  BOOST_CHECK_EQUAL(15, p->read_message_begin(p, &len, &name, &type, &seq));

  BOOST_CHECK_EQUAL(0, strncmp("foo", name, 3));
  BOOST_CHECK_EQUAL(3, len);
  BOOST_CHECK_EQUAL(T_CALL, type);
  BOOST_CHECK_EQUAL(42, seq);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_message_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_message_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_message_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_struct_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_struct_begin(nullptr, nullptr, nullptr));
  BOOST_CHECK_EQUAL(0, p->read_struct_begin(p, nullptr, nullptr));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_struct_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_struct_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_struct_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_field_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  char name_[64];
  char* name = name_;
  uint32_t len = sizeof(name_);
  enum t_type field_type;
  uint16_t field_id;

  BOOST_CHECK_EQUAL(-EINVAL, p->read_field_begin(nullptr, &len, &name, &field_type, &field_id));

  buffer[0] = T_BYTE;
  buffer[1] = 0xaa;
  buffer[2] = 0xbb;
  memory_buffer.rBase_ = memory_buffer.buffer_;
  memory_buffer.rBound_ = memory_buffer.rBase_ + 3;
  memory_buffer.wBase_ = memory_buffer.rBound_;

  BOOST_CHECK_EQUAL(3, p->read_field_begin(p, &len, &name, &field_type, &field_id));

  BOOST_CHECK_EQUAL(T_BYTE, field_type);
  BOOST_CHECK_EQUAL(0xaabb, field_id);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_field_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_field_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_field_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_map_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  enum t_type ktype;
  enum t_type vtype;
  uint32_t size;

  BOOST_CHECK_EQUAL(-EINVAL, p->read_map_begin(nullptr, &ktype, &vtype, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_map_begin(p, nullptr, &vtype, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_map_begin(p, &ktype, nullptr, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_map_begin(p, &ktype, &vtype, nullptr));

  buffer[0] = T_BYTE;
  buffer[1] = T_BYTE;
  buffer[2] = 0xaa;
  buffer[3] = 0xbb;
  buffer[4] = 0xcc;
  buffer[5] = 0xdd;
  memory_buffer.rBase_ = memory_buffer.buffer_;
  memory_buffer.rBound_ = memory_buffer.rBase_ + 6;
  memory_buffer.wBase_ = memory_buffer.rBound_;

  BOOST_CHECK_EQUAL(6, p->read_map_begin(p, &ktype, &vtype, &size));

  BOOST_CHECK_EQUAL(T_BYTE, ktype);
  BOOST_CHECK_EQUAL(T_BYTE, vtype);
  BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_map_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_map_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_map_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_list_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  enum t_type etype;
  uint32_t size;

  BOOST_CHECK_EQUAL(-EINVAL, p->read_list_begin(nullptr, &etype, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_list_begin(p, nullptr, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_list_begin(p, &etype, nullptr));

  buffer[0] = T_BYTE;
  buffer[1] = 0xaa;
  buffer[2] = 0xbb;
  buffer[3] = 0xcc;
  buffer[4] = 0xdd;
  memory_buffer.rBase_ = memory_buffer.buffer_;
  memory_buffer.rBound_ = memory_buffer.rBase_ + 5;
  memory_buffer.wBase_ = memory_buffer.rBound_;

  BOOST_CHECK_EQUAL(5, p->read_list_begin(p, &etype, &size));

  BOOST_CHECK_EQUAL(T_BYTE, etype);
  BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_list_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_list_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_list_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_set_begin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  enum t_type etype;
  uint32_t size;

  BOOST_CHECK_EQUAL(-EINVAL, p->read_set_begin(nullptr, &etype, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_set_begin(p, nullptr, &size));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_set_begin(p, &etype, nullptr));

  buffer[0] = T_BYTE;
  buffer[1] = 0xaa;
  buffer[2] = 0xbb;
  buffer[3] = 0xcc;
  buffer[4] = 0xdd;
  memory_buffer.rBase_ = memory_buffer.buffer_;
  memory_buffer.rBound_ = memory_buffer.rBase_ + 5;
  memory_buffer.wBase_ = memory_buffer.rBound_;

  BOOST_CHECK_EQUAL(5, p->read_set_begin(p, &etype, &size));

  BOOST_CHECK_EQUAL(T_BYTE, etype);
  BOOST_CHECK_EQUAL(0xaabbccdd, size);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_set_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  BOOST_CHECK_EQUAL(-EINVAL, p->read_set_end(nullptr));
  BOOST_CHECK_EQUAL(0, p->read_set_end(p));
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_bool) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  bool val;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_bool(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_bool(p, nullptr));

  val = false;
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  buffer[0] = true;
  memory_buffer.rBound_++;
  memory_buffer.wBase_++;
  BOOST_CHECK_EQUAL(1, p->read_bool(p, &val));
  BOOST_CHECK_EQUAL(true, val);

  // read true-ish
  val = false;
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  buffer[0] = 42;
  memory_buffer.rBound_++;
  memory_buffer.wBase_++;
  BOOST_CHECK_EQUAL(1, p->read_bool(p, &val));
  BOOST_CHECK_EQUAL(true, val);
  BOOST_CHECK_EQUAL(1, *((int8_t*)&val));

  // read false
  val = true;
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  buffer[0] = false;
  memory_buffer.rBound_++;
  memory_buffer.wBase_++;
  BOOST_CHECK_EQUAL(1, p->read_bool(p, &val));
  BOOST_CHECK_EQUAL(false, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_byte) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  uint8_t val;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_byte(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_byte(p, nullptr));

  val = 0;
  buffer[0] = 42;
  memory_buffer.rBound_++;
  memory_buffer.wBase_++;
  BOOST_CHECK_EQUAL(1, p->read_byte(p, &val));
  BOOST_CHECK_EQUAL(42, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_i16) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  int16_t val;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i16(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i16(p, nullptr));

  val = 0;
  buffer[0] = 0xaa;
  buffer[1] = 0xbb;
  memory_buffer.rBound_ += 2;
  memory_buffer.wBase_ += 2;
  BOOST_CHECK_EQUAL(2, p->read_i16(p, &val));
  BOOST_CHECK_EQUAL(int16_t(0xaabb), val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_i32) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  int32_t val;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i32(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i32(p, nullptr));

  val = 0;
  buffer[0] = 0xaa;
  buffer[1] = 0xbb;
  buffer[2] = 0xcc;
  buffer[3] = 0xdd;
  memory_buffer.rBound_ += 4;
  memory_buffer.wBase_ += 4;
  BOOST_CHECK_EQUAL(4, p->read_i32(p, &val));
  BOOST_CHECK_EQUAL(0xaabbccdd, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_i64) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  int64_t val;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i64(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_i64(p, nullptr));

  val = 0;
  buffer[0] = 0x01;
  buffer[1] = 0x23;
  buffer[2] = 0x45;
  buffer[3] = 0x67;
  buffer[4] = 0x89;
  buffer[5] = 0xab;
  buffer[6] = 0xcd;
  buffer[7] = 0xef;
  memory_buffer.rBound_ += 8;
  memory_buffer.wBase_ += 8;
  BOOST_CHECK_EQUAL(8, p->read_i64(p, &val));
  BOOST_CHECK_EQUAL(0x0123456789abcdef, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_double) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  double val = 0;
  BOOST_CHECK_EQUAL(-EINVAL, p->read_double(nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_double(p, nullptr));

  val = 0;
  buffer[0] = 0x40;
  buffer[1] = 0x09;
  buffer[2] = 0x21;
  buffer[3] = 0xfb;
  buffer[4] = 0x54;
  buffer[5] = 0x44;
  buffer[6] = 0x2d;
  buffer[7] = 0x18;
  memory_buffer.rBound_ += 8;
  memory_buffer.wBase_ += 8;
  BOOST_CHECK_EQUAL(8, p->read_double(p, &val));
  BOOST_CHECK_EQUAL(M_PI, val);
}

BOOST_AUTO_TEST_CASE(test_t_binary_protocol_read_string) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_REQUIRE_EQUAL(0, t_binary_protocol_init(&_protocol, t, nullptr));

  char val_[64];
  char* val = val_;
  uint32_t len = sizeof(val_);
  BOOST_CHECK_EQUAL(-EINVAL, p->read_string(nullptr, &len, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_string(p, nullptr, &val));
  BOOST_CHECK_EQUAL(-EINVAL, p->read_string(p, &len, nullptr));

  memset(val_, 0, sizeof(val_));
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 3;
  buffer[4] = 'f';
  buffer[5] = 'o';
  buffer[6] = 'o';
  memory_buffer.rBound_ += 7;
  memory_buffer.wBase_ += 7;

  BOOST_CHECK_EQUAL(7, p->read_string(p, &len, &val));
  BOOST_CHECK_EQUAL(len, 3);
  BOOST_CHECK_EQUAL(0, strncmp(val, "foo", 3));

  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  memset(val_, 0, sizeof(val_));
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  memory_buffer.rBound_ += 4;
  memory_buffer.wBase_ += 4;

  val = val_;
  len = sizeof(val_);
  BOOST_CHECK_EQUAL(4, p->read_string(p, &len, &val));
  BOOST_CHECK_EQUAL(len, 0);

  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  memset(val_, 0, sizeof(val_));
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 3;
  buffer[4] = 'f';
  buffer[5] = 'o';
  buffer[6] = 'o';
  memory_buffer.rBound_ += 7;
  memory_buffer.wBase_ += 7;

  // in the future, we may support dynamic allocation of strings
  // but not at this time.
  val = NULL;
  len = sizeof(val_);
  BOOST_CHECK_EQUAL(-EINVAL, p->read_string(p, &len, &val));

  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  memset(val_, 0, sizeof(val_));
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 42;
  memory_buffer.rBound_ += 46;
  memory_buffer.wBase_ += 46;

  // _if read_string returns -E2_b_iG it's a bit of a problem.
  // The current Thrift AP_i does not really account for when
  // the string buffer is not large enough to read the string
  // after the size has been read. There should be a 'borrow_i32'
  // to support this. Without dynamic allocation, it's also
  // not possible to recover from this error. However, with
  // dynmamic allocation, the buffer could be reallocated to
  // the right size and then read byte by byte.
  //
  // The code generator should get around this by setting
  // aside a string buffer of a maximum size. _in fact, that's
  // the C++ AP_i does that as well. That way, if an incoming
  // string exceeds the maximum length, you know immediately
  // that the transport has been corrupted or something malicious
  // is happening.
  val = val_;
  len = 8;
  BOOST_CHECK_EQUAL(-E2BIG, p->read_string(p, &len, &val));
  BOOST_CHECK_EQUAL(len, 42);
}
