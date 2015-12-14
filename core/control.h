#ifndef _CONTROL_
#define _CONTROL_
#include "../utils/net_stream.h"
#include "../utils/log_wrapper.h"
#include "../utils/ini_config.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <zmq.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional/optional.hpp>
#include <boost/foreach.hpp>
#include <unistd.h>

#define MAX_MACHINE_NUM 100

class controller_server{
private:
	zmq::context_t  				*context;
	zmq::socket_t   				*publisher;	
	zmq::socket_t   				*receiver;
	boost::thread   				*recv_thrd;
	std::vector<std::string > 		machines;
	std::map<std::string, bool >	flags;
	int								p_port;
	int								r_port;
	unsigned int 					niter;
public:
	controller_server(unsigned iteration_num){
		
		_load_json();
		niter = iteration_num;
		context = new zmq::context_t(1);


		std::stringstream record;
		std::string address = "tcp://*:";

		//publisher start
		publisher = new zmq::socket_t(*context, ZMQ_PUB);
		std::string publish_port;

		record.clear();
		record << p_port;
		record >> publish_port;
		LOG_TRIVIAL(info) << "publisher: bind to local port " << p_port;

		publisher->bind(address + publish_port);
		//publisher->bind("ipc://weather.ipc");		
		//publisher end

		//receiver start
		std::string receive_port;
		record.clear();
		record << r_port;
		record >> receive_port;
		
		receiver = new zmq::socket_t(*context, ZMQ_PULL);
		receiver->bind(address + receive_port);
		sleep(1);		
		
	}
	void _load_json(){
		boost::property_tree::ptree pt;	
		boost::property_tree::json_parser::read_json("./machines.json", pt);

		p_port = pt.get<int >("local.publisher_port");
		r_port = pt.get<int >("local.receive_port");
		boost::property_tree::ptree ips = pt.get_child("ips");
		
		BOOST_FOREACH(auto &v, ips){
			//insert all machines ip to a vector
			
			machines.push_back(v.second.get_value<std::string>());
			//#ifdef DEBUG
			//std::cout<<"ip: "<<v.second.get_value<std::string>();
			//#endif
		}
		//initialize the map, set all flags to false
		for (auto iter = machines.begin(); iter != machines.end(); iter ++){
			flags[*iter] = false;
		}
		
	}
	
	//send a message	
	void send_message(zmq::message_t message){
		
		
		publisher->send(message);
	}

	//send a string message
	void send_message(std::string msg){
		//publisher->send(_generate_msg(msg));
		send_message(_generate_msg(msg));
	}	

	//generate a message using string
	zmq::message_t _generate_msg(std::string msg){
		zmq::message_t message(msg.size());
		memcpy((void *)message.data(), (void *)msg.c_str(), msg.size());
		return message;
	}
	
	//send all subscribers a start message
	void send_start(){
		std::string start_str = "{\"start\":true}";
		#ifdef DEBUG
		LOG_TRIVIAL(info)<< "send the start message: "<< start_str;
		#endif
		send_message( start_str );
		
	}
	
	//send the "go on" message
	void send_gonext(){
		std::string gonext_str = "{\"go_on\":true}";
		#ifdef DEBUG
		LOG_TRIVIAL(info)<< "send the go_on message: "<< gonext_str;
		#endif
		send_message( gonext_str );
	}
	//send all subscribers a start message
	void send_end(){
		std::string end_str = "{\"end\":true}";
		#ifdef DEBUG
		LOG_TRIVIAL(info)<< "send the end message: "<< end_str;
		#endif
		send_message( end_str );
	}

