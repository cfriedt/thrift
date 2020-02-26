#include <math.h>
#include <stdbool.h>

#include "imap.h"

/* The MISB IMAP mask always occupies the most-significant NBITS.
 * Since the number of bytes is not fixed, there is no defined shift.
 * The L (sizeInBytes) value needs to be calculated in order to determine the mask.
 */
#define IMAP_SPECIAL_MASK 0b11111
#define IMAP_SPECIAL_MASK_NBITS 5

#define IMAP_SPECIAL_P_INF 0b11001
#define IMAP_SPECIAL_N_INF 0b11101

#define IMAP_SPECIAL_P_QNAN 0b11010
#define IMAP_SPECIAL_N_QNAN 0b11110

#define IMAP_SPECIAL_P_SNAN 0b11011
#define IMAP_SPECIAL_N_SNAN 0b11111

#define FLOAT64_N_INF 0xfff0000000000000ULL
#define FLOAT64_P_INF 0x7ff0000000000000ULL
#define FLOAT64_N_SNAN 0xfff4000000000000ULL
#define FLOAT64_P_SNAN 0x7ff4000000000000ULL
#define FLOAT64_N_QNAN 0xfff8000000000000ULL
#define FLOAT64_P_QNAN 0x7ff8000000000000ULL

static inline bool imapBoundariesAreValid( const double a, const double b ) {
    return !(
        false
        || isnan( a )
        || isnan( b )
        || isinf( a )
        || isinf( b )
        || b <= a
    );
}

static inline bool imapPrecisionIsValid( const double a, const double b, const double g ) {
    return !(
        false
        || ( ! imapBoundariesAreValid( a, b ) )
        || 0 == g
        || g < 0
        || isnan( g )
        || isinf( g )
        || (b - a) < g
    );
}

static inline bool imapSizeIsValid( const size_t sizeInBytes ) {
    return
        true
        && 0 < sizeInBytes
        && sizeInBytes <= sizeof( uintmax_t )
        ;
}

struct imapSpecial {
    uint8_t mask;
    double val;
};

