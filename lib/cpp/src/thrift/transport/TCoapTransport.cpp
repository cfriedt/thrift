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
	//writeBuffer_.write( buf, len );
	throw std::runtime_error( "TCoapTransport::write() not implemented" );
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

	// FIXME: @CF: Assumes Socket Transport
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
			TCoapTransport::handle_request( ctx, rcvd );
		} else if ( COAP_MESSAGE_IS_RESPONSE( rcvd->pdu->hdr ) ) {
			TCoapTransport::handle_response( ctx, sent, rcvd );
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

void TCoapTransport::handle_request( coap_context_t *context, coap_queue_t *node ) {

	coap_method_handler_t h = NULL;
	coap_pdu_t *response = NULL;
	coap_opt_filter_t opt_filter;
	coap_resource_t *resource;
	coap_key_t key;

	boost::shared_ptr<TTransportException> ex;

	coap_option_filter_clear( opt_filter );

	/* try to find the resource from the request URI */
	coap_hash_request_uri( node->pdu, key );
	resource = coap_get_resource_from_key( context, key );

	if ( !resource ) {
		/* The resource was not found. Check if the request URI happens to
		 * be the well-known URI. In that case, we generate a default
		 * response, otherwise, we return 4.04 */

		switch ( node->pdu->hdr->code ) {

		case COAP_REQUEST_GET:
			if ( is_wkc( key ) ) { /* GET request for .well-known/core */
//				info( "create default response for %s\n", COAP_DEFAULT_URI_WELLKNOWN );
				response = coap_wellknown_response( context, node->pdu );

			} else { /* GET request for any another resource, return 4.04 */

//				debug( "GET for unknown resource 0x%02x%02x%02x%02x, return 4.04\n", key[ 0 ], key[ 1 ], key[ 2 ], key[ 3 ] );
				response = coap_new_error_response( node->pdu, COAP_RESPONSE_CODE( 404 ), opt_filter );
			}
			break;

		default: /* any other request type */

//			debug( "unhandled request for unknown resource 0x%02x%02x%02x%02x\r\n", key[ 0 ], key[ 1 ], key[ 2 ], key[ 3 ] );

			response = coap_new_error_response( node->pdu, COAP_RESPONSE_CODE( 405 ), opt_filter );
		}

		if ( response && !no_response( node->pdu, response ) && coap_send( context, &node->local_if, &node->remote, response ) == COAP_INVALID_TID ) {
//			warn( "cannot send response for transaction %u\n", node->id );
//			ex = boost::make_shared<TTransportException>( "cannot send response for transaction" );
		}
		coap_delete_pdu( response );

		return;
	}

	/* the resource was found, check if there is a registered handler */
	if ( (size_t) node->pdu->hdr->code - 1 < sizeof( resource->handler ) / sizeof(coap_method_handler_t) ) {
		h = resource->handler[ node->pdu->hdr->code - 1 ];
	}

	if ( h ) {
		//debug( "call custom handler for resource 0x%02x%02x%02x%02x\n", key[ 0 ], key[ 1 ], key[ 2 ], key[ 3 ] );
		response = coap_pdu_init( node->pdu->hdr->type == COAP_MESSAGE_CON ? COAP_MESSAGE_ACK : COAP_MESSAGE_NON, 0, node->pdu->hdr->id, COAP_MAX_PDU_SIZE );

		/* Implementation detail: coap_add_token() immediately returns 0
		 if response == NULL */
		if ( coap_add_token( response, node->pdu->hdr->token_length, node->pdu->hdr->token ) ) {
			str token = { node->pdu->hdr->token_length, node->pdu->hdr->token };
			coap_opt_iterator_t opt_iter;
			coap_opt_t *observe = NULL;
			int observe_action = COAP_OBSERVE_CANCEL;

			/* check for Observe option */
			if ( resource->observable ) {
				observe = coap_check_option( node->pdu, COAP_OPTION_OBSERVE, &opt_iter );
				if ( observe ) {
					observe_action = coap_decode_var_bytes( coap_opt_value( observe ), coap_opt_length( observe ) );

					if ( ( observe_action & COAP_OBSERVE_CANCEL ) == 0 ) {
						coap_subscription_t *subscription;

//						coap_log( LOG_DEBUG, "create new subscription\n" );
						subscription = coap_add_observer( resource, &node->local_if, &node->remote, &token );
						if ( subscription ) {
							coap_touch_observer( context, &node->remote, &token );
						}
					} else {
//						coap_log( LOG_DEBUG, "removed observer\n" );
						coap_delete_observer( resource, &node->remote, &token );
					}
				}
			}

			h( context, resource, &node->local_if, &node->remote, node->pdu, &token, response );

			if ( !no_response( node->pdu, response ) ) {
				if ( observe && ( COAP_RESPONSE_CLASS(response->hdr->code) > 2 ) ) {
					coap_log( LOG_DEBUG, "removed observer\n" );
					coap_delete_observer( resource, &node->remote, &token );
				}

				/* If original request contained a token, and the registered
				 * application handler made no changes to the response, then
				 * this is an empty ACK with a token, which is a malformed
				 * PDU */
				if ( ( response->hdr->type == COAP_MESSAGE_ACK ) && ( response->hdr->code == 0 ) ) {
					/* Remove token from otherwise-empty acknowledgment PDU */
					response->hdr->token_length = 0;
					response->length = sizeof(coap_hdr_t);
				}

				if ( response->hdr->type != COAP_MESSAGE_NON || ( response->hdr->code >= 64 && !coap_mcast_interface( &node->local_if ) ) ) {

					if ( coap_send( context, &node->local_if, &node->remote, response ) == COAP_INVALID_TID ) {
//						debug( "cannot send response for message %d\n", node->pdu->hdr->id );
					}
				}
			}
			coap_delete_pdu( response );
		} else {
//			warn( "cannot generate response\r\n" );
//			ex = boost::make_shared<TTransportException>( "cannot generate response" );
		}
	} else {
		if ( WANT_WKC( node->pdu, key ) ) {
//			debug( "create default response for %s\n", COAP_DEFAULT_URI_WELLKNOWN );
			response = coap_wellknown_response( context, node->pdu );
//			debug( "have wellknown response %p\n", (void * )response );
		} else {
			response = coap_new_error_response( node->pdu, COAP_RESPONSE_CODE( 405 ), opt_filter );
		}

		if ( !response || ( coap_send( context, &node->local_if, &node->remote, response ) == COAP_INVALID_TID ) ) {
//			debug( "cannot send response for transaction %u\n", node->id );
		}
		coap_delete_pdu( response );
	}
}

void TCoapTransport::handle_response( coap_context_t *context, coap_queue_t *sent, coap_queue_t *rcvd ) {

	coap_send_ack( context, &rcvd->local_if, &rcvd->remote, rcvd->pdu );

	/* In a lossy context, the ACK of a separate response may have
	 * been lost, so we need to stop retransmitting requests with the
	 * same token.
	 */
	coap_cancel_all_messages( context, &rcvd->remote, rcvd->pdu->hdr->token, rcvd->pdu->hdr->token_length );

	/* Call application-specific response handler when available. */
	if ( context->response_handler ) {
		context->response_handler( context, &rcvd->local_if, &rcvd->remote, sent ? sent->pdu : NULL, rcvd->pdu, rcvd->id );
	}
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
