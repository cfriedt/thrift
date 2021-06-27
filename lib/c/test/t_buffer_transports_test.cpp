#include <cerrno>
#include <vector>

#define BOOST_TEST_MODULE t_memory_buffer
#include <boost/test/unit_test.hpp>

#include "thrift/c/transport/t_buffer_transports.h"
#include "thrift/c/transport/t_transport.h"

using namespace std;

static vector<uint8_t> buffer(1024);
static t_memory_buffer memory_buffer = {};
static t_transport* t = (t_transport*)&memory_buffer;
static const string hi = "Hello, world!";

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_init) {
  BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(nullptr, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(&memory_buffer, nullptr, buffer.size()));
  BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(&memory_buffer, &buffer.front(), 0));

  BOOST_CHECK_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));
  BOOST_CHECK_EQUAL(true, t_transport_is_valid(t));

  BOOST_CHECK_EQUAL(memory_buffer.open_, false);
  BOOST_CHECK_EQUAL(memory_buffer.buffer_, &buffer[0]);
  BOOST_CHECK_EQUAL(memory_buffer.bufferSize_, buffer.size());
  BOOST_CHECK_EQUAL(memory_buffer.rBase_, &buffer[0]);
  BOOST_CHECK_EQUAL(memory_buffer.rBound_, &buffer[0]);
  BOOST_CHECK_EQUAL(memory_buffer.wBase_, &buffer[0]);
  BOOST_CHECK_EQUAL(memory_buffer.wBound_, &buffer[buffer.size()]);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_is_open) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(false, t->peek(nullptr));

  BOOST_CHECK_EQUAL(false, t->peek(nullptr));
  memory_buffer.rBound_ = memory_buffer.rBase_ + 1;
  memory_buffer.wBase_ = memory_buffer.rBound_;
  BOOST_CHECK_EQUAL(true, t->peek(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_peek) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(false, t->is_open(nullptr));

  BOOST_REQUIRE_EQUAL(false, memory_buffer.open_);
  BOOST_CHECK_EQUAL(false, t->is_open(t));

  memory_buffer.open_ = true;
  BOOST_CHECK_EQUAL(true, t->is_open(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_open) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->open(nullptr));

  BOOST_REQUIRE_EQUAL(false, memory_buffer.open_);
  BOOST_CHECK_EQUAL(0, t->open(t));
  BOOST_CHECK_EQUAL(true, memory_buffer.open_);

  BOOST_CHECK_EQUAL(-EALREADY, t->open(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_close) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->close(nullptr));

  BOOST_REQUIRE_EQUAL(false, memory_buffer.open_);
  BOOST_CHECK_EQUAL(-EALREADY, t->close(t));

  memory_buffer.open_ = true;
  BOOST_CHECK_EQUAL(0, t->close(t));
  BOOST_CHECK_EQUAL(false, memory_buffer.open_);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_read) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  vector<char> buf(1024, '\0');

  BOOST_CHECK_EQUAL(-EINVAL, t->read(nullptr, &buf.front(), hi.size() + 1));
  BOOST_CHECK_EQUAL(-EINVAL, t->read(t, nullptr, hi.size() + 1));
  BOOST_CHECK_EQUAL(0, t->read(t, &buf.front(), 0));

  strcpy((char*)memory_buffer.buffer_, hi.c_str());
  memory_buffer.rBound_ = memory_buffer.buffer_ + hi.size();
  BOOST_CHECK_EQUAL(hi.size(), t->read(t, &buf.front(), buf.size()));
  BOOST_CHECK_EQUAL(memory_buffer.rBase_, memory_buffer.buffer_ + hi.size());

  strncpy((char*)memory_buffer.buffer_ + memory_buffer.bufferSize_ - 3, hi.c_str(), 3);
  memory_buffer.rBound_ = memory_buffer.buffer_ + memory_buffer.bufferSize_;
  memory_buffer.rBase_ = memory_buffer.rBound_ - 3;
  BOOST_CHECK_EQUAL(3, t->read(t, &buf.front(), buf.size()));
  BOOST_CHECK_EQUAL(string(&buf[0], &buf[3]), hi.substr(0, 3));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_write) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->write(nullptr, hi.c_str(), hi.size() + 1));
  BOOST_CHECK_EQUAL(-EINVAL, t->write(t, nullptr, hi.size() + 1));
  BOOST_CHECK_EQUAL(0, t->write(t, hi.c_str(), 0));

  BOOST_CHECK_EQUAL(hi.size() + 1, t->write(t, hi.c_str(), hi.size() + 1));
  BOOST_CHECK_EQUAL(memory_buffer.buffer_, memory_buffer.rBase_);
  BOOST_CHECK_EQUAL(memory_buffer.buffer_ + hi.size() + 1, memory_buffer.rBound_);
  BOOST_CHECK_EQUAL(memory_buffer.buffer_ + hi.size() + 1, memory_buffer.wBase_);
  BOOST_CHECK_EQUAL(memory_buffer.buffer_ + memory_buffer.bufferSize_, memory_buffer.wBound_);
  BOOST_CHECK_EQUAL(string(memory_buffer.buffer_, memory_buffer.buffer_ + hi.size()), hi);

  memory_buffer.wBase_ = memory_buffer.buffer_ + memory_buffer.bufferSize_ - 3;
  memory_buffer.wBound_ = memory_buffer.buffer_ + memory_buffer.bufferSize_;
  BOOST_CHECK_EQUAL(3, t->write(t, hi.c_str(), hi.size() + 1));
  BOOST_CHECK_EQUAL(string(memory_buffer.buffer_ + memory_buffer.bufferSize_ - 3,
                           memory_buffer.buffer_ + memory_buffer.bufferSize_),
                    hi.substr(0, 3));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_flush) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->flush(nullptr));
  BOOST_CHECK_EQUAL(0, t->flush(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_read_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->read_end(nullptr));
  BOOST_CHECK_EQUAL(0, t->read_end(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_write_end) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->write_end(nullptr));
  BOOST_CHECK_EQUAL(0, t->write_end(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_borrow) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  uint8_t buf_[32];
  uint8_t* buf = buf_;
  uint32_t len = sizeof(buf_);
  BOOST_CHECK_EQUAL(-EINVAL, t->borrow(nullptr, &buf, &len));
  BOOST_CHECK_EQUAL(-EINVAL, t->borrow(t, NULL, &len));
  BOOST_CHECK_EQUAL(-EINVAL, t->borrow(t, &buf, NULL));
  BOOST_CHECK_EQUAL(0, t->borrow(t, &buf, &len));
  BOOST_CHECK_EQUAL(0, len);

  memory_buffer.rBound_ = memory_buffer.rBase_ + 6;
  memory_buffer.wBase_ = memory_buffer.rBase_ + 6;
  memory_buffer.rBase_[0] = 0xaa;
  memory_buffer.rBase_[1] = 0xbb;
  memory_buffer.rBase_[2] = 0xcc;
  memory_buffer.rBase_[3] = 0xdd;
  memory_buffer.rBase_[4] = 0xee;
  memory_buffer.rBase_[5] = 0xff;

  memset(buf_, 0, sizeof(buf_));
  buf = buf_;
  len = sizeof(buf_);
  BOOST_CHECK_EQUAL(6, t->borrow(t, &buf, &len));
  BOOST_CHECK_EQUAL(6, len);
  BOOST_CHECK(buf == buf_);
  BOOST_CHECK_EQUAL(0, memcmp(buf, memory_buffer.rBase_, 6));

  memset(buf_, 0, sizeof(buf_));
  buf = NULL;
  len = sizeof(buf_);
  BOOST_CHECK_EQUAL(6, t->borrow(t, &buf, &len));
  BOOST_CHECK_EQUAL(6, len);
  BOOST_CHECK(buf == memory_buffer.rBase_);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_consume) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(-EINVAL, t->consume(nullptr, 5000));

  memory_buffer.rBound_ = memory_buffer.rBase_ + 6;
  memory_buffer.wBase_ = memory_buffer.rBound_;
  memory_buffer.rBase_[0] = 0xaa;
  memory_buffer.rBase_[1] = 0xbb;
  memory_buffer.rBase_[2] = 0xcc;
  memory_buffer.rBase_[3] = 0xdd;
  memory_buffer.rBase_[4] = 0xee;
  memory_buffer.rBase_[5] = 0xff;

  BOOST_CHECK_EQUAL(0, t->consume(t, 6));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_getOrigin) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_CHECK_EQUAL(nullptr, t->get_origin(nullptr));
  BOOST_CHECK_EQUAL("memory", t->get_origin(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_available_read) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_REQUIRE_EQUAL(0, t->available_read(t));

  memory_buffer.rBound_ += 5;
  BOOST_REQUIRE_EQUAL(5, t->available_read(t));

  memory_buffer.rBase_ += 5;
  BOOST_REQUIRE_EQUAL(0, t->available_read(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_available_write) {
  BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&memory_buffer, &buffer.front(), buffer.size()));

  BOOST_REQUIRE_EQUAL(buffer.size(), t->available_write(t));

  memory_buffer.wBase_ += 5;
  BOOST_REQUIRE_EQUAL(buffer.size() - 5, t->available_write(t));

  memory_buffer.wBase_ = memory_buffer.buffer_ + memory_buffer.bufferSize_;
  BOOST_REQUIRE_EQUAL(0, t->available_write(t));
}
