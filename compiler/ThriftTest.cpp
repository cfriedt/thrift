#include <array>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TSocket.h"

#include "thrift/c/processor/t_processor.h"
#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/server/t_simple_server.h"
#include "thrift/c/thrift.h"
#include "thrift/c/transport/t_buffer_transports.h"
#include "thrift/c/transport/t_server_socket.h"
#include "thrift/c/transport/t_simple_transport_factory.h"
#include "thrift/c/transport/t_socket.h"

#include "SecondService.h"
#include "ThriftTest.h"

#define container_of(p, t, f) ((t*)((uint8_t*)p - offsetof(t, f)))

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace thrift::test;

extern "C" {
// TODO: create a ThriftTest 'handler' for the 'processor' (normally auto-generated)
struct thrift_test_handler {
  void (*testVoid)(void);
  string testString(string thing);
  bool (*testBool)(bool thing);
  int8_t (*testByte)(int8_t thing);
  int32_t (*testI32)(int32_t thing);
  int64_t (*testI64)(int64_t thing);
  double (*testDouble)(double thing);
  // binary testBinary(binary thing);
  Xtruct (*testStruct)(Xtruct thing);
  Xtruct2 (*testNest)(Xtruct2 thing);
  // map<i32,i32> testMap(1: map<i32,i32> thing);
  // map<string,string> testStringMap(1: map<string,string> thing);
  // set<i32> testSet(1 : set<i32> thing);
  // list<i32> testList(1: list<i32> thing);
  // Numberz testEnum(1 : Numberz thing);
  // map<i32,map<i32,i32>> testMapMap(1: i32 hello),
  // map<UserId, map<Numberz,Insanity>> testInsanity(1: Insanity argument)
  // void testException(1: string arg) throws(1: Xception err1),
  // Xtruct testMultiException(1: string arg0, 2: string arg1) throws(1: Xception err1, 2: Xception2
  // err2)

  // oneway void testOneway(1:i32 secondsToSleep)
};

static int thrift_test_handler_init(struct thrift_test_handler* handler) {
  (void)handler;
  return 0;
}

// dispatch-style
struct thrift_test_processor {
  T_PROCESSOR_METHODS;
  struct thrift_test_handler* handler;
};

static int thrift_test_processor_process(struct t_processor* p,
                                         struct t_protocol* input_protocol,
                                         struct t_protocol* output_protocol) {
  (void)p;
  (void)input_protocol;
  (void)output_protocol;

  return -ENOSYS;
}

static int thrift_test_processor_init(struct thrift_test_processor* p,
                                      struct thrift_test_handler* handler) {
  p->process = thrift_test_processor_process;
  p->handler = handler;
  return 0;
}
}

class ThriftTest : public testing::Test {
public:
  ThriftTest() {}
  ~ThriftTest() {}

protected:
  // storage for server transport and protocol factories
  array<t_socket, 1> transport_factory_storage;
  long transport_factory_usage;
  array<t_binary_protocol, 1> protocol_factory_storage;
  long protocol_factory_usage;

  // C server
  thread server_thread;
  thrift_test_handler c_handler;
  thrift_test_processor c_processor;
  t_server_socket c_server_transport;
  t_socket c_client_transport;
  t_simple_transport_factory c_transport_factory;
  t_binary_protocol_factory c_protocol_factory;
  t_simple_server c_server;

  // C++ client
  shared_ptr<TTransport> cpp_xport;
  shared_ptr<TProtocol> cpp_proto;
  shared_ptr<ThriftTestClient> cpp_client;

