#include <cstdlib>
#include <vector>

#include <gtest/gtest.h>

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TWrappedCTransport.h"

#include "thrift/c/protocol/t_binary_protocol.h"
#include "thrift/c/transport/t_buffer_transports.h"

#include "ThriftTest.h"

using namespace std;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace thrift::test;

// TODO: create a ThriftTest 'handler' for the 'processor' (normally auto-generated)
struct thrift_test_handler {
  void (*testVoid)(void);
  // string testString(string thing);
  bool (*testBool)(bool thing);
  int8_t (*testByte)(int8_t thing);
  int32_t (*testI32)(int32_t thing);
  int64_t (*testI64)(int64_t thing);
  double (*testDouble)(double thing);
  // binary testBinary(binary thing);
  struct Xtruct (*testStruct)(struct Xtruct thing);
  struct Xtruct2 (*testNest)(struct Xtruct2 thing);
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

// TODO: create a ThriftTest 'processor' for `t_simple_server` (normally auto-generated)

class ThriftTest : public testing::Test {
protected:
  vector<uint8_t> memory;

  // C++ client
  shared_ptr<TTransport> cpp_xport;
  shared_ptr<TProtocol> cpp_proto;
  shared_ptr<ThriftTestClient> cpp_client;

  // C server
  t_memory_buffer c_xport;
  t_binary_protocol c_proto;

  void SetUp(void) override {
    memory = vector<uint8_t>(64, 0);
    ASSERT_EQ(0, t_memory_buffer_init(&c_xport, (void*)&memory.front(), memory.size()));
    ASSERT_EQ(0, t_binary_protocol_init(&c_proto, (t_transport*)&c_xport, nullptr));
    // TODO: create a ThriftTest 'handler' for the 'processor' (normally auto-generated)
    // TODO: create a ThriftTest 'processor' for `t_simple_server` (normally auto-generated)
    // TODO: create a t_memory_buffer 'server transport' for `t_simple_server` (not auto)
    // TODO: create a t_memory_buffer 'transport factory' for `t_simple_server` (not auto)
    // TODO: create a t_binary_protocol 'protocol factory' for `t_simple_server` (not auto)
    cpp_xport = shared_ptr<TTransport>(new TWrappedCTransport((t_transport*)&c_xport));
    cpp_proto = shared_ptr<TProtocol>(new TBinaryProtocol(cpp_xport));
  }

  void TearDown(void) override {}
};

TEST_F(ThriftTest, empty) {}
