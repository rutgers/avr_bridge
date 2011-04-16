/*
 * Ros.h
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Adam Stambler, Cody Schafer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Adam Stambler, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef NODE_HANDLE_H_
#define NODE_HANDLE_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ros_types.h"
#include "ros_string.h"
#include "Msg.h"

 
#ifndef UINT8_MAX
#define UINT8_MAX 0xff
#endif

namespace ros {

typedef void (RosCb)(Msg const *msg);

/* XXX: there are 3 ways to go about giving class Ros the ability to send
 * data over the wire:
 *  1) use hard coded function names which (for some inexplicable
 *     reason, use the stdio style even though it is uneeded)
 *  2) pass a (FILE *) to the Ros constructor.
 *  3) pass a class implimenting a send_packet method of some sort.
 *
 * Lets try to get #3 in here
 */
int fputc(char c, FILE *stream);
int fgetc(FILE* stream);
static FILE *ros_io = fdevopen(ros::fputc, ros::fgetc);


typedef uint8_t Publisher;

typedef void (*ros_cb)(const Msg* msg);

struct packet_header{
		uint8_t packet_type;
		uint8_t topic_tag;
		uint16_t msg_length;
	};


class NodeHandle {
public:
   NodeHandle(const char * node_name, char mst_ct, int16_t buffer_size);


	//Get the publisher for a topic
	//You cannot advertise topics that were not in the config file
	Publisher advertise(const char* topic);
	void publish(Publisher pub, Msg* msg);

	//Get the publisher for a topic
	//You cannot advertise topics that were not in the config file
	void subscribe(const char* name, ros_cb funct, Msg* msg);
	void spin();

	//send a msg to the bridge node
	void send(uint8_t* data, uint16_t length, char packet_type, char topicID); //handles actually sending the data

	ros::string name;

	~NodeHandle(){};
private:
	ros_cb* cb_list;
	Msg ** msgList;
	uint8_t *outBuffer;
	uint8_t * buffer;

	const char MSG_CT;
	const uint16_t BUFFER_SZ;

	void sendID();

	packet_header * header;
	int packet_data_left;
	uint16_t buffer_index;

	enum packet_state{
		header_state , msg_data_state
	} com_state;

	void resetStateMachine();

	/* given the character string of a topic, determines the numeric tag to
	 * place in a packet */
	/* char getTopicTag(char const *topic); */
#include "ros_get_topic_tag.h"
};

} /* namespace ros */

#endif /* NODE_HANDLE_H_ */
