#include <gtest/gtest.h>

#include "../../lib/cpp/src/thrift/protocol/imap.h"

using namespace std;

/**
 * Test from MISB ST 1201.4 Example 3
 * IMAPA(-900, 19000, 0.5) for -900
 */
TEST( IMAPATest, Example3_n900 ) {
    const double lowerBound = -900;
    const double upperBound = 19000;
    const double precision = 0.5;

    const size_t expected_size = 3;
    size_t actual_size = ::imapAEncodeLength( lowerBound, upperBound, precision );
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_size, expected_size );

    const double expected_double = -900;
    const uintmax_t expected_uintmax = 0;
    uintmax_t actual_uintmax;
    actual_size = ::imapAEncode(lowerBound, upperBound, precision, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapADecode(lowerBound, upperBound, precision, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double );
}

/**
 * Test from MISB ST 1201.4 Example 3
 * IMAPA(-900, 19000, 0.5) for 0
 */
TEST( IMAPATest, Example3_0 ) {
    const double lowerBound = -900;
    const double upperBound = 19000;
    const double precision = 0.5;

    const size_t expected_size = 3;
    size_t actual_size = ::imapAEncodeLength( lowerBound, upperBound, precision );
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_size, expected_size );

    const double expected_double = 0;
    const uintmax_t expected_uintmax = 230400;
    uintmax_t actual_uintmax;
    actual_size = ::imapAEncode(lowerBound, upperBound, precision, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapADecode(lowerBound, upperBound, precision, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double );
}

/**
 * Test from MISB ST 1201.4 Example 3
 * IMAPA(-900, 19000, 0.5) for 10
 */
TEST( IMAPATest, Example3_10 ) {
    const double lowerBound = -900;
    const double upperBound = 19000;
    const double precision = 0.5;

    const size_t expected_size = 3;
    size_t actual_size = ::imapAEncodeLength( lowerBound, upperBound, precision );
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_size, expected_size );

    const double expected_double = 10.0;
    const uintmax_t expected_uintmax = 232960;
    uintmax_t actual_uintmax;
    actual_size = ::imapAEncode(lowerBound, upperBound, precision, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapADecode(lowerBound, upperBound, precision, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double );
}

/**
 * Test from MISB ST 1201.4 Example 3
 * IMAPA(-900, 19000, 0.5) for -Inf
 */
TEST( IMAPATest, Example3_nInf ) {
    const double lowerBound = -900;
    const double upperBound = 19000;
    const double precision = 0.5;

    const size_t expected_size = 3;
    size_t actual_size = ::imapAEncodeLength( lowerBound, upperBound, precision );
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_size, expected_size );

    ASSERT_TRUE( ::numeric_limits<double>::has_infinity );
    ASSERT_TRUE( ::numeric_limits<double>::is_iec559 );

    const double expected_double = -numeric_limits<double>::infinity();
    const uintmax_t expected_uintmax = 0xe80000;
    uintmax_t actual_uintmax;
    actual_size = ::imapAEncode(lowerBound, upperBound, precision, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapADecode(lowerBound, upperBound, precision, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double );
}

/**
 * Test from MISB ST 1201.4 Example 4
 * IMAPB(0.1, 0.9, 2) for 0.1
 */
TEST( IMAPBTest, Example4_0p1 ) {
    const double lowerBound = 0.1;
    const double upperBound = 0.9;

    const size_t expected_size = 2;
    size_t actual_size;

    const double expected_double = 0.1;
    const uintmax_t expected_uintmax = 0;
    uintmax_t actual_uintmax;
    actual_size = ::imapBEncode(lowerBound, upperBound, expected_size, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapBDecode(lowerBound, upperBound, expected_size, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double );
}

/**
 * Test from MISB ST 1201.4 Example 4
 * IMAPB(0.1, 0.9, 2) for 0.5
 */
TEST( IMAPBTest, Example4_0p5 ) {
    const double lowerBound = 0.1;
    const double upperBound = 0.9;

    const size_t expected_size = 2;
    size_t actual_size;

    const double expected_double = 0.5;
    const uintmax_t expected_uintmax = 0x3333;
    uintmax_t actual_uintmax;
    actual_size = ::imapBEncode(lowerBound, upperBound, expected_size, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapBDecode(lowerBound, upperBound, expected_size, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_NEAR( actual_double, expected_double, 0.001 );

    EXPECT_NEAR( expected_double - actual_double, 6.10e-6, 1e-8 );
}

/**
 * Test from MISB ST 1201.4 Example 4
 * IMAPB(0.1, 0.9, 2) for 0.9
 */
TEST( IMAPBTest, Example4_0p9 ) {
    const double lowerBound = 0.1;
    const double upperBound = 0.9;

    const size_t expected_size = 2;
    size_t actual_size;

    const double expected_double = 0.9;
    const uintmax_t expected_uintmax = 0x6666;
    uintmax_t actual_uintmax;
    actual_size = ::imapBEncode(lowerBound, upperBound, expected_size, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapBDecode(lowerBound, upperBound, expected_size, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_NEAR( actual_double, expected_double, 0.001 );

    EXPECT_NEAR( expected_double - actual_double, 1.22e-5, 1e-7 );
}

/**
 * Test from MISB ST 1201.4 Example 4
 * IMAPB(0.1, 0.9, 2) for -Inf
 */
TEST( IMAPBTest, Example4_nInf ) {
    const double lowerBound = 0.1;
    const double upperBound = 0.9;

    const size_t expected_size = 2;
    size_t actual_size;

    const double expected_double = -numeric_limits<double>::infinity();
    const uintmax_t expected_uintmax = 0xe800;
    uintmax_t actual_uintmax;
    actual_size = ::imapBEncode(lowerBound, upperBound, expected_size, expected_double, & actual_uintmax);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_uintmax, expected_uintmax );

    double actual_double;
    actual_size = ::imapBDecode(lowerBound, upperBound, expected_size, expected_uintmax, & actual_double);
    ASSERT_NE( int(actual_size), -1 );
    EXPECT_EQ( actual_double, expected_double);
}
