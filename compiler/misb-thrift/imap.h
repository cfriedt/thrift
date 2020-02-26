#ifndef IMAP_H_
#define IMAP_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int imapAEncode( const double lowerBound, const double upperBound, const double precision, const double value, uintmax_t *result );
int imapADecode( const double lowerBound, const double upperBound, const double precision, const uintmax_t value, double *result );

int imapBEncode( const double lowerBound, const double upperBound, const size_t sizeInBytes, const double value, uintmax_t *result );
int imapBDecode( const double lowerBound, const double upperBound, const size_t sizeInBytes, const uintmax_t value, double *result );

static inline int imapAEncodeLength( const double lowerBound, const double upperBound, const double precision ) {
    return imapAEncode( lowerBound, upperBound, precision, 0, NULL );
}

__END_DECLS

#endif /* IMAP_H_ */
