#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "hexprint.h"
#include "readall.h"

ssize_t readall( int fd, void *buf, size_t buf_size ) {

    size_t o, n;
    for( o = 0, n = 0; buf_size; o += n, buf_size -= n ) {

        int nfds = fd + 1;
        fd_set rfds;
        fd_set efds;
        FD_ZERO( & rfds );
        FD_ZERO( & efds );
        FD_SET( fd, & rfds );
        FD_SET( fd, & efds );

        int r = select( nfds, & rfds, NULL, & efds, NULL );
        if ( -1 == r ) {
            perror("select");
            break;
        }
        if ( FD_ISSET( fd, & efds ) ) {
            D( "exception occurred on fd %d", fd );
            break;
        }
        if ( 0 == r ) {
            sleep( 1 );
            continue;
        }
        if ( ! FD_ISSET( fd, & rfds ) ) {
            continue;
        }

        int navail = 0;
        r = ioctl( fd, FIONREAD, & navail );
        if ( -1 == r ) {
            perror("ioctl");
            break;
        }
        if ( 0 == navail ) {
            sleep( 1 );
            continue;
        }
        D( "connection %d has %d bytes available", fd, navail );

        if ( (size_t)navail > buf_size ) {
            D( "only reading %d bytes instead of %d", (int) buf_size, navail );
            navail = buf_size;
        }

        r = read( fd, & ( (uint8_t *) buf )[ o ], navail );
        if ( -1 == r ) {
            perror( "read" );
            return (ssize_t)r;
        }
        n = (size_t) r;
    }

    D("read %d bytes from connection %d", (int)o, fd );
    hexprint( buf, (size_t)o );

    return (ssize_t)o;
}

