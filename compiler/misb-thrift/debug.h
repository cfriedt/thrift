#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

#if defined( DEBUG )
#define D() clog
#else
class NullBuffer : public ::std::streambuf {
public:
    int overflow( int c ) { return c; }
};
static NullBuffer null_buffer;
static std::ostream null_stream( & null_buffer );
#define D() null_stream
#endif

#endif /* DEBUG_H_ */
