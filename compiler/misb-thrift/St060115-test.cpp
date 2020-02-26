#include <cstdint>
#include <cstdlib>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include <gtest/gtest.h>

#include "../../lib/cpp/src/thrift/protocol/ber.h"
#include "../../lib/cpp/src/thrift/protocol/beroid.h"
#include "../../lib/cpp/src/thrift/protocol/TMISBProtocol.h"
#include "../../lib/cpp/src/thrift/server/TSimpleServer.h"
#include "../../lib/cpp/src/thrift/transport/TSocket.h"
#include "../../lib/cpp/src/thrift/transport/TTransportUtils.h"
#include "../../lib/cpp/src/thrift/transport/TServerSocket.h"
#include "../../lib/cpp/src/thrift/transport/TBufferTransports.h"

// the thrift-generated protocol header
#include "St060115.h"

#include "imap.h"

// just some header files for testing
#undef DEBUG
#include "debug.h"

#define U8(x) (uint8_t((x) & 0xff))

using namespace ::std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::transport;

using namespace  ::org::misb;

template <typename T>
string to_string( const vector<T> & v ) {
    stringstream ss;
    ss << "{ ";
    for( auto & vv: v ) {
        ss << hex << setw( 8*sizeof(T)/4 ) << setfill('0') << uintmax_t( vv ) << " ";
    }
    ss << "}";
    return ss.str();
}

template <typename T>
ostream & operator<<( ostream & os, const vector<T> & v ) {
    os << to_string( v );
    return os;
}

// puts the contents of t into a vector in big-endian format
template <typename T>
vector<uint8_t> to_vector( const T & t ) {
    vector<uint8_t> r;
    for( int i = int(sizeof(T)) - 1; i >= 0; --i ) {
        r.push_back( uint8_t(t >> 8 * i) );
    }
    return r;
}

template <>
vector<uint8_t> to_vector( const string & t ) {
    vector<uint8_t> r;
    for ( auto & c: t ) {
        r.push_back( uint8_t( c ) );
    }
    return r;
}

class TServerSharedTransport : public ::apache::thrift::server::TServerTransport {
public:
  TServerSharedTransport() : shouldExit( false ), cv( defaultCv ), mu( defaultM ), ready( defaultR ) {}
  TServerSharedTransport( shared_ptr<TTransport> t, mutex &m, condition_variable &c, bool &r )
  :
      shouldExit( false ),
      cv( c ),
      mu( m ),
      ready( r ),
      trans( t )
  {
  }
  ~TServerSharedTransport() = default;
  virtual void close() override {
    D() << "close() called" << endl;
  }

  virtual void listen() override {
    D() << "listen() called" << endl;
  }

  virtual void interrupt() {
    D() << "interrupt() called" << endl;
    shouldExit = true;
  }

  virtual void interruptChildren() {
    D() << "interruptChildren() called" << endl;
  }

  virtual THRIFT_SOCKET getSocketFD() {
    D() << "getSocketFD() called" << endl;
    return -1;
  }

protected:

  bool shouldExit;
  condition_variable &cv;
  mutex &mu;
  bool &ready;
  shared_ptr<TTransport> trans;

  static condition_variable defaultCv;
  static mutex defaultM;
  static bool defaultR;

  virtual std::shared_ptr<TTransport> acceptImpl() override {
      unique_lock<mutex> lock( mu );
      D() << "waiting for ready.." << endl;
      cv.wait( lock, [&](){ return this->ready; } );
      if ( shouldExit ) {
          D() << "should exit" << endl;
          throw TTransportException( TTransportException::TTransportExceptionType::INTERRUPTED );
      }
      D() << "ready!" << endl;
      // clear the ready flag so that the server blocks properly
      ready = false;
      lock.unlock();
      return trans;
  }
};
condition_variable TServerSharedTransport::defaultCv;
mutex TServerSharedTransport::defaultM;
bool TServerSharedTransport::defaultR;

class St060115Handler : virtual public St060115If {
 public:

  UasDataLinkLocalSet msg;

  St060115Handler( condition_variable & c, mutex & m, bool & p ) : cv( c ), mu( m ), processed( p )  {
  }

