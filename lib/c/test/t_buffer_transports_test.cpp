#include <cerrno>
#include <vector>

#define BOOST_TEST_MODULE t_memory_buffer
#include <boost/test/unit_test.hpp>

#include "thrift/c/transport/t_transport.h"
#include "thrift/c/transport/t_buffer_transports.h"

using namespace std;

static vector<uint8_t> buffer(1024);
static t_memory_buffer _transport = {};
static t_transport *t = (t_transport *)&_transport;
static const string hi = "Hello, world!";

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_init) {
    BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(nullptr, &buffer.front(), buffer.size()));
    BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(&_transport, nullptr, buffer.size()));
    BOOST_CHECK_EQUAL(-EINVAL, t_memory_buffer_init(&_transport, &buffer.front(), 0));

    BOOST_CHECK_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));
    BOOST_CHECK(_transport.isOpen != nullptr);
    BOOST_CHECK(_transport.peek != nullptr);
    BOOST_CHECK(_transport.open != nullptr);
    BOOST_CHECK(_transport.close != nullptr);
    BOOST_CHECK(_transport.read != nullptr);
    BOOST_CHECK(_transport.readAll != nullptr);
    BOOST_CHECK(_transport.write != nullptr);
    BOOST_CHECK(_transport.writeAll != nullptr);
    BOOST_CHECK(_transport.flush != nullptr);
    BOOST_CHECK(_transport.readEnd != nullptr);
    BOOST_CHECK(_transport.writeEnd != nullptr);
    BOOST_CHECK(_transport.borrow != nullptr);
    BOOST_CHECK(_transport.consume != nullptr);
    BOOST_CHECK(_transport.getOrigin != nullptr);
    BOOST_CHECK(_transport.available_read != nullptr);
    BOOST_CHECK(_transport.available_write != nullptr);

    BOOST_CHECK_EQUAL(_transport.open_, false);
    BOOST_CHECK_EQUAL(_transport.buffer_, &buffer[0]);
    BOOST_CHECK_EQUAL(_transport.bufferSize_, buffer.size());
    BOOST_CHECK_EQUAL(_transport.rBase_, &buffer[0]);
    BOOST_CHECK_EQUAL(_transport.rBound_, &buffer[0]);
    BOOST_CHECK_EQUAL(_transport.wBase_, &buffer[0]);
    BOOST_CHECK_EQUAL(_transport.wBound_, &buffer[buffer.size()]);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_isOpen) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(false, t->peek(nullptr));

    BOOST_CHECK_EQUAL(false, t->peek(nullptr));
    _transport.rBound_ = _transport.rBase_ + 1;
    _transport.wBase_ = _transport.rBound_;
    BOOST_CHECK_EQUAL(true, t->peek(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_peek) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(false, t->isOpen(nullptr));

    BOOST_REQUIRE_EQUAL(false, _transport.open_);
    BOOST_CHECK_EQUAL(false, t->isOpen(t));

    _transport.open_ = true;
    BOOST_CHECK_EQUAL(true, t->isOpen(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_open) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->open(nullptr));

    BOOST_REQUIRE_EQUAL(false, _transport.open_);
    BOOST_CHECK_EQUAL(0, t->open(t));
    BOOST_CHECK_EQUAL(true, _transport.open_);

    BOOST_CHECK_EQUAL(-EALREADY, t->open(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_close) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->close(nullptr));

    BOOST_REQUIRE_EQUAL(false, _transport.open_);
    BOOST_CHECK_EQUAL(-EALREADY, t->close(t));

    _transport.open_ = true;
    BOOST_CHECK_EQUAL(0, t->close(t));
    BOOST_CHECK_EQUAL(false, _transport.open_);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_read) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    vector<char> buf(1024, '\0');

    BOOST_CHECK_EQUAL(-EINVAL, t->read(nullptr, &buf.front(), hi.size() + 1));
    BOOST_CHECK_EQUAL(-EINVAL, t->read(t, nullptr, hi.size() + 1));
    BOOST_CHECK_EQUAL(0, t->read(t, &buf.front(), 0));

    strcpy((char *)_transport.buffer_, hi.c_str());
    _transport.rBound_ = _transport.buffer_ + hi.size();
    BOOST_CHECK_EQUAL(hi.size(), t->read(t, &buf.front(), buf.size()));
    BOOST_CHECK_EQUAL(_transport.rBase_, _transport.buffer_ + hi.size());

    strncpy((char *)_transport.buffer_ + _transport.bufferSize_ - 3, hi.c_str(), 3);
    _transport.rBound_ = _transport.buffer_ + _transport.bufferSize_;
    _transport.rBase_ = _transport.rBound_ - 3;
    BOOST_CHECK_EQUAL(3, t->read(t, &buf.front(), buf.size()));
    BOOST_CHECK_EQUAL(string(&buf[0], &buf[3]), hi.substr(0, 3));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_write) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->write(nullptr, hi.c_str(), hi.size() + 1));
    BOOST_CHECK_EQUAL(-EINVAL, t->write(t, nullptr, hi.size() + 1));
    BOOST_CHECK_EQUAL(0, t->write(t, hi.c_str(), 0));

    BOOST_CHECK_EQUAL(hi.size() + 1, t->write(t, hi.c_str(), hi.size() + 1));
    BOOST_CHECK_EQUAL(_transport.buffer_, _transport.rBase_);
    BOOST_CHECK_EQUAL(_transport.buffer_ + hi.size() + 1, _transport.rBound_);
    BOOST_CHECK_EQUAL(_transport.buffer_ + hi.size() + 1, _transport.wBase_);
    BOOST_CHECK_EQUAL(_transport.buffer_ + _transport.bufferSize_, _transport.wBound_);
    BOOST_CHECK_EQUAL(string(_transport.buffer_, _transport.buffer_ + hi.size()), hi);

    _transport.wBase_ = _transport.buffer_ + _transport.bufferSize_ - 3;
    _transport.wBound_ = _transport.buffer_ + _transport.bufferSize_;
    BOOST_CHECK_EQUAL(3, t->write(t, hi.c_str(), hi.size() + 1));
    BOOST_CHECK_EQUAL(string(_transport.buffer_ + _transport.bufferSize_ - 3, _transport.buffer_ + _transport.bufferSize_), hi.substr(0, 3));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_flush) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->flush(nullptr));
    BOOST_CHECK_EQUAL(0, t->flush(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_readEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->readEnd(nullptr));
    BOOST_CHECK_EQUAL(0, t->readEnd(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_writeEnd) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->writeEnd(nullptr));
    BOOST_CHECK_EQUAL(0, t->writeEnd(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_borrow) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    uint8_t buf_[32];
    void *buf = buf_;
    uint32_t len = sizeof(buf_);
    BOOST_CHECK_EQUAL(-EINVAL, t->borrow(nullptr, &buf, &len));
    BOOST_CHECK_EQUAL(-EINVAL, t->borrow(t, NULL, &len));
    BOOST_CHECK_EQUAL(-EINVAL, t->borrow(t, &buf, NULL));
    BOOST_CHECK_EQUAL(0, t->borrow(t, &buf, &len));
    BOOST_CHECK_EQUAL(0, len);

    _transport.rBound_ = _transport.rBase_ + 6;
    _transport.wBase_ = _transport.rBase_ + 6;
    _transport.rBase_[0] = 0xaa;
    _transport.rBase_[1] = 0xbb;
    _transport.rBase_[2] = 0xcc;
    _transport.rBase_[3] = 0xdd;
    _transport.rBase_[4] = 0xee;
    _transport.rBase_[5] = 0xff;
    
    memset(buf_, 0, sizeof(buf_));
    buf = buf_;
    len = sizeof(buf_);
    BOOST_CHECK_EQUAL(6, t->borrow(t, &buf, &len));
    BOOST_CHECK_EQUAL(6, len);
    BOOST_CHECK(buf == buf_);
    BOOST_CHECK_EQUAL(0, memcmp(buf, _transport.rBase_, 6));

    memset(buf_, 0, sizeof(buf_));
    buf = NULL;
    len = sizeof(buf_);
    BOOST_CHECK_EQUAL(6, t->borrow(t, &buf, &len));
    BOOST_CHECK_EQUAL(6, len);
    BOOST_CHECK(buf == _transport.rBase_);
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_consume) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(-EINVAL, t->consume(nullptr, 5000));

    _transport.rBound_ = _transport.rBase_ + 6;
    _transport.wBase_ = _transport.rBound_;
    _transport.rBase_[0] = 0xaa;
    _transport.rBase_[1] = 0xbb;
    _transport.rBase_[2] = 0xcc;
    _transport.rBase_[3] = 0xdd;
    _transport.rBase_[4] = 0xee;
    _transport.rBase_[5] = 0xff;

    BOOST_CHECK_EQUAL(0, t->consume(t, 6));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_getOrigin) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_CHECK_EQUAL(nullptr, t->getOrigin(nullptr));
    BOOST_CHECK_EQUAL("memory", t->getOrigin(t));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_available_read) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_REQUIRE_EQUAL(0, _transport.available_read(&_transport));

    _transport.rBound_ += 5;
    BOOST_REQUIRE_EQUAL(5, _transport.available_read(&_transport));

    _transport.rBase_ += 5;
    BOOST_REQUIRE_EQUAL(0, _transport.available_read(&_transport));
}

BOOST_AUTO_TEST_CASE(test_t_memory_buffer_available_write) {
    BOOST_REQUIRE_EQUAL(0, t_memory_buffer_init(&_transport, &buffer.front(), buffer.size()));

    BOOST_REQUIRE_EQUAL(buffer.size(), _transport.available_write(&_transport));

    _transport.wBase_ += 5;
    BOOST_REQUIRE_EQUAL(buffer.size() - 5, _transport.available_write(&_transport));

    _transport.wBase_ = _transport.buffer_ + _transport.bufferSize_;
    BOOST_REQUIRE_EQUAL(0, _transport.available_write(&_transport));
}
