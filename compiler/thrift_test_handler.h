#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct thrift_test_handler;
struct thrift_test_handler {
  void (*testVoid)(void);
  // string testString(string thing);
  // bool (*testBool)(bool thing);
  // int8_t (*testByte)(int8_t thing);
  // int32_t (*testI32)(int32_t thing);
  // int64_t (*testI64)(int64_t thing);
  // double (*testDouble)(double thing);
  // binary testBinary(binary thing);
  // Xtruct (*testStruct)(Xtruct thing);
  // Xtruct2 (*testNest)(Xtruct2 thing);
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

#ifdef __cplusplus
}
#endif
