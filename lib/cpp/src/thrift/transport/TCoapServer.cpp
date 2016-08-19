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

#include <cstdlib>
#include <sstream>
#include <iostream>

#include <thrift/transport/TCoapServer.h>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

TCoapServer::TCoapServer(boost::shared_ptr<TTransport> transport )
:
	TCoapTransport( transport )
{
}

TCoapServer::~TCoapServer() {
	resource.clear();
}

std::string TCoapServer::getUri() {
	return uri;
}

std::string TCoapServer::getMethod() {
	return method;
}

void TCoapServer::flush() {
}

/*
void TCoapServer::requestResource( boost::shared_ptr<coap_resource_t> req ) {
	coap_resource_t *res;
	res = coap_resource_init( req->uri.s, req->uri.length, req->flags );
	for( int j = 0; j < ARRAY_SIZE( req->handler ); j++ ) {
		if ( NULL == req->handler[ j ] ) {
			continue;
		}
		coap_register_handler( res, j, req->handler[ j ] );
		coap_add_resource( getCoapContext().get(), res );
	}
	resource.push_back( boost::shared_ptr<coap_resource_t>( res ) );
}

void TCoapServer::flush() {
	throw std::runtime_error( "TCoapServer::flush() not implemented" );

  // Fetch the contents of the write buffer
  uint8_t* buf;
  uint32_t len;
  writeBuffer_.getBuffer(&buf, &len);

  // Construct the HTTP header
  std::ostringstream h;
  h << "HTTP/1.1 200 OK" << CRLF << "Date: " << getTimeRFC1123() << CRLF << "Server: Thrift/"
    << VERSION << CRLF << "Access-Control-Allow-Origin: *" << CRLF
    << "Content-Type: application/x-thrift" << CRLF << "Content-Length: " << len << CRLF
    << "Connection: Keep-Alive" << CRLF << CRLF;
  string header = h.str();

  // Write the header, then the data, then flush
  // cast should be fine, because none of "header" is under attacker control
  transport_->write((const uint8_t*)header.c_str(), static_cast<uint32_t>(header.size()));
  transport_->write(buf, len);
  transport_->flush();

  // Reset the buffer and header variables
  writeBuffer_.resetBuffer();
  readHeaders_ = true;
*/
}

void TCoapServer::handle_request( coap_context_t *context, coap_queue_t *node, uint8_t **buf, uint32_t *len ) {

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

// XXX: @CF: [Begin] SNIP

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

// XXX: @CF: [End] SNIP

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
*/

}
}
} // apache::thrift::transport
