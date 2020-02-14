#ifndef BEROID_H_
#define BEROID_H_

#include <stddef.h>
#include <limits.h>
#include <stdint.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * BER-OID encoding, like all(?) MISB types, encodes multibyte data values
 * using network byte order (i.e. in big-endian). Additionally, the most
 * significant bit of each byte of a BER-OID encoded integer is reserved.
 *
 * The most-significant bit is used to indicate that subsequent bytes contain
 * additional data.
 *
 * |0xxxxxxx|                               7-bit value
 * |1xxxxxxx|0xxxxxxx|            8-bit to 14-bit value
 * |1xxxxxxx|1xxxxxxx|0xxxxxxx|  15-bit to 22-bit value
 * ...
 *
 * It is possible to represent an arbitrarily large number that way. The
 * @berOidUintEncode(), @berOidUintDecode(), @berOidUintEncodeLength(), and
 * @berOidUintDecodeLength() only operate on integer values that can be encoded
 * using ISO C integral data types, which translates to unsigned integer values
 * that are in the range [0, @UINTMAX_MAX].
 *
 * For more information on BER-OID encoding, see
 *
 * @see <a href="https://www.itu.int/rec/T-REC-X.690/">ISO/IEC 8825-1:2015 (ITU-T X.690) Information Technology – ASN.1 Encoding
 * Rules - Sub-Identifier defined in Section 8.19.2</a>
 * @see <a href="https://gwg.nga.mil/misb//docs/misp/MISP-2019.2_Motion_Imagery_Handbook.pdf">MISP 2019.2 Motion Imagery Handbook
 * Section 7.3.2</a>
 *
 * It is important to note that BER and BER-OID encoded values are only
 * equivalent if the encoded value is less than or equal to 127.
 *
 * For more information on BER encoding, see
 *
 * @see <a href="https://www.itu.int/rec/T-REC-X.690/">ISO/IEC 8825-1:2015 (ITU-T X.690) Information Technology – ASN.1 Encoding
 * Rules - Length defined in Section 8.1.3.3</a>
 * @see <a href="https://gwg.nga.mil/misb//docs/misp/MISP-2019.2_Motion_Imagery_Handbook.pdf">MISP 2019.2 Motion Imagery Handbook
 * Section 7.3.1</a>
 *
 * Software requirements: MISB ST 0601.15 Compatibility
 *
 * Acronyms:
 *
 * BER  - Basic Encoding Rules
 * OID  - Object Identifier
 * MISB - Motion Imagery Standards Board
 */

/// The number of places a 1 must be shifted to create the BER-OID divisor
#define BEROID_MODULO_SHIFT 7

/// The maximum value that can be stored in a single byte under BER-OID
#define BEROID_BYTE_MAX (1 << BEROID_MODULO_SHIFT)

/// The maximum unsigned integer that can be converted from native representation
#define BEROID_MAX UINTMAX_MAX

/// The number of bytes required to encode BEROID_MAX
#define BEROID_MAX_BYTES 10

/**
 * Encode an unsigned integer using ASN.1 Basic Encoding Rules for Object ID's
 * (BER-OID).
 *
 * If @buffer is NULL and @bufferSize is 0, then this function will return the
 * minimum number of bytes required to encode @x.
 *
 * @param x           the unsigned integer to encode
 * @param buffer      the output buffer
 * @param bufferSize  length of the output buffer
 *
 * @return The minimum, non-negative number of bytes required to encode @x
 * @return -1 when input parameters are invalid or @bufferSize indicates
 *         there is not enough space in @buffer
 */
int berOidUintEncode(const uintmax_t x, void *buffer, const size_t bufferSize);

/**
 * Determine the number of bytes required to encode an unsigned integer using
 * ASN.1 Basic Encoding Rules for Object ID's (BER-OID).
 *
 * @param x the unsigned integer to encode
 *
 * @return The minimum, non-negative number of bytes required to encode @x
 *
 * @see @berOidUintEncode()
 */
int berOidUintEncodeLength(uintmax_t x);

/**
 * Decode an unsigned integer that was encoded using ASN.1 Basic Encoding Rules
 * for Object ID's (BER-OID).
 *
 * On success, this function returns the number of bytes from @buffer used in
 * the decoding process, which is useful for advancing the buffer for subsequent
 * decode operations.
 *
 * If @x is NULL, the function simply returns the number of bytes from @buffer
 * that would be used in the decoding process.
 *
 * @param buffer      the input buffer
 * @param bufferSize  length of the input buffer
 * @param x           a pointer to where the decoded output should be stored
 *
 * @return The positive number of bytes from @buffer used to decode @x
 * @return -1 when @buffer is NULL
 * @return -1 when the input buffer is of insufficient size
 * @return 0 if @x is not NULL and the value stored in @buffer is
 *         unrepresentable by the @uintmax_t type
 */
int berOidUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x);

/**
 * Determine the number of bytes required to decode an unsigned integer encoded
 * using ASN.1 Basic Encoding Rules for Object ID's (BER-OID).
 *
 * Note, this function can return a number of bytes that is unrepresentable by
 * the @uintmax_t type.
 *
 * @param buffer     the buffer containing the BER-OID-encoded integer
 * @param bufferSize the the size of the buffer
 *
 * @return The minimum, non-negative number of bytes required to decode the value in @buffer
 * @return -1 when the input buffer is of insufficient size
 *
 * @see @berOidUintDecode()
 */
int berOidUintDecodeLength(const void *buffer, const size_t bufferSize);

__END_DECLS

#endif /* BEROID_H_ */