static uintmax_t imapSpecialValue( const double _x, const size_t L ) {

    uintmax_t y;
    double x = _x;

    // See Table 2 in ST 1201.4
    if ( false ) {
    } else if ( FLOAT64_P_INF == *((uint64_t *)&x) ) {
        // +INFINITY
        y = IMAP_SPECIAL_P_INF << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else if ( FLOAT64_N_INF == *((uint64_t *)&x) ) {
        // -INFINITY
        y = IMAP_SPECIAL_N_INF << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else if ( FLOAT64_P_SNAN == *((uint64_t *)&x) ) {
        // +SNAN
        y = IMAP_SPECIAL_P_SNAN << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else if ( FLOAT64_N_SNAN == *((uint64_t *)&x) ) {
        // -SNAN
        y = IMAP_SPECIAL_N_SNAN << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else if ( FLOAT64_P_QNAN == *((uint64_t *)&x) ) {
        // +QNAN
        y = IMAP_SPECIAL_P_QNAN << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else if ( FLOAT64_N_QNAN == *((uint64_t *)&x) ) {
        // -QNAN
        y = IMAP_SPECIAL_N_QNAN << ( L * 8 - IMAP_SPECIAL_MASK_NBITS );
    } else {
        y = 0;
    }
    return y;
}

// convert the most significant IMAP_SPECIAL_MASK_NBITS of a value to a double
// if the value is special
static double imapToSpecialValue( const uint8_t mask ) {
    uint64_t tmp;
    switch( mask ) {
    case IMAP_SPECIAL_P_INF: tmp = FLOAT64_P_INF; break;
    case IMAP_SPECIAL_N_INF: tmp = FLOAT64_N_INF; break;
    case IMAP_SPECIAL_P_SNAN: tmp = FLOAT64_P_SNAN; break;
    case IMAP_SPECIAL_N_SNAN: tmp = FLOAT64_N_SNAN; break;
    case IMAP_SPECIAL_P_QNAN: tmp = FLOAT64_P_QNAN; break;
    case IMAP_SPECIAL_N_QNAN: tmp = FLOAT64_N_QNAN; break;
    default: return 0;
    }
    return *( (double *) & tmp );
}

// check if the most significant IMAP_SPECIAL_MASK_NBITS of a value are reserved
static inline bool imapIsReserved( const uint8_t mask ) {

    return
        false
        || 0b11100 == mask
        || 0b10100 == (0b11100 & mask)
        ;

}

// check if the most significant IMAP_SPECIAL_MASK_NBITS of a value are reserved
static inline bool imapIsUser( const uint8_t mask ) {

    return 0b11000 == mask;
}

static inline size_t imapSizeInBytes( const double a, const double b, const double g ) {

    const size_t Lbits = ceil(log2(b-a)) - floor(log2(g)) + 1;
    const size_t L = ceil( Lbits / 8.0 );

    return L;
}

static inline double imapZOffset( const double a, const double b, const double sF ) {

    double zOffset = 0.0;

    if ( a < 0 && b > 0 ) {
        zOffset = sF * a - floor( sF * a );
    }

    return zOffset;
}

int imapAEncode( const double lowerBound, const double upperBound, const double precision, const double value, uintmax_t *result ) {

    if ( ! imapPrecisionIsValid( lowerBound, upperBound, precision ) ) {
        return -1;
    }

    return imapBEncode( lowerBound, upperBound, imapSizeInBytes( lowerBound, upperBound, precision ), value, result );
}

int imapADecode( const double lowerBound, const double upperBound, const double precision, const uintmax_t value, double *result ) {

    if ( ! imapPrecisionIsValid( lowerBound, upperBound, precision ) ) {
        return -1;
    }

    return imapBDecode( lowerBound, upperBound, imapSizeInBytes( lowerBound, upperBound, precision ), value, result );
}

static inline void imapConstants( const double a, const double b, const size_t L, double *sF, double *sR ) {
    const size_t bPow = ceil(log2(b-a));
    const size_t dPow = 8 * L - 1;
    if ( NULL != sF ) {
        *sF = pow(2, dPow - bPow);
    }
    if ( NULL != sR ) {
        *sR = 1.0 / pow(2, dPow - bPow);
    }
}

static inline bool imapBit( const size_t bit, const uintmax_t y ) {
    return (bool) ( y >> bit );
}

int imapBEncode( const double lowerBound, const double upperBound, const size_t sizeInBytes, const double value, uintmax_t *result ) {

    const bool write = (NULL != result);

    if ( false == imapBoundariesAreValid( lowerBound, upperBound ) ) {
        return -1;
    }

    const size_t L = sizeInBytes;

    if ( write ) {

        if ( ! imapSizeIsValid( sizeInBytes ) ) {
            return -1;
        }

        // just aliases that line up with MISB ST1201.4 Example 3
        const double a = lowerBound;
        const double x = value;
        uintmax_t *y = result;
        double sF;

        imapConstants( lowerBound, upperBound, L, & sF, NULL );

        const double zOffset = imapZOffset( lowerBound, upperBound, sF );

        *y = imapSpecialValue( x, L );
        if ( 0 == *y ) {
            *y = (uintmax_t) floor( sF * ( x - a ) + zOffset );
        }
    }

    return L;
}

int imapBDecode( const double lowerBound, const double upperBound, const size_t sizeInBytes, const uintmax_t value, double *result ) {
    const bool write = (NULL != result);

    if ( false == imapBoundariesAreValid( lowerBound, upperBound ) ) {
        return -1;
    }

    const size_t L = sizeInBytes;

    if ( ! imapSizeIsValid( sizeInBytes ) ) {
        return -1;
    }

    const uint8_t msbs = value >> ( 8 * ( L - 1 ) + 3 );

    if ( imapIsUser( msbs ) || imapIsReserved( msbs ) ) {
        return -1;
    }

    if ( write ) {

        // just aliases that line up with MISB ST1201.4 Example 3
        const double a = lowerBound;
        const uintmax_t y = value;
        double *x = result;
        double sF;
        double sR;

        imapConstants( lowerBound, upperBound, L, & sF, & sR );

        const double zOffset = imapZOffset( lowerBound, upperBound, sF );

        const size_t msb = 8 * L - 1;
        if ( imapBit( msb, y ) && imapBit( msb - 1, y ) ) {
            *x = imapToSpecialValue( msbs );
        } else {
            *x = sR * ( y - zOffset ) + a;
        }
    }

    return L;
}
