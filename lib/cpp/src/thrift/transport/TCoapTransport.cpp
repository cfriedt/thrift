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
#include <sstream>

#include <boost/make_shared.hpp>

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TCoapTransport.h>

extern "C" {
#include <unistd.h> // close(2)
}

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapTransport::TCoapTransport( boost::shared_ptr<TTransport> transport )
:
	transport_( transport ),
	origin_( "" )
{
	const std::string coap_ver = coap_package_version();
}

TCoapTransport::~TCoapTransport() {
	if ( NULL != coap_context.get() ) {
		coap_context_t *ctx = coap_context.get();
		coap_free_context( ctx );
	}
	coap_address.reset();
	coap_context.reset();
}

// XXX: @CF: this is a workaround for the necessity to call coap_new_context() without
// being able to pass in a custom file descriptor / socket.
void TCoapTransport::replaceSocketAfterOpen() {
	int old_socket_fd;
	int new_socket_fd;
	boost::shared_ptr<TSocket> socket_trans;
	// FIXME: @CF: Assumes Socket Transport
	socket_trans = boost::static_pointer_cast<TSocket>( transport_ );
	old_socket_fd = socket_trans->getSocketFD();
	new_socket_fd = coap_context->sockfd;
	if ( new_socket_fd != old_socket_fd ) {
		socket_trans->setSocketFD( new_socket_fd );
		::close( old_socket_fd );
	}
}

void TCoapTransport::open() {
	transport_->open();
	coap_address = toCoapAddress( transport_ );
	coap_context = boost::shared_ptr<coap_context_t>( coap_new_context( coap_address.get() ) );
	if ( NULL == coap_context.get() ) {
		throw std::runtime_error( "coap_new_context() failed" );
	}
	replaceSocketAfterOpen();
}


uint32_t TCoapTransport::read( uint8_t* buf, uint32_t len ) {
	uint32_t r;
	coap_context_t *ctx;

	ctx = getCoapContext().get();
	r = TCoapTransport::coap_read( ctx, buf, len );

	return r;
}

void TCoapTransport::write( const uint8_t* buf, uint32_t len ) {
	writeBuffer_.write( buf, len );
}

const std::string TCoapTransport::getOrigin() {
	std::ostringstream oss;
	if ( !origin_.empty() ) {
		oss << origin_ << ", ";
	}
	oss << transport_->getOrigin();
	return oss.str();
}

void TCoapTransport::init() {
}

boost::shared_ptr<coap_context_t> TCoapTransport::getCoapContext() {
	return coap_context;
}

boost::shared_ptr<coap_address_t> TCoapTransport::toCoapAddress( boost::shared_ptr<TTransport> trans ) {

	sockaddr *sock_p;
	socklen_t sock_len;

	boost::shared_ptr<coap_address_t> coap_address;
	boost::shared_ptr<TSocket> socket_trans;

	socket_trans = boost::static_pointer_cast<TSocket>( trans );
	sock_p = socket_trans->getCachedAddress( & sock_len );
	if ( NULL == sock_p ) {
		throw std::runtime_error( "TSocket::getCachedAddress() returned NULL!" );
	}

	coap_address = boost::make_shared<coap_address_t>();

	switch( sock_len ) {

	case sizeof(sockaddr_in):

		std::memcpy( & coap_address->addr.sin, sock_p, sock_len );
		coap_address->addr.sin.sin_port = ntohs( coap_address->addr.sin.sin_port );
		coap_address->addr.sin.sin_addr.s_addr = ntohl( coap_address->addr.sin.sin_addr.s_addr );

		break;

	case sizeof(sockaddr_in6):

		std::memcpy( & coap_address->addr.sin6, sock_p, sock_len );
		coap_address->addr.sin6.sin6_port = ntohs( coap_address->addr.sin6.sin6_port );

		break;

	default:
		throw std::runtime_error( "unsupported socket length" );
		break;
	}

	return coap_address;
}

// functions borrowed / modified from libcoap

