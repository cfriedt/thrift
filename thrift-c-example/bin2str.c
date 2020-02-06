#include "bin2str.h"

static char buf[ 1024 ];

char *bin2str( const uint8_t *data, const size_t size ) {
    size_t i, j;
    for( i = 0, j = 0; i < size && j < sizeof( buf ); ++i ) {
        uint8_t byte = data[ i ];
        buf[ j++ ] = ( byte >> 4 ) & 0xf;
        buf[ j++ ] = ( byte >> 0 ) & 0xf;
        if ( i < size -1 ) {
            buf[ j++ ] = ',';
        }
    }
    buf[ j ] = '\0';
    return buf;
}
