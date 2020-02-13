#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "St060115.h"

using namespace ::std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace  ::org::misb;

int main(int argc, char **argv) {

    (void) argc;
    (void) argv;

    int r = 0;

    shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    St060115Client st060115Client(protocol);

    try {
        transport->open();
    } catch( ... ) {
        cout << "failed to open transport" << endl;
        return -1;
    }

    try {

        UasDataLinkLocalSet msg;
        // precisionTimeStamp is a required field
        msg.__set_precisionTimeStamp( -1 );
        msg.__set_alternatePlatformName( "Hello, from Thrift!" );

        st060115Client.update( msg );

    } catch ( ... ) {
        cout << "an exception was thrown" << endl;
    }
    transport->close();

    try {

        transport->open();
        UasDataLinkLocalSet msg;
        // oh, that's right! The checksum is a required field too
        msg.__set_checksum( -1 );
        msg.__set_precisionTimeStamp( -1 );
        // oh, that's right! The uasDatalinkLsVersionNumber is a required field too
        msg.__set_uasDatalinkLsVersionNumber(15);
        msg.__set_alternatePlatformName( "Hello, from C++!" );

        st060115Client.update( msg );

        cout << "no exception was thrown" << endl;

    } catch ( ... ) {
        cout << "an exception was thrown" << endl;
        r = -1;
    }

    return r;
}
