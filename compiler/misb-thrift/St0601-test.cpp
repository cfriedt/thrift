#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

#include "../../lib/cpp/src/thrift/protocol/ber.h"
#include "../../lib/cpp/src/thrift/protocol/beroid.h"
#include "../../lib/cpp/src/thrift/protocol/imap.h"
#include "../../lib/cpp/src/thrift/protocol/TMISBProtocol.h"
#include "../../lib/cpp/src/thrift/server/TSimpleServer.h"
#include "../../lib/cpp/src/thrift/transport/TSocket.h"
#include "../../lib/cpp/src/thrift/transport/TTransportUtils.h"
#include "../../lib/cpp/src/thrift/transport/TServerSocket.h"
#include "../../lib/cpp/src/thrift/transport/TBufferTransports.h"

// the thrift-generated protocol header
#include "St0601.h"

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

class St0601Handler : virtual public St0601If {
 public:

  UasDataLinkLocalSet msg;

  St0601Handler( condition_variable & c, mutex & m, bool & p ) : cv( c ), mu( m ), processed( p )  {
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

  /*
   * returns the exact same transport that is passed in
   */
  std::shared_ptr<TTransport> getTransport(std::shared_ptr<TTransport> trans) override {
    return trans;
  }
};


constexpr size_t TMemoryBufferDefaultSize = 1024;
class St0601Test : public ::testing::Test {

public:
    ~St0601Test() = default;

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
        handler = make_shared<St0601Handler>( processedCv, processedMu, processed );
        processor = make_shared<St0601Processor>(handler);
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
        client = make_shared<St0601Client>(protocol);
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
            D() << "key: " << beroid << " length: " << ber << " value: " << tagData << endl;

            tagOffset[ beroid ] = offset;
            tagSize[ beroid ] = beroidLen + berLen + ber;
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

