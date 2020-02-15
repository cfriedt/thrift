#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "ber.h"

int berUintEncode(const uintmax_t x, void *buffer, const size_t bufferSize) {
    bool write = true;

    if ( NULL == buffer || 0 == bufferSize ) {
        if ( NULL == buffer && 0 == bufferSize ) {
            write = false;
        } else {
            return -1;
        }
    }

    uint8_t *buf = (uint8_t *) buffer;
    if ( x < BER_SINGLE_BYTE_MODULUS ) {
        if ( write ) {
            buf[ 0 ] = x;
        }
        return 1;
    }

    uint8_t leading_zeros = __builtin_clzl(x);
    uint8_t bits = sizeof( uintmax_t ) * 8 - leading_zeros;
    // ceil( bits / bytes ) = (bits / 8) + ((bits % 8) != 0) = (bits + 7) / 8;
    uint8_t bytes = (bits + 7) / 8;

    if ( write ) {
        if ( bufferSize < (size_t)(bytes + 1) ) {
            return -1;
        }

        uint8_t offs = 0;
        buf[ offs++ ] = BER_LONG_FORM_MASK | bytes;
        for( int i = bytes - 1; i >= 0; --i ) {
            buf[ offs++ ] = (uint8_t)( x >> ( i * 8 ) );
        }
    }

    return bytes + 1;
}

int berUintEncodeLength(uintmax_t x) {
    return berUintEncode( x, NULL, 0 );
}

int berUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x) {

    if ( NULL == buffer || 0 == bufferSize ) {
        return -1;
    }

    bool write = (NULL != x);

    uint8_t *buf = (uint8_t *)buffer;
    if ( !(buf[ 0 ] & BER_LONG_FORM_MASK) ) {
        if ( write ) {
            *x = buf[ 0 ];
        }
        return 1;
    }

    uint8_t bytes = (buf[ 0 ] & (~BER_LONG_FORM_MASK));
    if ( bytes > sizeof(uintmax_t) ) {
        // unrepresentable in ISO C
        return -1;
    }

    if ( write ) {
        *x = 0;
        for( uint8_t b = 1; b <= bytes; ++b ) {
            *x <<= 8;
            *x |= buf[ b ];
        }
    }

    return bytes + 1;
}

int berUintDecodeLength(const void *buffer, const size_t bufferSize) {
    return berUintDecode( buffer, bufferSize, NULL );
}
