#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "minmax.h"

void hexprint( uint8_t *buf, size_t n ) {

    const size_t bpl = 16; // bytes per line
    const size_t cols_before_ascii = strlen( "0123abcd   ab ab ab ab  ab ab ab ab  ab ab ab ab  ab ab ab ab  " );

    for( size_t o = 0, m = MIN( bpl, n ); n; o += m, n -= m, m = MIN( bpl, n ) ) {

        size_t col = 0;

        printf( "%08x   ", (unsigned int)o );

        col += 11;

        for( size_t i = 0; i < m; ++i ) {

            char c = (char) buf[ o + i ];

            printf( "%02x", ((unsigned)c) & 0xff );
            col += 2;
            if ( 3 == i % 4 ) {
                putchar(' ');
                putchar(' ');
                col += 2;
            } else {
                putchar(' ');
                col += 1;
            }
        }

        for( ; col < cols_before_ascii; col++ ) {
            putchar( ' ' );
        }

        for( size_t i = 0; i < m; ++i ) {

            char c = (char) buf[ o + i ];
            char d = isprint( c ) ? c : '.';

            putchar( d );
            if ( 3 == i % 4 ) {
                putchar(' ');
            }
        }

        putchar( '\n' );
    }
}
