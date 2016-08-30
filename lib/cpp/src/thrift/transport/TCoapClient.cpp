/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <iostream>

#include <limits>
#include <cstdlib>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <thrift/transport/TUdpSocket.h>
#include <thrift/transport/TCoapClient.h>

#include <cantcoap/cantcoap.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapClient::TCoapClient( boost::shared_ptr<TTransport> transport, std::string host, std::string path )
:
	TCoapTransport( transport ),
	host_( host ),
	path_( path ),
    message_type( CoapPDU::COAP_CONFIRMABLE ),
    method_type( CoapPDU::COAP_GET )
{
}

TCoapClient::TCoapClient( string host, int port, string path )
:
	TCoapTransport( boost::shared_ptr<TTransport>( new TUdpSocket( host, port ) ) ),
    host_(host),
    path_(path),
    message_type( CoapPDU::COAP_CONFIRMABLE  ),
    method_type( CoapPDU::COAP_GET )
{
}

TCoapClient::TCoapClient( string host, int server_port, int client_port, string path )
:
	TCoapTransport( boost::shared_ptr<TTransport>( new TUdpSocket( host, server_port, client_port ) ) ),
    host_(host),
    path_(path),
    message_type( CoapPDU::COAP_CONFIRMABLE  ),
    method_type( CoapPDU::COAP_GET )
{
}

TCoapClient::~TCoapClient() {
}

void TCoapClient::open() {
	TCoapTransport::open();
	//coap_register_response_handler( getCoapContext().get(), coap_response_handler );
}

void TCoapClient::flush() {

	uint8_t *write_buffer;
	unsigned write_buffer_sz;

	uint8_t *pdu_ptr;
	unsigned pdu_len;

	uint64_t token;
	uint8_t  token_len = sizeof( uint64_t );

	CoapPDU pdu;

	writeBuffer_.getBuffer( & write_buffer, & write_buffer_sz );

	if ( 0 == write_buffer_sz ) {
		goto out;
	}

	pdu.setVersion( 1 );
	pdu.setType( (CoapPDU::Type) getCoapMessageType() );
	pdu.setCode( (CoapPDU::Code) getCoapMethodType() );
	pdu.setMessageID( ::random() );
	token = ::random();
	pdu.setToken( (uint8_t *) & token, token_len );
	pdu.setURI( (char *) path_.c_str() );
	pdu.setPayload( write_buffer, write_buffer_sz );

	if ( ! pdu.validate() ) {
		throw TTransportException( TTransportException::CORRUPTED_DATA, "unable to validate CoAP PDU!!" );
	}

	pdu_ptr = pdu.getPDUPointer();
	pdu_len = pdu.getPDULength();

	transport_->write( pdu_ptr, pdu_len );
	transport_->flush();

	writeBuffer_.resetBuffer();

out:
	return;
}

int TCoapClient::getCoapMessageType()
{
	return message_type;
}
void
TCoapClient::setCoapMessageType( int message_type )
{
	this->message_type = message_type;
}

int
TCoapClient::getCoapMethodType()
{
	return method_type;
}
void TCoapClient::setCoapMethodType( int method_type )
{
	this->method_type = method_type;
}

/*
void TCoapClient::sendBufferedRequest() {
	uint8_t *buf;
	uint32_t len;
	coap_uri_t uri;
	coap_pdu_t *request;
	bool split_uri_failed;
	coap_context_t *ctx;
	std::string server_uri;
	int send_r;
	bool add_data_success;
	bool add_option_success;

	coap_address_t dst_addr;

	sockaddr *sock_p;
	socklen_t sock_len;
	boost::shared_ptr<TSocket> socket_trans;

	ctx = getCoapContext().get();

	if ( NULL == ctx ) {
		return;
	}

	socket_trans = boost::static_pointer_cast<TSocket>( transport_ );
	sock_p = socket_trans->getCachedAddress( & sock_len );
	if ( NULL == sock_p ) {
		throw std::runtime_error( "TSocket::getCachedAddress() returned NULL!" );
	}

	server_uri.clear();
	server_uri.append( "coap://" );
	server_uri.append( host_ );
	server_uri.append( "/" );
	server_uri.append( path_ );

	split_uri_failed = coap_split_uri( (const unsigned char *) server_uri.c_str(), server_uri.size(), &uri );
	if ( split_uri_failed ) {
		throw TTransportException( TTransportException::BAD_ARGS, "coap_split_uri could not parse server_uri '" + server_uri + "'" );
	}

	coap_address_init( &dst_addr );
	// values should already be in network byte order
	std::memcpy( &dst_addr.addr, sock_p, sock_len );
	dst_addr.size = sock_len;

	// Construct a CoAP header
	request = coap_new_pdu();
	request->hdr->type = message_type;
	request->hdr->id   = coap_new_message_id( ctx );
	request->hdr->code = method_type;

	add_option_success = coap_add_option( request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s );
	if ( ! add_option_success ) {
		throw TTransportException( TTransportException::BAD_ARGS, "coap_add_option() failed" );
	}

	writeBuffer_.getBuffer( &buf, &len );
	add_data_success = coap_add_data( request, len, buf );
	if ( ! add_data_success ) {
		throw TTransportException( TTransportException::BAD_ARGS, "coap_add_data() failed" );
	}

	std::cout <<
		"TCoapClient::sendBufferedRequest(): sending request" <<
		" with transaction id " << request->hdr->id <<
		" to address " << toString( sock_p, sock_len ) <<
		std::endl;

	send_r = coap_send_message_type( ctx, ctx->endpoint, & dst_addr, request, message_type );
	if ( COAP_INVALID_TID == send_r ) {
		throw TTransportException( TTransportException::BAD_ARGS, "coap_send_message_type() failed" );
	}
}
*/
/*
void TCoapClient::handle_response( coap_context_t *context, coap_queue_t *sent, coap_queue_t *rcvd, uint8_t **buf, uint32_t *len ) {

	THRIFT_UNUSED_VARIABLE( buf );
	THRIFT_UNUSED_VARIABLE( len );

	std::cout << "TCoapClient::handle_response()" << std::endl;

	coap_send_ack( context, &rcvd->local_if, &rcvd->remote, rcvd->pdu );

//	In a lossy context, the ACK of a separate response may have
//	been lost, so we need to stop retransmitting requests with the
//	same token.
	coap_cancel_all_messages( context, &rcvd->remote, rcvd->pdu->hdr->token, rcvd->pdu->hdr->token_length );

//	Call application-specific response handler when available.
	if ( context->response_handler ) {
		context->response_handler( context, &rcvd->local_if, &rcvd->remote, sent ? sent->pdu : NULL, rcvd->pdu, rcvd->id );
	}
}

void
TCoapClient::coap_response_handler(
		coap_context_t *ctx,
		const coap_endpoint_t *local_interface, const coap_address_t *remote,
	    coap_pdu_t *sent, coap_pdu_t *received,
	    const coap_tid_t id
) {
	std::cout << "TCoapClient::coap_response_handler(): received response to transaction id " << id << std::endl;
}
*/

}
}
} // apache::thrift::transport