  void update(const UasDataLinkLocalSet& message) {
      unique_lock<mutex> lock( mu );

      msg = message;

      processed = true;
      cv.notify_one();
      lock.unlock();
  }

protected:
  condition_variable & cv;
  mutex & mu;
  bool & processed;
};

class TSimpleTransportFactory : public TTransportFactory {
public:
  TSimpleTransportFactory() = default;

  ~TSimpleTransportFactory() override = default;

  /**
   * returns the exact same transport that is passed in
   */
  std::shared_ptr<TTransport> getTransport(std::shared_ptr<TTransport> trans) override {
    return trans;
  }
};


constexpr size_t TMemoryBufferDefaultSize = 1024;
class St060115Test : public ::testing::Test {

public:
    ~St060115Test() = default;

protected:
    void SetUp() override {

        ready = false;
        processed = false;

        // the transport is shared between client & server, but is owned and freed by Thrift
        // TMemoryBuffer::defaultSize has no actual storage
        memory = (uint8_t *)calloc( 1, TMemoryBufferDefaultSize );
        D() << "memory is at address " << (void *)memory << endl;
        transport = make_shared<TMemoryBuffer>(memory, TMemoryBufferDefaultSize, TMemoryBuffer::MemoryPolicy::TAKE_OWNERSHIP);

        // first set up the server
        handler = make_shared<St060115Handler>( processedCv, processedMu, processed );
        processor = make_shared<St060115Processor>(handler);
        serverTransport = make_shared<TServerSharedTransport>(transport, readyMu, readyCv, ready);
        transportFactory = make_shared<TSimpleTransportFactory>();
        protocolFactory = make_shared<TMISBProtocolFactory>();
        server = make_shared<TSimpleServer>(processor, serverTransport, transportFactory, protocolFactory);
        serverThread = thread([&]() {
            D() << "about to call server.serve()" << endl;
            server->serve();
            D() << "returned from server.serve()" << endl;
        });

        // then set up the client
        protocol = make_shared<TMISBProtocol>(transport);
        client = make_shared<St060115Client>(protocol);
    }

    void TearDown() override {
        D() << "about to call server.stop()" << endl;
        server->stop();
        readyMu.lock();
        ready = true;
        readyCv.notify_one();
        readyMu.unlock();
        D() << "returned from server.stop()" << endl;

        if ( serverThread.joinable() ) {
            D() << "about to call serverThread.join()" << endl;
            serverThread.join();
            D() << "returned from serverThread.join()" << endl;
        }
    }

    void tagify() {



        for( size_t remaining = TMemoryBufferDefaultSize, offset = 0, consumed = 0; remaining > 0; offset += consumed, remaining -= consumed ) {

            size_t beroidLen;
            uintmax_t beroid;
            size_t berLen;
            uintmax_t ber;

            int r;

            consumed = 0;

            r = ::berOidUintDecode( & memory[ offset ], remaining, & beroid );
            if ( -1 == r || 0 == beroid ) {
                break;
            }
            beroidLen = (size_t)r;
            consumed += beroidLen;

            r = ::berUintDecode( & memory[ offset + beroidLen ], remaining - beroidLen, & ber );
            if ( -1 == r || 0 == ber ) {
                break;
            }
            berLen = (size_t)r;
            consumed += berLen;

            consumed += ber;

            vector<uint8_t> tagData = vector<uint8_t>( & memory[ offset + beroidLen + berLen ], & memory[ offset + beroidLen + berLen ] + ber );

            cout << "key: " << beroid << " length: " << ber << " value: " << tagData << endl;

            tagOffset[ beroid ] = offset;
            tagSize[ beroid ] = ber;
        }
    }

