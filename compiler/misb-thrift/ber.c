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

    if ( x < BER_SINGLE_BYTE_MODULUS ) {
        if ( write ) {
            *((uint8_t *)buffer) = x;
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

        uint8_t *buf = (uint8_t *) buffer;
        uint8_t offs = 0;
        buf[ offs++ ] = BER_LONG_FORM_MASK | bytes;
        for( uint8_t b = bytes; b; --b ) {
            buf[ offs++ ] = (uint8_t)( x >> ( b * 8 ) );
        }
    }

    return bytes + 1;
}

int berUintEncodeLength(uintmax_t x) {
    return berUintEncode( x, NULL, 0 );
}

int berUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x) {
    (void) buffer;
    (void) bufferSize;
    (void) x;
    return -1;
}

int berUintDecodeLength(const void *buffer, const size_t bufferSize) {
    return berUintDecode( buffer, bufferSize, NULL );
}