    void validateBytes( unsigned tag, const vector<uint8_t> expected_v8 ) {

        ASSERT_NE( tagOffset.end(), tagOffset.find( tag ) );
        ASSERT_NE( tagSize.end(), tagSize.find( tag ) );

        size_t offs = tagOffset[ tag ];
        size_t sz = tagSize[ tag ];

        ASSERT_LE( offs + sz, TMemoryBufferDefaultSize );

        uintmax_t beroid;
        uintmax_t ber;
        size_t len;
        int r;

        r = ::berOidUintDecode( & memory[ offs ], sz, & beroid );
        ASSERT_NE( r, -1 );
        ASSERT_EQ( uintmax_t(tag), beroid );
        len = r;

        offs += len;
        sz -= len;

        r = ::berUintDecode( & memory[ offs ], sz, & ber );
        ASSERT_NE( r, -1 );
        ASSERT_EQ( uintmax_t(expected_v8.size()), ber );
        len = r;

        offs += len;
        sz -= len;

        // validate the remaining bytes
        vector<uint8_t> actual_v8 = vector<uint8_t>( & memory[ offs ], & memory[ offs ] + sz );
        EXPECT_EQ( actual_v8, expected_v8 );

        sz -= expected_v8.size();

        ASSERT_EQ( sz, 0 );
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

    shared_ptr<St0601Handler> handler;
    shared_ptr<TProcessor> processor;
    shared_ptr<TServerTransport> serverTransport;
    shared_ptr<TTransportFactory> transportFactory;
    shared_ptr<TProtocolFactory> protocolFactory;
    shared_ptr<TSimpleServer> server;
    thread serverThread;

    shared_ptr<TProtocol> protocol;
    shared_ptr<St0601Client> client;

    unordered_map<unsigned,size_t> tagOffset;
    unordered_map<unsigned,size_t> tagSize;
};
const uint16_t St0601Test::expected_checksum = 0xabcd;
const uint64_t St0601Test::expected_precisionTimeStamp = 0x0011223344556677;
const uint8_t St0601Test::expected_uasDatalinkLsVersionNumber = 15;

TEST_F( St0601Test, testMemoryWrite ) {
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

/*
 * Simply test the 3 required items within the UAS Datalink Local Set are sent,
 * are in the correct order, and have the correct binary encoding.
 */
TEST_F( St0601Test, requiredTags ) {
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

/*
 * Test that we can encode / decode a string tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, string ) {
    string expected_string( "Testing, Testing, 123" );

    expected_message.__set_platformDesignation( expected_string );
    ASSERT_TRUE( expected_message.__isset.platformDesignation );

    common();

    EXPECT_TRUE( actual_message.__isset.platformDesignation );
    string actual_string = actual_message.platformDesignation;

    EXPECT_EQ( actual_string, expected_string );

    validateBytes( St0601Tag::PLATFORM_DESIGNATION, to_vector( expected_string ) );
}

/*
 * Test that that the MaxLength annotation is respected for strings
 */
TEST_F( St0601Test, string_MaxLength_127 ) {
    string expected_string( 127, 'x' );

    expected_message.__set_platformDesignation( expected_string + expected_string );
    ASSERT_TRUE( expected_message.__isset.platformDesignation );

    common();

    EXPECT_TRUE( actual_message.__isset.platformDesignation );
    string actual_string = actual_message.platformDesignation;

    EXPECT_EQ( actual_string, expected_string );
}

// This test fails (predictably)
#if 0
/*
 * Test that that the FixedLength annotation is respected for strings
 * Note, 0601.15 does not currently support any fixed-length strings that
 * I'm aware of, so this test is only successful if you adjust st0601.thrift
 */
TEST_F( St0601Test, string_FixedLength_lt127 ) {
    string x( "x" );
    string expected_string = x + string( 126, '\0' );

    expected_message.__set_platformDesignation( x );
    ASSERT_TRUE( expected_message.__isset.platformDesignation );

    common();

    EXPECT_TRUE( actual_message.__isset.platformDesignation );
    string actual_string = actual_message.platformDesignation;

    EXPECT_EQ( actual_string, expected_string );
}
#endif

/*
 * Test that that the FixedLength annotation is respected for strings
 * Note, 0601.15 does not currently support any fixed-length strings that
 * I'm aware of, so this test is only successful if you adjust st0601.thrift
 */
TEST_F( St0601Test, string_FixedLength_gt127 ) {
    string expected_string( 127, 'x' );

    expected_message.__set_platformDesignation( expected_string + expected_string );
    ASSERT_TRUE( expected_message.__isset.platformDesignation );

    common();

    EXPECT_TRUE( actual_message.__isset.platformDesignation );
    string actual_string = actual_message.platformDesignation;

    EXPECT_EQ( actual_string, expected_string );
}

/*
 * Test that we can encode / decode a bool tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, bool ) {
    bool expected_bool = true;

    expected_message.__set_icingDetected( expected_bool );
    ASSERT_TRUE( expected_message.__isset.icingDetected );

    common();

    EXPECT_TRUE( actual_message.__isset.icingDetected );
    bool actual_bool = actual_message.icingDetected;

    EXPECT_EQ( actual_bool, expected_bool );

    const vector<uint8_t> expected_v8 {
		U8(expected_bool >> 0),
    };

    validateBytes( St0601Tag::ICING_DETECTED, expected_v8 );
}

/*
 * Test that we can encode / decode an i8 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, i8 ) {
    int8_t expected_int8 = -25;

    expected_message.__set_outsideAirTemperature( expected_int8 );
    ASSERT_TRUE( expected_message.__isset.outsideAirTemperature );

    common();

    EXPECT_TRUE( actual_message.__isset.outsideAirTemperature );
    int8_t actual_int8 = actual_message.outsideAirTemperature;

    EXPECT_EQ( actual_int8, expected_int8 );

    const vector<uint8_t> expected_v8 {
		U8(expected_int8 >> 0),
    };

    validateBytes( St0601Tag::OUTSIDE_AIR_TEMPERATURE, expected_v8 );
}

// FIXME: currently, the "Unsigned" annotation does nothing for integer values
// XXX: unsure if there are any signed 16-bit tags that we can test with

/*
 * Test that we can encode / decode an i16 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, i16 ) {

    int16_t expected_int16 = 0xaabb;

    expected_message.__set_weaponLoad( expected_int16 );
    ASSERT_TRUE( expected_message.__isset.weaponLoad );

    common();

    EXPECT_TRUE( actual_message.__isset.weaponLoad );
    int16_t actual_int16 = actual_message.weaponLoad;

    EXPECT_EQ( actual_int16, expected_int16 );

    const vector<uint8_t> expected_v8 {
		U8(expected_int16 >> 8),
		U8(expected_int16 >> 0),
    };

    validateBytes( St0601Tag::WEAPON_LOAD, expected_v8 );
}

/*
 * Test that we can encode / decode an i32 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, i32 ) {

    int32_t expected_int32 = 0xaabbccdd;

    expected_message.__set_leapSeconds( expected_int32 );
    ASSERT_TRUE( expected_message.__isset.leapSeconds );

    common();

    EXPECT_TRUE( actual_message.__isset.leapSeconds );
    int32_t actual_int32 = actual_message.leapSeconds;

    EXPECT_EQ( actual_int32, expected_int32 );

    const vector<uint8_t> expected_v8 {
		U8(expected_int32 >> 24),
		U8(expected_int32 >> 16),
		U8(expected_int32 >> 8),
		U8(expected_int32 >> 0),
    };

    validateBytes( St0601Tag::LEAP_SECONDS, expected_v8 );
}

/*
 * Test that we can encode / decode an i64 tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, i64 ) {

    int64_t expected_int64 = 0x1122334455667788;

    expected_message.__set_correctionOffset( expected_int64 );
    ASSERT_TRUE( expected_message.__isset.correctionOffset );

    common();

    EXPECT_TRUE( actual_message.__isset.correctionOffset );
    int64_t actual_int64 = actual_message.correctionOffset;

    EXPECT_EQ( actual_int64, expected_int64 );

    const vector<uint8_t> expected_v8 {
		U8(expected_int64 >> 56),
		U8(expected_int64 >> 48),
		U8(expected_int64 >> 40),
		U8(expected_int64 >> 32),
		U8(expected_int64 >> 24),
		U8(expected_int64 >> 16),
		U8(expected_int64 >> 8),
		U8(expected_int64 >> 0),
    };

    validateBytes( St0601Tag::CORRECTION_OFFSET, expected_v8 );
}

TEST_F( St0601Test, i64_VariableLength ) {

    // With variable-length integers (non-beroid, non-ber)
    // any zeros in the upper bytes are not transmitted.
    // In this case, we expect 1 byte for the zero value.
    int64_t expected_int64 = 0;

    expected_message.__set_correctionOffset( expected_int64 );
    ASSERT_TRUE( expected_message.__isset.correctionOffset );

    common();

    EXPECT_TRUE( actual_message.__isset.correctionOffset );
    int64_t actual_int64 = actual_message.correctionOffset;

    EXPECT_EQ( actual_int64, expected_int64 );

    const vector<uint8_t> expected_v8 { 0 };

    validateBytes( St0601Tag::CORRECTION_OFFSET, expected_v8 );
}

/*
 * Test that we can encode / decode an enum tag and that it is
 * binary-compatible with MISB
 */
TEST_F( St0601Test, enum ) {

    OperationalMode::type expected_operationalMode = OperationalMode::OPERATIONAL;

    expected_message.__set_operationalMode( expected_operationalMode );
    ASSERT_TRUE( expected_message.__isset.operationalMode );

    common();

    EXPECT_TRUE( actual_message.__isset.operationalMode );
    OperationalMode::type actual_operationalMode = actual_message.operationalMode;

    EXPECT_EQ( actual_operationalMode, expected_operationalMode );

    const vector<uint8_t> expected_v8 {
        U8(expected_operationalMode >> 0),
    };

    validateBytes( St0601Tag::OPERATIONAL_MODE, expected_v8 );
}

/*
 * Test that the IMAPA annotation correctly triggers IMAPA encode & decode, and
 * that the encoded / decoded values are correct using the platformCourseAngle()
 * method.
 *
 * IMAPA(0,360,0.016625)
 */
TEST_F( St0601Test, IMAPA ) {

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
    ASSERT_EQ( expected_v8.size(), expected_size );

    validateBytes( St0601Tag::PLATFORM_COURSE_ANGLE, expected_v8);
}

/*
 * Test that the IMAPB annotation correctly triggers IMAPB encode & decode, and
 * that the encoded / decoded values are correct using the alternatePlatformLongitude()
 * method.
 *
 * IMAPB(-180, 180, 4)
 */
TEST_F( St0601Test, IMAPB ) {

    const double lowerBound = -180;
    const double upperBound = 180;
    const size_t expected_size = 4;

    const double expected_double = 42;
    uintmax_t expected_uintmax = 0x37800000;

    size_t actual_size = ::imapBEncode(lowerBound, upperBound, expected_size, expected_double, & expected_uintmax);
    ASSERT_NE(int(actual_size), -1);
    ASSERT_EQ(actual_size, expected_size);

    expected_message.__set_alternatePlatformLongitude( expected_double );
    ASSERT_TRUE( expected_message.__isset.alternatePlatformLongitude );

    common();

    EXPECT_TRUE( actual_message.__isset.alternatePlatformLongitude );
    double actual_double = actual_message.alternatePlatformLongitude;

    EXPECT_NEAR( actual_double, expected_double, imapBEncodePrecision(lowerBound, upperBound, expected_size) );

    const vector<uint8_t> expected_v8 {
    	U8(expected_uintmax >> 24),
    	U8(expected_uintmax >> 16),
        U8(expected_uintmax >> 8),
        U8(expected_uintmax >> 0),
    };
    ASSERT_EQ( expected_v8.size(), expected_size );

    validateBytes( St0601Tag::ALTERNATE_PLATFORM_LONGITUDE, expected_v8);
}

/*
 * Test that nested local sets are properly encoded / decoded
 */
TEST_F( St0601Test, NestedStruct ) {

    SecurityLocalSet expected_securityLocalSet;

    // set required fields
    expected_securityLocalSet.__set_securityClassification( SecurityClassification::type::TOP_SECRET );
    expected_securityLocalSet.__set_classifyingCountryAndReleasingInstructionsCountryCodingMethod( ClassifyingCountryAndReleasingInstructionsCountryCodingMethod::type::ISO_3166_TWO_LETTER );
    expected_securityLocalSet.__set_classifyingCountry( "//CA" );
    expected_securityLocalSet.__set_objectCountryCodingMethod( ObjectCountryCodingMethod::type::ISO_3166_TWO_LETTER );
    expected_securityLocalSet.__set_objectCountryCodes( "CA" );
    expected_securityLocalSet.__set_version( 12 );

    expected_message.__set_securityLocalSet( expected_securityLocalSet );
    ASSERT_TRUE( expected_message.__isset.securityLocalSet );

    common();

    EXPECT_TRUE( actual_message.__isset.securityLocalSet );
    SecurityLocalSet actual_securityLocalSet = actual_message.securityLocalSet;

    const vector<uint8_t> expected_v8 {
        1,1,SecurityClassification::type::TOP_SECRET,
        2,1,ClassifyingCountryAndReleasingInstructionsCountryCodingMethod::type::ISO_3166_TWO_LETTER,
        3,4,'/','/','C','A',
        12,1,ObjectCountryCodingMethod::type::ISO_3166_TWO_LETTER,
        13,2,'C','A',
        22,2,0,12,
    };

    validateBytes( St0601Tag::SECURITY_LOCAL_SET, expected_v8);
}

/*
 * Test that DLP are properly encoded / decoded
 */
TEST_F( St0601Test, DLP ) {

    SensorFrameRatePack expected_sensorFrameRatePack;

    // Set mandatory fields
    expected_sensorFrameRatePack.__set_numerator(30);
    // This is kind of a tricky one, because the denominator is optional
    // In this case, the limiting factor of the deserializer is going to be
    // the reported length of the packet.

    expected_message.__set_sensorFrameRatePack( expected_sensorFrameRatePack );
    ASSERT_TRUE( expected_message.__isset.sensorFrameRatePack );

    common();

    EXPECT_TRUE( actual_message.__isset.sensorFrameRatePack );
    SensorFrameRatePack actual_sensorFrameRatePack = actual_message.sensorFrameRatePack;

    const vector<uint8_t> expected_v8 {
        30,
    };

    validateBytes( St0601Tag::SENSOR_FRAME_RATE_PACK, expected_v8);
}

/*
 * Test that DLP are properly encoded / decoded
 */
TEST_F( St0601Test, DLP2 ) {

    SensorFrameRatePack expected_sensorFrameRatePack;

    expected_sensorFrameRatePack.__set_numerator(30);
    expected_sensorFrameRatePack.__set_denominator(1);

    expected_message.__set_sensorFrameRatePack( expected_sensorFrameRatePack );
    ASSERT_TRUE( expected_message.__isset.sensorFrameRatePack );

    common();

    EXPECT_TRUE( actual_message.__isset.sensorFrameRatePack );
    SensorFrameRatePack actual_sensorFrameRatePack = actual_message.sensorFrameRatePack;

    const vector<uint8_t> expected_v8 {
        30,
        1,
    };

    validateBytes( St0601Tag::SENSOR_FRAME_RATE_PACK, expected_v8);
}

/*
 * Test that the VMTI Local Set is properly encoded / decoded
 *
 * Specifically, the VTarget Pack with its irregular VTarget ID
 * field that has no key or length but is BER-OID encoded.
 */
TEST_F( St0601Test, VMTILocalSetVTargetPack ) {

    const string sourceSensor("The Eye of Sauron");
    const string vmtiAlgorithm("Bogosort");

    VMTILocalSet expected_vmtiLocalSet;

    expected_vmtiLocalSet.__set_totalNumberOfTargetsDetectedInTheFrame(1);
    expected_vmtiLocalSet.__set_numberOfReportedTargets(1);
    expected_vmtiLocalSet.__set_motionImageryFrameNumber(0);
    expected_vmtiLocalSet.__set_vmtiSourceSensor(sourceSensor);

    VTargetPack tgt;
    tgt.__set_targetIdNumber(144); // use the BER-OID example from MISB
    tgt.__set_targetCentroidPixelNumber(0);
    tgt.__set_targetCentroidPixelNumber(0);
    tgt.__set_boundingBoxTopLeftPixelNumber(0);
    tgt.__set_boundingBoxBottomRightPixelNumber(0);
    tgt.__set_targetCentroidPixelRow(0);
    tgt.__set_targetCentroidPixelColumn(0);

    VTrackerLocalSet trk;

    trk.__set_algorithm(vmtiAlgorithm);
    tgt.__set_vTrackerLs(trk);

    expected_vmtiLocalSet.__set_vTargetSeries( tgt );

    expected_message.__set_vmtiLocalSet( expected_vmtiLocalSet );
    ASSERT_TRUE( expected_message.__isset.vmtiLocalSet );

    common();

    EXPECT_TRUE( actual_message.__isset.vmtiLocalSet );
    VMTILocalSet actual_vmtiLocalSet = actual_message.vmtiLocalSet;

    auto append = []( vector<uint8_t> & a, const vector<uint8_t> & b ) {
      copy( begin(b), end( b ), back_inserter( a ) );
    };
    auto sappend = []( vector<uint8_t> & a, const string & s ) {
      for( auto & c: s ) {
          a.push_back(uint8_t(c));
      }
    };

    vector<uint8_t> vmtiLocalSetData;

    // total number of targets detected
    append(vmtiLocalSetData, vector<uint8_t>({ 5, 1, 1 }));
    // number of reported targets
    append(vmtiLocalSetData, vector<uint8_t>({ 6, 1, 1 }));
    // motion imagery frame number
    append(vmtiLocalSetData, vector<uint8_t>({ 7, 1, 0 }));
    // vmti source sensor
    vmtiLocalSetData.push_back(10);
    append(vmtiLocalSetData, vector<uint8_t>({uint8_t(sourceSensor.size())}));
    sappend(vmtiLocalSetData, sourceSensor);

    vector<uint8_t> vtargetSeriesData;
    // target id number. This is the BER-OID encoding of decimal 144 from MISB
    append(vtargetSeriesData, vector<uint8_t>({0x81, 0x10}));
    // target centroid pixel number
    append(vtargetSeriesData, vector<uint8_t>({1, 1, 0}));
    // bounding box top left pixel number
    append(vtargetSeriesData, vector<uint8_t>({2, 1, 0}));
    // bounding box bottom right pixel number
    append(vtargetSeriesData, vector<uint8_t>({3, 1, 0}));
    // target centroid pixel row
    append(vtargetSeriesData, vector<uint8_t>({19, 1, 0}));
    // target centroid pixel column
    append(vtargetSeriesData, vector<uint8_t>({20, 1, 0}));

    vector<uint8_t> vtrackerLocalSetData;
    // vmti algorithm
    vtrackerLocalSetData.push_back(6);
    vtrackerLocalSetData.push_back(vmtiAlgorithm.size());
    sappend(vtrackerLocalSetData, vmtiAlgorithm);

    vtargetSeriesData.push_back(104);
    vtargetSeriesData.push_back(vtrackerLocalSetData.size());
    append(vtargetSeriesData, vtrackerLocalSetData);

    vmtiLocalSetData.push_back(101);
    vmtiLocalSetData.push_back(vtargetSeriesData.size());
    append(vmtiLocalSetData, vtargetSeriesData);

    const vector<uint8_t> expected_v8 = vmtiLocalSetData;

    validateBytes( St0601Tag::VMTI_LOCAL_SET, expected_v8);
}
