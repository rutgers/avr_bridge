/*
 * node_handle.cpp.cpp
 *
 * 
# Software License Agreement (BSD License)
#
# Copyright (c) 2011, Adam Stambler
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Adam Stambler, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
 */

#include "node_handle.h"

int ros::fputc(char c, FILE *stream);
int ros::fgetc(FILE *stream);

FILE* ros_io = fdevopen(ros::fputc, ros::fgetc);


using namespace ros;

NodeHandle::NodeHandle(const char * node_name, char message_count, int16_t buffer_size) : 
	name(node_name), BUFFER_SZ(buffer_size), MSG_CT(message_count){

	cb_list =   new ros_cb[MSG_CT];
	msgList =   new Msg*[MSG_CT];
	outBuffer = new uint8_t[BUFFER_SZ];
	buffer =    new uint8_t[BUFFER_SZ];
	
	this->packet_data_left = 0;
	this->buffer_index =0;
	this->header = (packet_header *) this->buffer;
	this->com_state = header_state;
}

void NodeHandle::subscribe(const char * topic, ros_cb funct, Msg* msg){
	int tag = getTopicTag(topic);
	this->cb_list[tag] = funct;
	this->msgList[tag] = msg;
}

void NodeHandle::publish(Publisher pub, Msg* msg){
	uint16_t bytes = msg->serialize(this->outBuffer);
		this->send(outBuffer,bytes,0,pub);
	}

void NodeHandle::resetStateMachine(){
	packet_data_left = 0;
	buffer_index = 0;
	com_state = header_state;
}


void NodeHandle::spin(){

	int com_byte =  ros::fgetc(ros_io);


	while (com_byte != -1) {
		buffer[buffer_index] = com_byte;
		buffer_index++;

		if(com_state == header_state){
			if ( buffer_index == sizeof(packet_header)){
				com_state = msg_data_state;
				this->packet_data_left = header->msg_length;

				if (com_state == msg_data_state){ 
					if (!((header->packet_type == 0) || (header->packet_type == 255)))
							resetStateMachine();
					if (header->packet_type == 0){
						if (header->topic_tag >= MSG_CT) resetStateMachine();
						if (header->msg_length>= BUFFER_SZ) resetStateMachine();
					}
				}
			}
		}
		if (com_state ==  msg_data_state){
			packet_data_left--;
			if (packet_data_left <0){
				resetStateMachine();
				if (header->packet_type ==255) this->sendID();
				if (header->packet_type==0){ //topic,
						//ie its a valid topic tag
						//then deserialize the msg
						this->msgList[header->topic_tag]->deserialize(buffer+4);
						//call the registered callback function
						this->cb_list[header->topic_tag](this->msgList[header->topic_tag]);
				}
				if(header->packet_type == 1){ //service

				}
			}
		}

		com_byte =  ros::fgetc(ros_io);
	}
}

//template <size_t MSG_CT, size_t BUFFER_SZ>
Publisher NodeHandle::advertise(const char* topic){

	return getTopicTag(topic);
}

void NodeHandle::send(uint8_t * data, uint16_t length, char packet_type, char topicID){

	fputc(packet_type,ros_io);

	//send msg ID
	fputc(topicID,ros_io);
	//send length of message to aid in io code on other side
	fwrite(&length,2,1,ros_io);

	fwrite(data, length,1,ros_io);
}

void NodeHandle::sendID(){
	uint16_t size = this->name.serialize(this->outBuffer);
	this->send(outBuffer, size, 255, 0);
}

