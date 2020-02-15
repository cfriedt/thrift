#include <gtest/gtest.h>

#include "ber.h"

using namespace ::std;
TEST( berUintEncodeLength, encode_0 ) {
    uintmax_t x = 0ULL;
    int expected_int = 1;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_7f ) {
    uintmax_t x = 0x7fULL;
    int expected_int = 1;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_80 ) {
    uintmax_t x = 0x80ULL;
    int expected_int = 2;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ff ) {
    uintmax_t x = 0xffULL;
    int expected_int = 2;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_0100 ) {
    uintmax_t x = 0x0100ULL;
    int expected_int = 3;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffff ) {
    uintmax_t x = 0xffffULL;
    int expected_int = 3;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_010000 ) {
    uintmax_t x = 0x010000ULL;
    int expected_int = 4;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffff ) {
    uintmax_t x = 0xffffffULL;
    int expected_int = 4;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_01000000 ) {
    uintmax_t x = 0x01000000ULL;
    int expected_int = 5;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffffff ) {
    uintmax_t x = 0xffffffffULL;
    int expected_int = 5;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_0100000000 ) {
    uintmax_t x = 0x0100000000ULL;
    int expected_int = 6;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffffffff ) {
    uintmax_t x = 0xffffffffffULL;
    int expected_int = 6;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_010000000000 ) {
    uintmax_t x = 0x010000000000ULL;
    int expected_int = 7;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffffffffff ) {
    uintmax_t x = 0xffffffffffffULL;
    int expected_int = 7;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_01000000000000 ) {
    uintmax_t x = 0x01000000000000ULL;
    int expected_int = 8;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffffffffffff ) {
    uintmax_t x = 0xffffffffffffffULL;
    int expected_int = 8;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_0100000000000000 ) {
    uintmax_t x = 0x0100000000000000ULL;
    int expected_int = 9;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}

TEST( berUintEncodeLength, encode_ffffffffffffffff ) {
    uintmax_t x = 0xffffffffffffffffULL;
    int expected_int = 9;
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
}