    void common() {
        expected_message.__set_precisionTimeStamp( expected_precisionTimeStamp );
        expected_message.__set_checksum( expected_checksum );
        expected_message.__set_uasDatalinkLsVersionNumber( expected_uasDatalinkLsVersionNumber );

        D() << "calling update" << endl;
        client->update( expected_message );
        D() << "returned from update" << endl;
        ready = true;
        readyCv.notify_one();

        unique_lock<mutex> processedLock( processedMu );
        processedCv.wait( processedLock, [&](){ return processed; } );
        actual_message = handler->msg;

        // XXX: would be ideal if isset items were still populated for required fields
        //ASSERT_TRUE( actual_message.__isset.checksum );
        //ASSERT_TRUE( actual_message.__isset.precisionTimeStamp );
        //ASSERT_TRUE( actual_message.__isset.uasDatalinkLsVersionNumber );

        actual_checksum = actual_message.checksum;
        actual_precisionTimeStamp = actual_message.precisionTimeStamp;
        actual_uasDatalinkLsVersionNumber = actual_message.uasDatalinkLsVersionNumber;

        ASSERT_EQ( actual_checksum, expected_checksum );
        ASSERT_EQ( actual_precisionTimeStamp, expected_precisionTimeStamp );
        ASSERT_EQ( actual_uasDatalinkLsVersionNumber, expected_uasDatalinkLsVersionNumber );

        tagify();
    }

    void validateBytes( unsigned tag, unsigned len, const vector<uint8_t> expected_bytes ) {
        vector<uint8_t> expected_v8;
        vector<uint8_t> actual_v8;
        int r;

        size_t offs = 0;
        size_t sz = 0;

        // get the offset of the first byte of the specified tag
        size_t tagOffs = tagOffset[ tag ];
        size_t tagSz = tagSize[ tag ];

        offs += tagOffs;
        sz += tagSz;

        // validate the BER-OID encoded key
        size_t beroidSize = ::berOidUintEncodeLength( tag );
        ASSERT_NE( beroidSize, size_t(-1) );
        expected_v8 = vector<uint8_t>( beroidSize );
        r = ::berOidUintEncode( tag, & expected_v8.front(), expected_v8.size() );
        ASSERT_NE( r, -1 );
        actual_v8 = vector<uint8_t>( & memory[ offs ], & memory[ offs ] + min( beroidSize, sz ) );
        EXPECT_EQ( actual_v8, expected_v8 );

        offs += beroidSize;
        sz -= min( sz, beroidSize );

        // validate the BER encoded length
        size_t berSize = ::berUintEncodeLength( len );
        ASSERT_NE( berSize, size_t(-1) );
        expected_v8 = vector<uint8_t>( berSize );
        r = ::berUintEncode( len, & expected_v8.front(), expected_v8.size() );
        ASSERT_NE( r, -1 );
        actual_v8 = vector<uint8_t>( & memory[ offs ], & memory[ offs ] + min( berSize, sz ) );
        EXPECT_EQ( actual_v8, expected_v8 );

        offs += berSize;
        sz -= min( sz, berSize );

        // validate the remaining bytes
        actual_v8 = vector<uint8_t>( & memory[ offs ], & memory[ offs ] + min( sz, (size_t)len ) );

        EXPECT_EQ( actual_v8, expected_bytes );
    }

    static const uint16_t expected_checksum;
    uint16_t actual_checksum;

    static const uint64_t expected_precisionTimeStamp;
    uint64_t actual_precisionTimeStamp;

    static const uint8_t expected_uasDatalinkLsVersionNumber;
    uint8_t actual_uasDatalinkLsVersionNumber;

    UasDataLinkLocalSet expected_message;
    UasDataLinkLocalSet actual_message;

    // for synchronizing with server transport
    mutex readyMu;
    condition_variable readyCv;
    bool ready;

    // for synchronizing with handler
    condition_variable processedCv;
    mutex processedMu;
    bool processed;

    uint8_t *memory;
    shared_ptr<TTransport> transport;

    shared_ptr<St060115Handler> handler;
    shared_ptr<TProcessor> processor;
    shared_ptr<TServerTransport> serverTransport;
    shared_ptr<TTransportFactory> transportFactory;
    shared_ptr<TProtocolFactory> protocolFactory;
    shared_ptr<TSimpleServer> server;
    thread serverThread;

    shared_ptr<TProtocol> protocol;
    shared_ptr<St060115Client> client;

