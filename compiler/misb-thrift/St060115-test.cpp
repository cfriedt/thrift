#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include <thrift/protocol/TMISBProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "St060115.h"

#undef DEBUG
#include "debug.h"

using namespace ::std;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::transport;

using namespace  ::org::misb;

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
          D() << "returning nullptr" << endl;
          return nullptr;
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


class St060115Test : public ::testing::Test {

public:
    ~St060115Test() = default;

protected:
    void SetUp() override {

        ready = false;
        processed = false;

        // the transport is shared between client & server
        memory = vector<uint8_t>( TMemoryBuffer::defaultSize, 0 );
        D() << "memory.front() is at address " << (void *)(&memory.front()) << endl;
        transport = make_shared<TMemoryBuffer>(&memory.front(), memory.size(), TMemoryBuffer::MemoryPolicy::TAKE_OWNERSHIP);

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

    void common() {
        expected_message.__set_precisionTimeStamp( expected_precisionTimeStamp );
        expected_message.__set_checksum( expected_checksum );
        expected_message.__set_uasDatalinkLsVersionNumber( expected_uasDatalinkLsVersionNumber );

        client->update( expected_message );
        ready = true;
        readyCv.notify_one();

        unique_lock<mutex> processedLock( processedMu );
        processedCv.wait( processedLock, [&](){ return processed; } );
        actual_message = handler->msg;

        actual_checksum = actual_message.checksum;
        actual_precisionTimeStamp = actual_message.precisionTimeStamp;
        actual_uasDatalinkLsVersionNumber = actual_message.uasDatalinkLsVersionNumber;

        ASSERT_EQ( actual_checksum, expected_checksum );
        ASSERT_EQ( actual_precisionTimeStamp, expected_precisionTimeStamp );
        ASSERT_EQ( actual_uasDatalinkLsVersionNumber, expected_uasDatalinkLsVersionNumber );
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

    vector<uint8_t> memory;
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
};
const uint16_t St060115Test::expected_checksum = 0xabcd;
const uint64_t St060115Test::expected_precisionTimeStamp = 0x0011223344556677;
const uint8_t St060115Test::expected_uasDatalinkLsVersionNumber = 15;

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

TEST_F( St060115Test, precisionTimeStamp_checksum_version ) {
    common();
}
