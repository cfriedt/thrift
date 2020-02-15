#include <string>

#include <gtest/gtest.h>

using namespace std;

#include <limits.h>
#include <stdint.h>

#include "array-size.h"
#include "reverseN.h"
#include "beroid.h"

TEST( Sanity, Test_vector_compare_checks_length ) {
    vector<uint8_t> v1({0, 1, 2});
    vector<uint8_t> v2({0, 1, 2, 3});

    ASSERT_NE(v1, v2);
}

//
// ENCODER
//

TEST( berOidUintEncodeLength, Test_0_NULL_0 ) {

    // here we simply test that passing buffer = NULL and bufferSize = 0 causes the FUT to return the required length

    // it's basically like a dry run

    uintmax_t x = 0;
    void *buffer = NULL;
    size_t bufferSize = 0;

    // get the length by callling berOidUintEncode with buffer 0 and bufferSize NULL

    int expected_int = 1;
    int actual_int = ::berOidUintEncode( x, buffer, bufferSize );

    EXPECT_EQ( actual_int, expected_int );

    // ensure that the value is the same when calling berOidUintEncodeLength
    actual_int = ::berOidUintEncodeLength( x );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncodeLength, Test_0_NULL_1_error ) {

    uintmax_t x = 0;
    void *buffer = NULL;
    size_t bufferSize = 1;

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, buffer, bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncodeLength, Test_0_buff_0_error ) {

    uintmax_t x = 0;
    vector<uint8_t> buffer({0});
    size_t bufferSize = 0;

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncodeHappyPath, Test_0_buff_1 ) {

    uintmax_t x = 0;
    uint8_t buffer[ 1 ];
    size_t bufferSize = sizeof(buffer);

    int expected_int = 1;
    int actual_int = ::berOidUintEncode( x, buffer, bufferSize );

    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( buffer[0], 0 );
}


TEST( berOidUintEncodeHappyPath, Test_7f_buff_2_too_big_ok ) {

    // too big is ok..

    uintmax_t x = 0x7f;
    uint8_t buffer[ 2 ];
    size_t bufferSize = sizeof(buffer);

    int expected_int = 1;
    int actual_int = ::berOidUintEncode( x, buffer, bufferSize );

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( buffer[0], 127 );
}

TEST( berOidUintEncodeHappyPath, Test_0601_11_example ) {

    // this is the example given in the JIRA ticket by Stephen Spenler
    // It comes from MISB 0601.11

    uintmax_t x = 144;
    size_t bufferSize = 2;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, 0x10});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_7f_buff_1 ) {

    uintmax_t x = 0x7f;
    uint8_t buffer[ 1 ];
    size_t bufferSize = sizeof(buffer);

    int expected_int = 1;
    int actual_int = ::berOidUintEncode( x, buffer, bufferSize );

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( buffer[0], 127 );
}

