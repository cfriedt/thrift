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

#ifndef _THRIFT_TRANSPORT_THTTPCLIENT_H_
#define _THRIFT_TRANSPORT_THTTPCLIENT_H_ 1

#include <thrift/transport/TCoapTransport.h>

namespace apache {
namespace thrift {
namespace transport {

class TCoapClient: public TCoapTransport {
public:

	typedef enum {
		CON = COAP_MESSAGE_CON,
		NON,
		ACK,
		RST,
	} coap_message_t;

	typedef enum {
		GET = COAP_REQUEST_GET,
		POST,
		PUT,
		DELETE,
	} coap_method_t;

	TCoapClient( boost::shared_ptr<TTransport> transport, std::string host, std::string path = "" );

	TCoapClient( std::string host, int port, std::string path = "" );
	TCoapClient( std::string host, int server_port, int client_port, std::string path );

	virtual ~TCoapClient();

	void open();

	void flush();

	coap_message_t getCoapMessageType();
	void setCoapMessageType( coap_message_t message_type );

	coap_method_t getCoapMethodType();
	void setCoapMethodType( coap_method_t method_type );

protected:

	std::string host_;
	std::string path_;

	coap_message_t message_type;
	coap_method_t method_type;

	void sendBufferedRequest();

	void handle_response( coap_context_t *context, coap_queue_t *sent, coap_queue_t *rcvd, uint8_t **buf, uint32_t *len );

	static void coap_response_handler(
		struct coap_context_t *ctx,
		const coap_endpoint_t *local_interface, const coap_address_t *remote,
		coap_pdu_t *sent, coap_pdu_t *received,
		const coap_tid_t id
	);
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_THTTPCLIENT_H_
