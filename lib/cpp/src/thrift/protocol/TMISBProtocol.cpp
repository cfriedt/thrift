#include "ber.h"
#include "beroid.h"
#include "TMISBProtocol.h"

namespace apache {
namespace thrift {
namespace protocol {

size_t readBeroid(::apache::thrift::protocol::TProtocol* iprot, uintmax_t & r) {

    uint8_t beroid[ BEROID_MAX_BYTES ];
    int8_t byte;
    uint8_t nBytes;
    bool moreBytes;

    for( nBytes = 0, moreBytes = true; moreBytes && nBytes < sizeof(beroid); ++nBytes, moreBytes = !!(BEROID_BYTE_MAX & byte) ) {
        iprot->readByte(byte);
        beroid[ nBytes ] = uint8_t(byte);
    }

    ::berOidUintDecode( beroid, nBytes, & r );

    return nBytes;
}

size_t writeBeroid(::apache::thrift::protocol::TProtocol* oprot, const uintmax_t & x) {

    uint8_t beroid[ BEROID_MAX_BYTES ];

    int nBytes = ::berOidUintEncode( x, beroid, sizeof(beroid) );
    for( int i = 0; i < nBytes; ++i ) {
        oprot->writeByte( beroid[ i ] );
}

    return size_t( nBytes );
}

size_t readBer(::apache::thrift::protocol::TProtocol* iprot, uintmax_t & r) {
    size_t nbytes;
    int8_t byte;
    uint8_t ber[ BER_MAX_BYTES ];

    using ::apache::thrift::protocol::TProtocolException;

    iprot->readByte(byte);
    ber[ 0 ] = byte;

    if ( ber[0] & BER_LONG_FORM_MASK ) {
        nbytes = ber[0] & (~BER_LONG_FORM_MASK);
        if ( nbytes > sizeof( ber ) ) {
            throw TProtocolException(TProtocolException::INVALID_DATA);
        }
    } else {
        nbytes = 1;
    }
    for( size_t i = 1; i < nbytes; ++i ) {
        iprot->readByte(byte);
        ber[i] = byte;
    }
    if ( -1 == ::berUintDecode( ber, sizeof(ber), & r ) ) {
        throw TProtocolException(TProtocolException::INVALID_DATA);
    }

    return nbytes;
}

size_t writeBer(::apache::thrift::protocol::TProtocol* oprot, const uintmax_t & x) {
    uint8_t ber[ BER_MAX_BYTES ];

    size_t berLen = ::berUintEncode(x, ber, sizeof(ber));
    for(size_t i = 0; i < berLen; ++i) {
        oprot->writeByte(ber[i]);
    }

    return berLen;
}

size_t writeVariableLengthInteger(::apache::thrift::protocol::TProtocol* oprot, const size_t & maxLength, const uintmax_t & _x) {
    size_t r;

    using ::apache::thrift::protocol::TProtocolException;

    if ( 0 == maxLength || maxLength > sizeof(int64_t) ) {
        throw TProtocolException(TProtocolException::INVALID_DATA);
    }

    uintmax_t mask;
    if ( 8 == maxLength ) {
        mask = -1;
    } else {
        mask = (1ULL << (8 * maxLength)) - 1;
    }

    uintmax_t x = _x & mask;
    if ( 0 == x ) {
        if ( nullptr != oprot ) {
            oprot->writeByte(0);
        }
        return 1;
    }

    r = 0;
    bool foundNonZero = false;
    for( size_t i = maxLength; i > 0; --i ) {
        uint8_t byte = (x >> ((i - 1) * 8));
        if ( ! foundNonZero && 0 == byte ) {
            continue;
        }
        foundNonZero = true;
        if ( nullptr == oprot ) {
            r++;
        } else {
            r += oprot->writeByte(byte);
        }
    }

    return r;
}

size_t readVariableLengthInteger(::apache::thrift::protocol::TProtocol* iprot, const size_t & maxLength, uintmax_t & x) {
    size_t r;

    using ::apache::thrift::protocol::TProtocolException;

    if ( 0 == maxLength || maxLength > sizeof(int64_t) ) {
        throw TProtocolException(TProtocolException::INVALID_DATA);
    }

    x = 0;
    r = 0;
    for( size_t i = 0; i < maxLength; ++i ) {
        int8_t byte;
        iprot->readByte(byte);
        x <<= 8;
        x |= uint8_t(byte);
        r++;
    }

    return r;
}

}
}
} // apache::thrift::protocol