int TCoapTransport::coap_read( coap_context_t *ctx, uint8_t *buf, uint32_t len ) {
	// XXX: @CF: I've squished a number of libcoap functions into 1 here
	// The reason is that we do now want to rely on their application framework, only their
	// transport layer. Direct libcoap library calls give us the flexibility to use
	// the Thrift application layer.

	int r;
	ssize_t bytes_read;
	coap_packet_t *packet;
	unsigned char *msg;
	size_t msg_len;
	coap_queue_t *rcvd;
	bool pdu_get_data_success;
	uint32_t pdu_data_len;
	uint8_t *pdu_data;
	coap_queue_t *sent = NULL;
	coap_pdu_t *response = NULL;
	coap_opt_filter_t opt_filter;

	boost::shared_ptr<TTransportException> ex;

	ctx = getCoapContext().get();

	// [BEGIN] coap_read();

	bytes_read = ctx->network_read(ctx->endpoint, & packet);
	if ( bytes_read < 0 ) {
		ex = boost::make_shared<TTransportException>( "coap network_read() returned an invalid value" );
		goto throw_ex;
	}

	// [BEGIN] coap_handle_message()

	coap_packet_get_memmapped( packet, &msg, &msg_len );

	if ( msg_len < sizeof(coap_hdr_t) ) {
		ex = boost::make_shared<TTransportException>( "message length was smaller than a coap header" );
		goto free_packet;
	}

	// XXX: @CF: OMG I *love* *love* *love* when people use magic numbers 8-D
	if ( ((*msg >> 6) & 0x03) != COAP_DEFAULT_VERSION ) {
		ex = boost::make_shared<TTransportException>( "unknown coap protocol version" );
		goto free_packet;
	}

	rcvd = coap_new_node();
	if ( ! rcvd ) {
		ex = boost::make_shared<TTransportException>( "unable to allocate coap node" );
		goto free_packet;
	}
	rcvd->pdu = coap_pdu_init( 0, 0, 0, msg_len );

	if ( ! coap_pdu_parse( msg, msg_len, rcvd->pdu ) ) {
		// malformed pdu
		r = 0;
		goto free_node;
	}

	pdu_get_data_success = coap_get_data( rcvd->pdu, (size_t *) & pdu_data_len, & pdu_data );
	if ( ! pdu_get_data_success ) {
		ex = boost::make_shared<TTransportException>( "unable to get coap payload" );
		goto free_node;
	}
	// XXX: @CF: throw exception if read buffer is not large enough... maybe I *should* be using a readBuffer..
	if ( len < pdu_data_len ) {
		ex = boost::make_shared<TTransportException>( "receive buffer too small" );
		goto free_node;
	}

	coap_ticks( & rcvd->t );

	coap_packet_populate_endpoint( packet, &rcvd->local_if );
	coap_packet_copy_source( packet, &rcvd->remote );

	coap_transaction_id( &rcvd->remote, rcvd->pdu, &rcvd->id );

	// [BEGIN] coap_dispatch();

	memset(opt_filter, 0, sizeof(coap_opt_filter_t));

	/* version has been checked in coap_handle_message() */
	/* if ( rcvd->pdu->hdr->version != COAP_DEFAULT_VERSION ) { */
	/*   debug("dropped packet with unknown version %u\n", rcvd->pdu->hdr->version); */
	/*   goto cleanup; */
	/* } */

	switch ( rcvd->pdu->hdr->type ) {
	case COAP_MESSAGE_ACK:
		/* find transaction in sendqueue to stop retransmission */
		coap_remove_from_queue( &ctx->sendqueue, rcvd->id, &sent );

		if ( rcvd->pdu->hdr->code == 0 ) {
			goto coap_dispatch_cleanup;
		}

		/* if sent code was >= 64 the message might have been a
		 * notification. Then, we must flag the observer to be alive
		 * by setting obs->fail_cnt = 0. */
		if ( sent && COAP_RESPONSE_CLASS(sent->pdu->hdr->code) == 2 ) {
			const str token = { sent->pdu->hdr->token_length, sent->pdu->hdr->token };
			coap_touch_observer( ctx, &sent->remote, &token );
		}
		break;

	case COAP_MESSAGE_RST:
		/* We have sent something the receiver disliked, so we remove
		 * not only the transaction but also the subscriptions we might
		 * have. */

		/* find transaction in sendqueue to stop retransmission */
		coap_remove_from_queue( &ctx->sendqueue, rcvd->id, &sent );

		if ( sent ) {
			// #define WITHOUT_OBSERVER
			// coap_cancel( ctx, sent );
		}
		goto coap_dispatch_cleanup;

		break;

	case COAP_MESSAGE_NON: /* check for unknown critical options */
		if ( coap_option_check_critical( ctx, rcvd->pdu, opt_filter ) == 0 ) {
			goto coap_dispatch_cleanup;
		}
		break;

	case COAP_MESSAGE_CON: /* check for unknown critical options */
		if ( coap_option_check_critical( ctx, rcvd->pdu, opt_filter ) == 0 ) {

			/* FIXME: send response only if we have received a request. Otherwise,
			 * send RST. */
			response = coap_new_error_response( rcvd->pdu, COAP_RESPONSE_CODE( 402 ), opt_filter );

			if ( !response ) {
				warn( "coap_dispatch: cannot create error response\n" );
			} else {
				if ( coap_send( ctx, &rcvd->local_if, &rcvd->remote, response ) == COAP_INVALID_TID ) {
					warn( "coap_dispatch: error sending response\n" );
				}
				coap_delete_pdu( response );
			}

			goto coap_dispatch_cleanup;
		}
		break;

	default:
		break;
	}

	/* Pass message to upper layer if a specific handler was
	 * registered for a request that should be handled locally. */
	//if ( handle_locally( ctx, rcvd ) ) {
	if ( true ) {
		if ( COAP_MESSAGE_IS_REQUEST( rcvd->pdu->hdr ) ) {
			TCoapTransport::handle_request( ctx, rcvd, & buf, & len );
		} else if ( COAP_MESSAGE_IS_RESPONSE( rcvd->pdu->hdr ) ) {
			TCoapTransport::handle_response( ctx, sent, rcvd, & buf, & len );
		} else {
			//debug( "dropped message with invalid code (%d.%02d)\n", COAP_RESPONSE_CLASS(rcvd->pdu->hdr->code), rcvd->pdu->hdr->code & 0x1f );

			if ( ! coap_is_mcast( &rcvd->local_if.addr ) ) {
				coap_send_message_type( ctx, &rcvd->local_if, & rcvd->remote, rcvd->pdu, COAP_MESSAGE_RST );
			}
		}
	}

	coap_dispatch_cleanup:
	coap_delete_node( sent );
	coap_delete_node( rcvd );

	// [END] coap_dispatch();

	r = std::min( (uint32_t) rcvd->pdu->length, len );
	std::memcpy( buf, rcvd->pdu->data, r );

//delete_node:
	coap_delete_pdu( response );

	coap_delete_node( sent );
	coap_delete_node( rcvd );

free_node:
	// XXX: @CF: OMG I *love* *love* *love* when reinvent the memory management wheel!! 8-D
	//coap_free_node( node );
	std::free( rcvd );

	// [END] coap_handle_message()

free_packet:
	coap_free_packet(packet);

	// [END] coap_read()

throw_ex:
	if ( NULL != ex.get() ) {
		throw ex;
	}

	return r;
}

bool TCoapTransport::no_response( coap_pdu_t *request, coap_pdu_t *response ) {
	coap_opt_t *nores;
	coap_opt_iterator_t opt_iter;
	uint8_t val = 0;

	assert( request );
	assert( response );

	nores = coap_check_option( request, COAP_OPTION_NORESPONSE, &opt_iter );

	if ( nores ) {
		val = coap_decode_var_bytes( coap_opt_value( nores ), coap_opt_length( nores ) );
	}

	return ( COAP_RESPONSE_CLASS(response->hdr->code) > 0 ) && ( ( ( 1 << ( COAP_RESPONSE_CLASS(response->hdr->code) - 1 ) ) & val ) > 0 );
}

bool TCoapTransport::is_wkc( coap_key_t key ) {
	return 0 == std::memcmp( key, COAP_DEFAULT_WKC_HASHKEY, sizeof(coap_key_t) );
}
bool TCoapTransport::WANT_WKC( coap_pdu_t *pdu, coap_key_t key ) {
	return ( pdu->hdr->code == COAP_REQUEST_GET ) && is_wkc( key );
}


}
}
}
