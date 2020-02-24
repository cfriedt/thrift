#include <gtest/gtest.h>

#include "../../lib/cpp/src/thrift/protocol/ber.h"

using namespace ::std;

//
// berUintEncodeLength
//

TEST( BEREncode, 0 ) {
    uintmax_t x = 0ULL;
    array<uint8_t,1> expected_buffer = { 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,1> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 7f ) {
    uintmax_t x = 0x7fULL;
    array<uint8_t,1> expected_buffer = { 0x7f };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,1> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 80 ) {
    uintmax_t x = 0x80ULL;
    array<uint8_t,2> expected_buffer = { BER_LONG_FORM_MASK | 1, 0x80 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,2> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ff ) {
    uintmax_t x = 0xffULL;
    array<uint8_t,2> expected_buffer = { BER_LONG_FORM_MASK | 1, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,2> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 0100 ) {
    uintmax_t x = 0x0100ULL;
    array<uint8_t,3> expected_buffer = { BER_LONG_FORM_MASK | 2, 0x01, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,3> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffff ) {
    uintmax_t x = 0xffffULL;
    array<uint8_t,3> expected_buffer = { BER_LONG_FORM_MASK | 2, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,3> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 010000 ) {
    uintmax_t x = 0x010000ULL;
    array<uint8_t,4> expected_buffer = { BER_LONG_FORM_MASK | 3, 0x01, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,4> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffff ) {
    uintmax_t x = 0xffffffULL;
    array<uint8_t,4> expected_buffer = { BER_LONG_FORM_MASK | 3, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,4> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 01000000 ) {
    uintmax_t x = 0x01000000ULL;
    array<uint8_t,5> expected_buffer = { BER_LONG_FORM_MASK | 4, 0x01, 0x00, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,5> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffffff ) {
    uintmax_t x = 0xffffffffULL;
    array<uint8_t,5> expected_buffer = { BER_LONG_FORM_MASK | 4, 0xff, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,5> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 0100000000 ) {
    uintmax_t x = 0x0100000000ULL;
    array<uint8_t,6> expected_buffer = { BER_LONG_FORM_MASK | 5, 0x01, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,6> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffffffff ) {
    uintmax_t x = 0xffffffffffULL;
    array<uint8_t,6> expected_buffer = { BER_LONG_FORM_MASK | 5, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,6> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 010000000000 ) {
    uintmax_t x = 0x010000000000ULL;
    array<uint8_t,7> expected_buffer = { BER_LONG_FORM_MASK | 6, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,7> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffffffffff ) {
    uintmax_t x = 0xffffffffffffULL;
    array<uint8_t,7> expected_buffer = { BER_LONG_FORM_MASK | 6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,7> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 01000000000000 ) {
    uintmax_t x = 0x01000000000000ULL;
    array<uint8_t,8> expected_buffer = { BER_LONG_FORM_MASK | 7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,8> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffffffffffff ) {
    uintmax_t x = 0xffffffffffffffULL;
    array<uint8_t,8> expected_buffer = { BER_LONG_FORM_MASK | 7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,8> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, 0100000000000000 ) {
    uintmax_t x = 0x0100000000000000ULL;
    array<uint8_t,9> expected_buffer = { BER_LONG_FORM_MASK | 8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,9> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

TEST( BEREncode, ffffffffffffffff ) {
    uintmax_t x = 0xffffffffffffffffULL;
    array<uint8_t,9> expected_buffer = { BER_LONG_FORM_MASK | 8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = expected_buffer.size();
    int actual_int = berUintEncodeLength( x );
    EXPECT_EQ( actual_int, expected_int );
    array<uint8_t,9> actual_buffer;
    actual_int = berUintEncode( x, & actual_buffer.front(), actual_buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_buffer, expected_buffer );
}

//
// Decode
//

TEST( BERDecode, 0 ) {
    array<uint8_t,1> buffer = { 0 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 7f ) {
    array<uint8_t,1> buffer = { 0x7f };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x7fULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 80 ) {
    array<uint8_t,2> buffer = { BER_LONG_FORM_MASK | 1, 0x80 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x80ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ff ) {
    array<uint8_t,2> buffer = { BER_LONG_FORM_MASK | 1, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 0100 ) {
    array<uint8_t,3> buffer = { BER_LONG_FORM_MASK | 2, 0x01, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x0100ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffff ) {
    array<uint8_t,3> buffer = { BER_LONG_FORM_MASK | 2, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 010000 ) {
    array<uint8_t,4> buffer = { BER_LONG_FORM_MASK | 3, 0x01, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x010000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffff ) {
    array<uint8_t,4> buffer = { BER_LONG_FORM_MASK | 3, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 01000000 ) {
    array<uint8_t,5> buffer = { BER_LONG_FORM_MASK | 4, 0x01, 0x00, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x01000000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffffff ) {
    array<uint8_t,5> buffer = { BER_LONG_FORM_MASK | 4, 0xff, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 0100000000 ) {
    array<uint8_t,6> buffer = { BER_LONG_FORM_MASK | 5, 0x01, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x0100000000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffffffff ) {
    array<uint8_t,6> buffer = { BER_LONG_FORM_MASK | 5, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 010000000000 ) {
    array<uint8_t,7> buffer = { BER_LONG_FORM_MASK | 6, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x010000000000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffffffffff ) {
    array<uint8_t,7> buffer = { BER_LONG_FORM_MASK | 6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 01000000000000 ) {
    array<uint8_t,8> buffer = { BER_LONG_FORM_MASK | 7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x01000000000000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffffffffffff ) {
    array<uint8_t,8> buffer = { BER_LONG_FORM_MASK | 7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffffffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, 0100000000000000 ) {
    array<uint8_t,9> buffer = { BER_LONG_FORM_MASK | 8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0x0100000000000000ULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}

TEST( BERDecode, ffffffffffffffff ) {
    array<uint8_t,9> buffer = { BER_LONG_FORM_MASK | 8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    int expected_int = buffer.size();
    int actual_int = berUintDecodeLength( & buffer.front(), buffer.size() );
    EXPECT_EQ( actual_int, expected_int );
    uintmax_t expected_uintmax = 0xffffffffffffffffULL;
    uintmax_t actual_uintmax;
    actual_int = berUintDecode( & buffer.front(), buffer.size(), & actual_uintmax );
    EXPECT_EQ( actual_int, expected_int );
    EXPECT_EQ( actual_uintmax, expected_uintmax );
}