    unordered_map<unsigned,size_t> tagOffset;
    unordered_map<unsigned,size_t> tagSize;
};
const uint16_t St060115Test::expected_checksum = 0xabcd;
const uint64_t St060115Test::expected_precisionTimeStamp = 0x0011223344556677;
const uint8_t St060115Test::expected_uasDatalinkLsVersionNumber = 15;


#if 0
TEST_F( St060115Test, testMemoryWrite ) {
    expected_message.__set_precisionTimeStamp( expected_precisionTimeStamp );
    expected_message.__set_checksum( expected_checksum );
    expected_message.__set_uasDatalinkLsVersionNumber( expected_uasDatalinkLsVersionNumber );

    client->update( expected_message );

    EXPECT_NE( 0, memory[ 0 ] );
}

/*
 * Each of the integration tests below can be used to verify
 *
 * 1) message is binary-compatible with MISB
 * 2) generated encoder works as expected
 * 3) generated decoder works as expected
 */

/**
 * Simply test the 3 required items within the UAS Datalink Local Set are sent,
 * are in the correct order, and have the correct binary encoding.
 */
TEST_F( St060115Test, requiredTags ) {
    // if we return from this function, then the decoder is verified to work for
    // the three named fields. Note: we are not verifying the checksum is calculated
    // correctly, just that the value in the tag is correctly serialized / deserialized.
    common();

    const uint64_t ts = expected_precisionTimeStamp;
    const uint8_t v = expected_uasDatalinkLsVersionNumber;
    const uint16_t cs = expected_checksum;

    // Now, we check to ensure that the fields are correctly encoded on the wire
    // These are 1 byte keys because they are <= 127, so we can short-hand it
    // because this is not production code and is a rather contrived test, but
    // otherwise, we should properly get the BER-OID key, and BER lengths
    vector<uint8_t> expected_v8 = {
        // precisionTimeStamp MUST come first!
        2, 8, U8(ts >> ( 7 * 8 )), U8(ts >> ( 6 * 8 )), U8(ts >> ( 5 * 8 )), U8(ts >> ( 4 * 8 )), U8(ts >> ( 3 * 8 )), U8(ts >> ( 2 * 8 )), U8(ts >> ( 1 * 8 )), U8(ts >> ( 0 * 8 )),

        // version is the only other tag being transmitted
        65, 1, v >> ( 0 * 8 ),

        // checksum MUST come last!
        1, 2, U8(cs >> ( 1 * 8 )), U8(cs >> ( 0 * 8 )),
    };

    vector<uint8_t> actual_v8( memory, memory + expected_v8.size() );

    EXPECT_EQ( actual_v8, expected_v8 );
}