TEST( berOidUintEncode, Test_80_buff_1_too_small ) {

    // buffer is too small

    uintmax_t x = 0x7f + 1;
    size_t bufferSize = 1;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_80_buff_2 ) {

    uintmax_t x = 0x7f + 1;
    size_t bufferSize = 2;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_3fff_buff_2 ) {

    uintmax_t x = 0x3fff;
    size_t bufferSize = 2;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_4000_buff_2_too_small ) {

    // too small

    uintmax_t x = 0x3fff + 1;
    size_t bufferSize = 2;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_4000_buff_3 ) {

    uintmax_t x = 0x3fff + 1;
    size_t bufferSize = 3;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX | 0, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_1fffff_buff_3 ) {

    uintmax_t x = 0x1fffff;
    size_t bufferSize = 3;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_200000_buff_3_too_small ) {

    // too small

    uintmax_t x = 0x1fffff + 1;
    size_t bufferSize = 3;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_200000_buff_4 ) {

    uintmax_t x = 0x1fffff + 1;
    size_t bufferSize = 4;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_fffffff_buff_4 ) {

    uintmax_t x = 0xfffffff;
    size_t bufferSize = 4;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_10000000_buff_4_too_small ) {

    // too small

    uintmax_t x = 0xfffffff + 1;
    size_t bufferSize = 4;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_10000000_buff_5 ) {

    uintmax_t x = 0xfffffff + 1;
    size_t bufferSize = 5;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_7ffffffff_buff_5 ) {

    uintmax_t x = 0x7ffffffff;
    size_t bufferSize = 5;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_800000000_buff_6_too_small ) {

    // too small

    uintmax_t x = 0x7ffffffff + 1;
    size_t bufferSize = 5;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_800000000_buff_6 ) {

    uintmax_t x = 0x7ffffffff + 1;
    size_t bufferSize = 6;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_3ffffffffff_buff_6 ) {

    uintmax_t x = 0x3ffffffffff;
    size_t bufferSize = 6;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_40000000000_buff_6_too_small ) {

    // too small

    uintmax_t x = 0x3ffffffffff + 1;
    size_t bufferSize = 6;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_40000000000_buff_7 ) {

    uintmax_t x = 0x3ffffffffff + 1;
    size_t bufferSize = 7;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_1ffffffffffff_buff_7 ) {

    uintmax_t x = 0x1ffffffffffffULL;
    size_t bufferSize = 7;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_2000000000000_buff_7_too_small ) {

    // too small

    uintmax_t x = 0x1ffffffffffffULL + 1;
    size_t bufferSize = 7;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_2000000000000_buff_8 ) {

    uintmax_t x = 0x1ffffffffffffULL + 1;
    size_t bufferSize = 8;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_ffffffffffffff_buff_8 ) {

    uintmax_t x = 0xffffffffffffffULL;
    size_t bufferSize = 8;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_100000000000000_buff_8_too_small ) {

    // too small

    uintmax_t x = 0xffffffffffffffULL + 1;
    size_t bufferSize = 8;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_100000000000000_buff_9 ) {

    // too small

    uintmax_t x = 0xffffffffffffffULL + 1;
    size_t bufferSize = 9;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_7fffffffffffffff_buff_9 ) {

    uintmax_t x = 0x7fffffffffffffffULL;
    size_t bufferSize = 9;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_8000000000000000_buff_9_too_small ) {

    // too small

    uintmax_t x = 0x7fffffffffffffffULL + 1;
    size_t bufferSize = 9;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = -1;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintEncode, Test_8000000000000000_buff_10 ) {

    uintmax_t x = 0x7fffffffffffffffULL + 1;
    size_t bufferSize = 10;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

TEST( berOidUintEncode, Test_ffffffffffffffff_BEROID_MAX_buff_10 ) {

    uintmax_t x = BEROID_MAX;
    size_t bufferSize = 10;
    vector<uint8_t> buffer( bufferSize, 0 );

    int expected_int = bufferSize;
    int actual_int = ::berOidUintEncode( x, & buffer.front(), bufferSize );

    vector<uint8_t> expected_vu8({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    vector<uint8_t> actual_vu8(buffer);

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_vu8, expected_vu8 );
}

//
// DECODER
//

TEST( berOidUintDecodeLength, Test_0_buff_1_NULL ) {

    // here we simply test that passing x = NULL which causes the FUT to return the required length

    // it's basically like a dry run

    const vector<uint8_t> buffer({0});
    const size_t bufferSize = buffer.size();

    int expected_int = 1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, NULL );

    ASSERT_EQ(buffer.size(), 1);
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecodeHappyPath, Test_0601_11_example ) {

    // this is the example given in the JIRA ticket by Stephen Spenler
    // It comes from MISB 0601.11

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, 0x10});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 144ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 2);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecodeHappyPath, Test_7fabcdef_buff_1_do_not_read_past_buffSize ) {

    const vector<uint8_t> buffer({0x7f,0xab,0xcd,0xef});
    const size_t bufferSize = 1;

    uintmax_t expected_uintmax = 0x7fULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecodeCornerCase, Test_unrepresentable ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0xab, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t dummy;

    int expected_int = 0;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 10);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_00_buff_1 ) {

    const vector<uint8_t> buffer({0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 1);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_00_buff_0_too_small ) {

    const vector<uint8_t> buffer({0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_7f_buff_1 ) {

    const vector<uint8_t> buffer({0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x7fULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 1);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_80_buff_1_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 2);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_80_buff_2 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x80ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 2);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_3fff_buff_2 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x3fffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 2);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_4000_buff_2_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX | 0, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_4000_buff_3 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX | 0, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x4000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_1fffff_buff_3 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x1fffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_200000_buff_3_too_small ) {

    vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 4);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_200000_buff_4 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x200000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 4);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_fffffff_buff_4 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0xfffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 4);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_10000000_buff_4_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 5);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_10000000_buff_5 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x10000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 5);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_7ffffffff_buff_5 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x7ffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 5);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_800000000_buff_5_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 6);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_800000000_buff_6 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x800000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 6);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_3ffffffffff_buff_6 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x3ffffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 6);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_40000000000_buff_6_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 7);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_40000000000_buff_7 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x40000000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 7);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_1ffffffffffff_buff_7 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x1ffffffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 7);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_2000000000000_buff_7_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_2000000000000_buff_8 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x2000000000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_ffffffffffffff_buff_8 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0xffffffffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 8);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_100000000000000_buff_8_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 9);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_100000000000000_buff_9 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x100000000000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 9);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}


TEST( berOidUintDecode, Test_7fffffffffffffff_buff_9 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x7fffffffffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 9);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_8000000000000000_buff_9_too_small ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size() - 1;

    uintmax_t dummy = -1;

    int expected_int = -1;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & dummy );

    ASSERT_EQ(buffer.size(), 10);
    ASSERT_EQ( actual_int, expected_int );
}

TEST( berOidUintDecode, Test_8000000000000000_buff_10 ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, BEROID_BYTE_MAX, 0});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0x8000000000000000ULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 10);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( berOidUintDecode, Test_ffffffffffffffff_buff_10_BEROID_MAX ) {

    const vector<uint8_t> buffer({BEROID_BYTE_MAX | 1, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, BEROID_BYTE_MAX | 0x7f, 0x7f});
    const size_t bufferSize = buffer.size();

    uintmax_t expected_uintmax = 0xffffffffffffffffULL;
    uintmax_t actual_uintmax = -1;

    int expected_int = bufferSize;
    int actual_int = ::berOidUintDecode( & buffer.front(), bufferSize, & actual_uintmax );

    ASSERT_EQ(buffer.size(), 10);
    ASSERT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

//
// CODEC
//

TEST( berOidUintEncodeDecode, Test_0601_11_example ) {

    // this is the example given in the JIRA ticket by Stephen Spenler
    // It comes from MISB 0601.11

    const unsigned x = 144;

    size_t bufferSize = ::berOidUintEncodeLength( x );
    vector<uint8_t> buffer( bufferSize, 0 );

    uintmax_t expected_uintmax = x;
    uintmax_t actual_uintmax;

    ASSERT_EQ( ::berOidUintEncode( x, & buffer.front(), buffer.size() ), bufferSize );
    ASSERT_EQ( ::berOidUintDecode( & buffer.front(), buffer.size(), & actual_uintmax ), bufferSize );

    EXPECT_EQ( actual_uintmax, expected_uintmax );
}