  virtual void SetUp() override {
    ASSERT_EQ(0, thrift_test_handler_init(&c_handler));
    ASSERT_EQ(0, thrift_test_processor_init(&c_processor, &c_handler));
    ASSERT_EQ(0, t_server_socket_init(&c_server_transport, "::", 0));
    ASSERT_EQ(0, t_simple_transport_factory_init(&c_transport_factory));
    ASSERT_EQ(0, t_binary_protocol_factory_init(&c_protocol_factory));
    ASSERT_EQ(0, t_simple_server_init(&c_server, (struct t_processor*)&c_processor,
                                      (struct t_server_transport*)&c_server_transport,
                                      (struct t_transport_factory*)&c_transport_factory, nullptr,
                                      (struct t_protocol_factory*)&c_protocol_factory, nullptr));

    server_thread = thread([&]() {
      clog << "calling server.serve()" << endl;
      c_server.serve((t_server*)&c_server);
      clog << "returned from server.serve()" << endl;
    });

    for (; !c_server.running;) {
      usleep(10000);
    }

    cpp_xport = make_shared<TSocket>("::1", c_server_transport.port);
    cpp_proto = make_shared<TBinaryProtocol>(cpp_xport);
    cpp_client = make_shared<ThriftTestClient>(cpp_proto);

    cpp_xport->open();
  }

  virtual void TearDown() override {
    if (t_server_is_valid((t_server*)&c_server)) {
      c_server.stop((t_server*)&c_server);
    }
    server_thread.join();
  }
};

TEST_F(ThriftTest, empty) {}

TEST_F(ThriftTest, TestVoid) {
  cpp_client->testVoid();
  sleep(5);
}

