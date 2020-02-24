#include "ber.h"
#include "TMISBProtocol.h"

namespace apache {
namespace thrift {
namespace protocol {

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

size_t writeBer(::apache::thrift::protocol::TProtocol* oprot, const uintmax_t & len) {
    uint8_t ber[ BER_MAX_BYTES ];

    size_t berLen = ::berUintEncode(len, ber, sizeof(ber));
    for(size_t i = 0; i < berLen; ++i) {
        oprot->writeByte(ber[i]);
    }

    return berLen;
}


}
}
} // apache::thrift::protocol
