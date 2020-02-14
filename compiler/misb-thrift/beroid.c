#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "array-size.h"

#include "beroid.h"
#include "reverseN.h"

// Overall O(3*C/8) => O(1)
int berOidUintEncode(const uintmax_t x, void *buffer, const size_t bufferSize)
{
    // If ISO C requires numbers greater than 8 bytes in size, simply add entries to this array
    // If this were C++, I wonder if I could make this constepxr..
    static const uintmax_t limits[] = {
        // clang-format off

        // fits in 1 byte, 0x0000000000000007f
        0x00000000000000ffULL >> 1,

        // fits in 2 bytes, 0x0000000000003fff
        0x000000000000ffffULL >> 2,

        // fits in 3 bytes, 0x00000000001fffff
        0x0000000000ffffffULL >> 3,

        // fits in 4 bytes, 0x000000000fffffff
        0x00000000ffffffffULL >> 4,

        // fits in 5 bytes, 0x00000007ffffffff
        0x000000ffffffffffULL >> 5,

        // fits in 6 bytes, 0x000003ffffffffff
        0x0000ffffffffffffULL >> 6,

        // fits in 7 bytes, 0x0001ffffffffffff
        0x00ffffffffffffffULL >> 7,

        // fits in 8 bytes, 0x00ffffffffffffff
        0xffffffffffffffffULL >> 8,

        // fits in 9 bytes, 0x7fffffffffffffff
        0xffffffffffffffffULL >> 1,

        // clang-format on
    };

    bool write;
    size_t bytesRequired;
    uintmax_t dividend;

    uintmax_t remainder;
    uint8_t *out;

    if (NULL == buffer && 0 != bufferSize) {
        return -1;
    }

    // only write data to the buffer when buffer and bufferSize are non-NULL / non-zero
    write = !(NULL == buffer && 0 == bufferSize);

    // count bytes required
    // O(C/8) => O(1), C is the register bit-width
    for (bytesRequired = 1; bytesRequired - 1 < ARRAY_SIZE(limits); bytesRequired++) {
        if (x <= limits[bytesRequired - 1]) {
            break;
        }
    }

    if (write && bytesRequired > bufferSize) {
        return -1;
    }

    // encode in little-endian
    // O(C/8) => O(1), C is the register bit-width
    dividend = x;
    out      = (uint8_t *) buffer;
    for (size_t i = 0; write && i < bytesRequired; i++, out++) {
        remainder = dividend % BEROID_BYTE_MAX;
        *out      = remainder;
        dividend >>= BEROID_MODULO_SHIFT;
        if (i > 0) {
            // in BER-OID every byte except the least-significant byte has the most-significant bit set
            *out |= BEROID_BYTE_MAX;
        }
    }

    // reverse to big-endian
    // O(C/8) => O(1), C is the register bit-width
    if (write) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        // output should be in network byte order
        reverseN(buffer, bytesRequired);
#endif
    }

    return bytesRequired;
}

int berOidUintEncodeLength(uintmax_t x)
{
    return berOidUintEncode(x, NULL, 0);
}

#if !defined(HAVE_THIRD_PARTY_DECODE_VERIFICATION)
// Overall, O(2C/8) => O(1), C is the register bit-width
int berOidUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x)
{
    int bytesRequired = 1;
    uint8_t *in       = (uint8_t *) buffer;
    bool write        = (NULL != x);

    if (NULL == buffer || 0 == bufferSize) {
        return -1;
    }

    // O(C/8) => O(1), C is the register bit-width
    for (size_t i = 0; i < bufferSize; i++, bytesRequired++) {
        // the most significant bit of the last byte will be zero
        if (0 == (in[i] & BEROID_BYTE_MAX)) {
            break;
        }
    }

    if ((size_t) bytesRequired > bufferSize) {
        return -1;
    }

    if (write && BEROID_MAX_BYTES == bytesRequired && (BEROID_BYTE_MAX | 1) != in[0]) {
        // we have a value that is unrepresentable using uintmax_t
        return 0;
    }

    if (write) {
        *x = 0;
    }

    // O(C/8) => O(1), C is the register bit-width
    for (size_t i = 0; write && i < (size_t) bytesRequired; i++) {
        *x |= (in[i] & (~BEROID_BYTE_MAX));
        if (i + 1 < (size_t) bytesRequired) {
            *x <<= BEROID_MODULO_SHIFT;
        }
    }

    return bytesRequired;
}

int berOidUintDecodeLength(const void *buffer, const size_t bufferSize)
{
    return berOidUintDecode(buffer, bufferSize, NULL);
}
#endif /* !defined(HAVE_THIRD_PARTY_DECODE_VERIFICATION) */
