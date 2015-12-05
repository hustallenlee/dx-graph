#ifndef _NET_IOSTREAM_
#define _NET_IOSTREAM_

#include <zmq.hpp>
#include <string>
#include <sstream>
#include "update_stream.h"
namespace dx_lib{

	
	class net_stream{

	private:
		zmq::context_t *context;
		zmq::socket_t *rev_sock;
		zmq::socket_t *snd_sock;
		//update_stream<update_type> *update_buf;

	public:
		net_stream(std::string previous_ip, 
						int previous_port, 
						std::string next_ip,
						int next_port
						//update_stream<update_type> *update_buffer
						){
			std::stringstream ss;
			std::string p_sport, n_sport;

			std::string prefix = "tcp://";

			ss.clear();
			ss<< previous_port;
			ss>> p_sport;

			ss.clear();
			ss<< next_port;
			ss>> n_sport;

			context = new zmq::context_t(1);

			rev_sock = new zmq::socket_t(*context, ZMQ_PULL);
			snd_sock = new zmq::socket_t(*context, ZMQ_PUSH);
			
			std::string rev_str = prefix + previous_ip + ":" + p_sport;
			std::string snd_str = prefix + next_ip + ":" + n_sport;
			
			std::cout<<rev_str<<" "<<snd_str<<std::endl;

			rev_sock->bind(rev_str);
			//rev_sock->connect(rev_str.c_str());
			snd_sock->connect(snd_str.c_str());

			//update_buf = update_buffer;
		}
			
		bool recv(zmq::message_t &message){
			return rev_sock->recv(&message);
		}

		int recv(char * buf, int n){
			return rev_sock->recv((void *)buf, n);
		}
	
		bool send(zmq::message_t &message){
			//std::cout<<"here"<<std::endl;
			return  snd_sock->send(message);
		}

		int send(char * buf, int n){
			//std::cout<<"here"<<std::endl;
			return  snd_sock->send((void *)buf, n);
		}
		
	};
}
#endif