/**
 * Test that we can encode / decode a string tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, string ) {
    string expected_string( "Testing, Testing, 123" );

    expected_message.__set_platformDesignation( expected_string );
    ASSERT_TRUE( expected_message.__isset.platformDesignation );

    common();

    EXPECT_TRUE( actual_message.__isset.platformDesignation );
    string actual_string = actual_message.platformDesignation;

    EXPECT_EQ( actual_string, expected_string );

    //validateBytes( St060115Tag::PLATFORM_DESIGNATION, expected_string.size(), to_vector( expected_string ) );
}

/**
 * Test that we can encode / decode a bool tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, bool ) {
    bool expected_bool = true;

    expected_message.__set_icingDetected( expected_bool );
    ASSERT_TRUE( expected_message.__isset.icingDetected );

    common();

    EXPECT_TRUE( actual_message.__isset.icingDetected );
    bool actual_bool = actual_message.icingDetected;

    EXPECT_EQ( actual_bool, expected_bool );

    //validateBytes( St060115Tag::ICING_DETECTED, 1, to_vector( expected_bool ) );
}

/**
 * Test that we can encode / decode an i8 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, i8 ) {
    int8_t expected_int8 = -25;

    expected_message.__set_outsideAirTemperature( expected_int8 );
    ASSERT_TRUE( expected_message.__isset.outsideAirTemperature );

    common();

    EXPECT_TRUE( actual_message.__isset.outsideAirTemperature );
    int8_t actual_int8 = actual_message.outsideAirTemperature;

    EXPECT_EQ( actual_int8, expected_int8 );

    //validateBytes( St060115Tag::ICING_DETECTED, 1, to_vector( expected_int8 ) );
}

// FIXME: currently, the "Unsigned" annotation does nothing for integer values
// XXX: unsure if there are any signed 16-bit tags that we can test with

/**
 * Test that we can encode / decode an i16 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, i16 ) {

    int16_t expected_int16 = 0xaabb;

    expected_message.__set_weaponLoad( expected_int16 );
    ASSERT_TRUE( expected_message.__isset.weaponLoad );

    common();

    EXPECT_TRUE( actual_message.__isset.weaponLoad );
    int16_t actual_int16 = actual_message.weaponLoad;

    EXPECT_EQ( actual_int16, expected_int16 );

    //validateBytes( St060115Tag::WEAPON_LOAD, 1, to_vector( expected_int16 ) );
}

/**
 * Test that we can encode / decode an i32 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, i32 ) {

    int32_t expected_int32 = 0xaabbccdd;

    expected_message.__set_leapSeconds( expected_int32 );
    ASSERT_TRUE( expected_message.__isset.leapSeconds );

    common();

    EXPECT_TRUE( actual_message.__isset.leapSeconds );
    int32_t actual_int32 = actual_message.leapSeconds;

    EXPECT_EQ( actual_int32, expected_int32 );

//    validateBytes( St060115Tag::LEAP_SECONDS, 1, to_vector( expected_int32 ) );
}

/**
 * Test that we can encode / decode an i64 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, i64 ) {

    int64_t expected_int64 = 0x0011223344556677;

    expected_message.__set_correctionOffset( expected_int64 );
    ASSERT_TRUE( expected_message.__isset.correctionOffset );

    common();

    EXPECT_TRUE( actual_message.__isset.correctionOffset );
    int64_t actual_int64 = actual_message.correctionOffset;

    EXPECT_EQ( actual_int64, expected_int64 );

//    validateBytes( St060115Tag::CORRECTION_OFFSET, 1, to_vector( expected_int64 ) );
}

/**
 * Test that we can encode / decode an enum tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St060115Test, enum ) {

    OperationalMode::type expected_operationalMode = OperationalMode::OPERATIONAL;

    expected_message.__set_operationalMode( expected_operationalMode );
    ASSERT_TRUE( expected_message.__isset.operationalMode );

    common();

    EXPECT_TRUE( actual_message.__isset.operationalMode );
    OperationalMode::type actual_operationalMode = actual_message.operationalMode;

    EXPECT_EQ( actual_operationalMode, expected_operationalMode );

    //validateBytes( St060115Tag::OPERATIONAL_MODE, 1, to_vector( expected_operationalMode ) );
}
#endif

/**
 * Test that the IMAPA annotation correctly triggers IMAPA encode & decode, and
 * that the encoded / decoded values are correct using the platformCourseAngle()
 * method.
 *
 * IMAPA(0,360,0.016625)
 */
TEST_F( St060115Test, IMAPA ) {

    const double lowerBound = 0;
    const double upperBound = 360;
    const double precision = 0.016625;

    const double expected_double = 42;
    uintmax_t expected_uintmax = 0x0a80;

    const size_t expected_size = 2;
    size_t actual_size = ::imapAEncodeLength(lowerBound, upperBound, precision);
    ASSERT_NE(int(actual_size), -1);
    ASSERT_EQ(actual_size, expected_size);

    actual_size = ::imapAEncode(lowerBound, upperBound, precision, expected_double, & expected_uintmax);
    ASSERT_NE(int(actual_size), -1);
    ASSERT_EQ(actual_size, expected_size);

    expected_message.__set_platformCourseAngle( expected_double );
    ASSERT_TRUE( expected_message.__isset.platformCourseAngle );

    common();

    EXPECT_TRUE( actual_message.__isset.platformCourseAngle );
    double actual_double = actual_message.platformCourseAngle;

    EXPECT_NEAR( actual_double, expected_double, precision );

    const vector<uint8_t> expected_v8 {
        U8(expected_uintmax >> 8),
        U8(expected_uintmax >> 0),
    };

    validateBytes( St060115Tag::PLATFORM_COURSE_ANGLE, expected_size, expected_v8);
}