#if 0
TEST_F(ThriftTest, TestString) {
  string actual;
  string expected;

  cpp_client->testString(actual, expected);
  EXPECT_EQ(actual, expected);

  expected = "Hello, Thrift world!";
  cpp_client->testString(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestBool) {
  bool actual;
  bool expected;

  expected = false;
  actual = cpp_client->testBool(expected);
  EXPECT_EQ(actual, expected);

  expected = true;
  actual = cpp_client->testBool(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestByte) {
  int8_t actual;
  int8_t expected;

  expected = INT8_MIN;
  actual = cpp_client->testByte(expected);
  EXPECT_EQ(actual, expected);

  expected = -1;
  actual = cpp_client->testByte(expected);
  EXPECT_EQ(actual, expected);

  expected = 0;
  actual = cpp_client->testByte(expected);
  EXPECT_EQ(actual, expected);

  expected = 1;
  actual = cpp_client->testByte(expected);
  EXPECT_EQ(actual, expected);

  expected = INT8_MAX;
  actual = cpp_client->testByte(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestI32) {
  int32_t actual;
  int32_t expected;

  expected = INT32_MIN;
  actual = cpp_client->testI32(expected);
  EXPECT_EQ(actual, expected);

  expected = -1;
  actual = cpp_client->testI32(expected);
  EXPECT_EQ(actual, expected);

  expected = 0;
  actual = cpp_client->testI32(expected);
  EXPECT_EQ(actual, expected);

  expected = 1;
  actual = cpp_client->testI32(expected);
  EXPECT_EQ(actual, expected);

  expected = INT32_MAX;
  actual = cpp_client->testI32(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestI64) {
  int64_t actual;
  int64_t expected;

  expected = INT64_MIN;
  actual = cpp_client->testI64(expected);
  EXPECT_EQ(actual, expected);

  expected = -1;
  actual = cpp_client->testI64(expected);
  EXPECT_EQ(actual, expected);

  expected = 0;
  actual = cpp_client->testI64(expected);
  EXPECT_EQ(actual, expected);

  expected = 1;
  actual = cpp_client->testI64(expected);
  EXPECT_EQ(actual, expected);

  expected = INT64_MAX;
  actual = cpp_client->testI64(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestDouble) {
  double actual;
  double expected;

  expected = M_PI;
  actual = cpp_client->testDouble(expected);
  EXPECT_EQ(actual, expected);

  expected = -1;
  actual = cpp_client->testDouble(expected);
  EXPECT_EQ(actual, expected);

  expected = 0;
  actual = cpp_client->testDouble(expected);
  EXPECT_EQ(actual, expected);

  expected = 1.333333333;
  actual = cpp_client->testDouble(expected);
  EXPECT_EQ(actual, expected);

  expected = -1e-6;
  actual = cpp_client->testDouble(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestBinary) {
  string actual;
  string expected;

  cpp_client->testBinary(actual, expected);
  EXPECT_EQ(actual, expected);

  expected = "Hello, Binary world!";
  cpp_client->testBinary(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestStruct) {
  Xtruct actual;
  Xtruct expected;

  expected.__set_string_thing("Hello, Struct world!");
  expected.__set_byte_thing(0xaa);
  expected.__set_i32_thing(0x01234567);
  expected.__set_i64_thing(0x0123456789abcdef);

  cpp_client->testStruct(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestNest) {
  Xtruct2 actual;
  Xtruct nest;
  Xtruct2 expected;

  nest.__set_string_thing("Hello, Struct world!");
  nest.__set_byte_thing(0x42);
  nest.__set_i32_thing(0x01234567);
  nest.__set_i64_thing(0x0123456789abcdef);

  expected.__set_byte_thing(0x42);
  expected.__set_struct_thing(nest);
  nest.__set_i32_thing(0x76543210);

  cpp_client->testNest(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestMap) {
  map<int32_t, int32_t> actual;
  actual[0xcafebabe] = 0xb105f00d;
  actual[0x3a210] = 0x2e1da;
  map<int32_t, int32_t> expected;

  cpp_client->testMap(actual, expected);
  EXPECT_EQ(actual, expected);

  expected[42] = INT32_MIN;
  expected[INT32_MAX] = 0;
  cpp_client->testMap(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestStringMap) {
  map<string, string> actual;
  actual["The secret to life, the universe, and everything"] = "forty-two";
  map<string, string> expected;

  cpp_client->testStringMap(actual, expected);
  EXPECT_EQ(actual, expected);

  expected["forty-two"] = "The secret to life, the universe, and everything";
  cpp_client->testStringMap(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestSet) {
  set<int32_t> actual = {0, 1, 2, 3};
  set<int32_t> expected;

  cpp_client->testSet(actual, expected);
  EXPECT_EQ(actual, expected);

  expected = set<int32_t>{4, 5, 6, 7};
  cpp_client->testSet(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestList) {
  vector<int32_t> actual = {0, 1, 2, 3};
  vector<int32_t> expected;

  cpp_client->testList(actual, expected);
  EXPECT_EQ(actual, expected);

  expected = vector<int32_t>{4, 5, 6, 7};
  cpp_client->testList(actual, expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestEnum) {
  Numberz::type actual;
  actual = Numberz::type::SIX;
  Numberz::type expected = Numberz::type::ONE;

  actual = cpp_client->testEnum(expected);
  EXPECT_EQ(actual, expected);

  expected = Numberz::THREE;
  actual = cpp_client->testEnum(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestTypedef) {
  UserId actual;
  actual = 0x0123456789abcdef;
  UserId expected = 0x0011223344556677;

  actual = cpp_client->testTypedef(expected);
  EXPECT_EQ(actual, expected);

  expected = 0xfedcba9876543210;
  actual = cpp_client->testTypedef(expected);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestInsanity) {
  Insanity argument;
  map<UserId, map<Numberz::type, Insanity>> actual;
  map<UserId, map<Numberz::type, Insanity>> expected;

  cpp_client->testInsanity(actual, argument);
  EXPECT_EQ(actual, expected);

  GTEST_SKIP();
}

TEST_F(ThriftTest, TestMulti) {
  Xtruct actual;
  actual.__set_string_thing("qwerty");
  Xtruct expected;

  cpp_client->testMulti(actual, expected.byte_thing, expected.i32_thing, expected.i64_thing,
                        map<int16_t, string>(), Numberz::type::FIVE, 0x0123456789abcdef);
  EXPECT_EQ(actual, expected);

  expected.__set_string_thing("Hello, Struct world!");
  expected.__set_byte_thing(0xaa);
  expected.__set_i32_thing(0x01234567);
  expected.__set_i64_thing(0x0123456789abcdef);
  cpp_client->testMulti(actual, expected.byte_thing, expected.i32_thing, expected.i64_thing,
                        map<int16_t, string>{{42, "forty-two"}, {1980, "The most rad year ever!"}},
                        Numberz::type::TWO, 0xfedcba9876543210);
  EXPECT_EQ(actual, expected);
}

TEST_F(ThriftTest, TestException) {
  GTEST_SKIP();
}

TEST_F(ThriftTest, TestMultiException) {
  GTEST_SKIP();
}

TEST_F(ThriftTest, TestOneWay) {
  GTEST_SKIP();
}

// TODO: Test SecondService
#endif