	//receive the compute node 
	void _receive(){
		zmq::message_t message;
		unsigned int step = 0;
		unsigned int max_step = UINT_MAX;
		bool convergence;
	
		std::string  ip;
		std::map<std::string, bool>::iterator pos;
		unsigned int record_num = 0;
		
		unsigned int sp = 0;
		

		//first, send the start message to start the compute node
		send_start();

		while(sp < niter){

			//a super step
			LOG_TRIVIAL(info)<<"********super step "<<sp
							 <<" ********";

			//the number of compute node
			unsigned int nnode = machines.size();

			#ifdef DEBUG
			LOG_TRIVIAL(info)<<"the compute node number: "<< nnode;
			#endif
				
			//at every beginning of the super step
			//send the start message to all compute node
			bool stop = false;


			//every super step should receive all machine's message
			//and when all the messages have been received,
			//the controller should send the message to start the 
			//next super step of the compute node 
			while(nnode){

				//receive a message means that the node complete a superstep
				LOG_TRIVIAL(info)<<"remain "<< nnode <<" to handler";

				receiver->recv(&message);
				
				LOG_TRIVIAL(info)<<"message size "<< message.size();
				print_msg(message);
				step = get_value<unsigned int >(message, "current_step");
				
				//check the current step is right or not
				if (step != sp){
					LOG_TRIVIAL(info) << "The current step should be " << sp
									  << " but the current compute node step is "
									  << step;
					stop = true;
					break;
				}

				//the compute is controlled by the controller, controllrt determine the max loop
				//max_step = get_value<unsigned int >(message, "max_step");
				
				//indicate that the vertex of the node is convergence or not
				convergence = get_value<bool>(message, "convergence");
				
				//the ip of the node
				ip = get_value<std::string >(message, "ip");
			

				if ( ( pos = flags.find( ip ) ) != flags.end() ){
					pos->second = true;
					nnode --;
				}
				else{
					LOG_TRIVIAL(info)<<"the node is not in the control node";
				}
			}//end one iteration 
			
			//reset all the flags
			for (pos = flags.begin(); pos != flags.end(); pos ++){
				pos->second = false;
			}

			if (stop == true){
				break;
			}
			//send go on next iteration message to all the compute node
			send_gonext();		

			sp ++;
		}//end sp
		//send the end message to all the compute node
		send_end();
	}
	void print_msg(zmq::message_t &msg){
		std::string str((char *)msg.data(), msg.size());
		LOG_TRIVIAL(info)<<"received message: "<<str;
	}
	//analysis the message, the message is a json string
	//for exmaple, a json string is:
	//{
	//	"current_step":5,
	//	"convergece":true,
	//	"ip":"192.168.3.83"
	//}	
	//return value get from the json
	//	1: means the return value is the iterations
	//
	template <class type>
	type get_value(zmq::message_t &message, std::string path){
	
		//transfer the message to string
		std::string json_string((char *)message.data(), message.size());

		std::stringstream ss(json_string);
		//a property tree
		boost::property_tree::ptree pt;

		//read json using json_parser
		boost::property_tree::json_parser::read_json(ss, pt);

		return pt.get<type >(path);
	}

	void start(){
		//auto f = boost::bind(&controller_server::_receive, this);
		//recv_thrd = new boost::thread(f);		
		_receive();
		//recv_thrd->join();
	}
};

class controller_client{
private:
	zmq::context_t	*context;
	zmq::socket_t 	*subscriber;
	zmq::socket_t	*sender;	

	std::string		server_ip;
	std::string		localIP;
	int				server_port;
	int				push_port;

public:
	controller_client(std::string filepath){

		_load_config(filepath);

		std::string address_sub = "tcp://";
		std::string address_send = "tcp://";

		std::string	p_port;
		std::string s_port;
		std::stringstream record;


		record.clear();
		record << server_port;
		record >> s_port;

		record.clear();
		record << push_port;
		record >> p_port;

		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"s_port "<<s_port<<" p_port "<<p_port;
		#endif

		context = new zmq::context_t(1);

		subscriber = new zmq::socket_t(*context, ZMQ_SUB);

		address_sub = address_sub + server_ip + ":" + s_port;

		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"address_sub "<<address_sub;
		#endif
		
		subscriber->connect(address_sub);
		
		const char * filter ="";
		
		//the subscriber get all the messages
		subscriber->setsockopt(ZMQ_SUBSCRIBE, filter, 0);

		sender = new zmq::socket_t(*context, ZMQ_PUSH);
		address_send = address_send + server_ip + ":" +p_port;

		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"address_send "<<address_send;
		#endif
	
		sender->connect(address_send);
		
	}

	void _load_config(std::string filepath){

		format::ini_file ifile(filepath);

		server_ip = ifile.get_value<std::string >("control", "server_ip");
		localIP = ifile.get_value<std::string >("control", "localIP");

		server_port = ifile.get_value<int >("control", "server_port");
		push_port = ifile.get_value<int >("control", "push_port");	
	}
	
	void send_msg_to_server(int current_step, bool convergence){
	//{
	//	"current_step":5,
	//	"convergence":true,
	//	"ip":"192.168.3.83"
	//}	
		std::stringstream ss;
		std::string str;

		//form the message string
		ss << "{";
		ss << "\"current_step\":";
		ss << current_step;
		ss << ", \"convergence\":false,";
		ss << "\"ip\":";
		ss << "\"";	
		ss << localIP;
		ss << "\"";
		ss << "}";
		LOG_TRIVIAL(info)<< "send to server :" <<ss.str();
		str.assign(ss.str());
		zmq::message_t message(str.size());

		//copy to the message
		memcpy((void *)message.data(), str.c_str(), str.size());

		sender->send(message);
		
	}
	int recv_msg(){
		//the client may receive three type message
		//the first type is start message. "start":true
		//the second is go on message.		"go_on":true
		//the third is end message.			"end":true
		zmq::message_t command;
		subscriber->recv(&command);
		std::string json_string((char *)command.data(), command.size());
		
		std::stringstream ss(json_string);

		boost::property_tree::ptree pt;
		
		boost::property_tree::json_parser::read_json(ss, pt);
	
		boost::optional<bool > start = pt.get_optional<bool>("start");
		boost::optional<bool > go_on = pt.get_optional<bool>("go_on");
		boost::optional<bool > end = pt.get_optional<bool>("end");
		
		if (start){ // receive the start message
			return 1;
		}	
		else if (go_on){
			return 2;
		}
		else if (end){
			return 3;
		}
		else{
			return 0;
		}
	}	
};
#endif
