#include <iostream>
#include <unordered_set>

#include <thrift/protocol/TMISBProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "St0601.h"

#include "St0601Getter.h"

using namespace ::std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace  ::org::misb;

int main(int argc, char **argv) {

    (void) argc;
    (void) argv;

    int r = 0;

    // socket transport for demonstration purposes
    // but a memory buffer transport also works as well
    shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    shared_ptr<TTransport> transport(new TBufferedTransport(socket));


    shared_ptr<TProtocol> protocol(new TMISBProtocol(transport));
    St0601Client client(protocol);

    const St0601Getter getter;

    try {

        transport->open();
        UasDataLinkLocalSet msg;

        // order is specified by the position in st0601.thrift
        static const unordered_set<St0601Tag> tags {

            // required tags in every frame

            St0601Tag::ST_0601_CHECKSUM,
            St0601Tag::ST_0601_TIMESTAMP,
            St0601Tag::ST_0601_UAS_DATALINK_LS_VERSION_NUMBER,

            St0601Tag::ST_0601_ALTERNATE_PLATFORM_NAME,
        };

        getter.getTags(tags, msg);
        client.update( msg );

    } catch ( ... ) {
        cout << "an exception was thrown" << endl;
        r = -1;
    }

    return r;
}
