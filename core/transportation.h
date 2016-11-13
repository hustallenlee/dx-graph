#ifndef _TRANSPORTATION_
#define _TRANSPORTATION_
#include "../utils/ini_config.h"
#include "../utils/log_wrapper.h"
#include "../utils/filter.h"
#include "../utils/net_stream.h"
#include "judge.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#define MULTI_BUFFER_SIZE 100000

/*
class transportation can get update from local machine and net,
and filter the update, add to the local machine update buffer,
or send to the network. 
*/
template <class update_type>
class transportation{
private:

	//mul_update is a buffer that two thread can add update to the buffer
	//and one thread can get update from it.
	dx_lib::multi_update_stream<update_type>     *mul_update;

	//update_buf is a buffer that updates can be added, and a gather thread can
	//get updates from this buffer.
	dx_lib::update_stream<update_type>           *update_buf;

	//net_io is a class that can get update fron network 
	//and send update to the network
	dx_lib::net_stream                           *net_io;

	//fi is a filter that can filt the update, add the update to update_buf
	//or add the update to network 
	dx_lib::filter<update_type>					 *fi;

	//judge is a class that user define, to test an update whether 
	//belongs to local machine.
	judgement<update_type>						 *judge;
	
	//
	boost::thread * thrd;
public:
	transportation(    
    		dx_lib::update_stream<update_type>   *ub,
			judgement<update_type>					 *ju,
			std::string							 fn_machine	
		){
		update_buf = ub;
		mul_update = new dx_lib::multi_update_stream<update_type>(MULTI_BUFFER_SIZE);
		judge = ju;

		//initialize the ini file
		format::ini_file ifile(fn_machine);

		//get the prevoius ip and port
		std::string p_ip = ifile.get_value<std::string >("machines","p_ip");
		int p_port = ifile.get_value<int >("machines","p_port");

		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"transportation: previous ip "<<p_ip;
		LOG_TRIVIAL(info)<<"transportation: previous port "<<p_port;
		#endif

		//get the next ip and port
		std::string n_ip = ifile.get_value<std::string >("machines","n_ip");
		int n_port = ifile.get_value<int >("machines","n_port");
		
		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"transportation: next ip "<<n_ip;
		LOG_TRIVIAL(info)<<"transportation: next port "<<n_port;
		#endif
		unsigned long offset = ifile.get_value<unsigned long>("machines","min_id");

		#ifdef DEBUG
		LOG_TRIVIAL(info)<<"transportation: offset "<<offset;
		#endif

		net_io = new dx_lib::net_stream(p_ip, p_port, n_ip, n_port);
		
		fi = new dx_lib::filter<update_type >(update_buf, mul_update, net_io, judge, offset);
		
	}

	~transportation(){
		delete mul_update;
		delete net_io;
		delete fi;
	}

	//add to the multiple buffer generated by local from scatter phase
	void add_to_mul_from_local(update_type update){
		mul_update->add_update(update);
	}
	
	//add the update from network into the multi buffer
	void add_from_net(){
		const int recv_update_size = 10000; // 10000 updates once
		update_type recv_buf[recv_update_size];
		int recv_size = 0;
		int update_size = sizeof(update_type);
		int recv_byte_size = update_size * recv_update_size;
		while(1){
			recv_size = net_io -> recv( (char *)recv_buf, recv_byte_size);
			//add all the update received to the multi buffer
			mul_update -> write(recv_buf, (int )( recv_size / update_size ) );
			/*temp = 0;
			while(temp < recv_size){
				mul_update -> add_update_from_net( recv_buf[temp], temp == (recv_size -1) );
				temp ++ ;
			}*/
		}
	}
	void start_add_from_net(){
		auto f = boost::bind(&transportation::add_from_net, this);
		thrd = new boost::thread(f);
	}
	
	void join_add_from_net(){
		if(thrd){
			if( thrd -> joinable() ){
				thrd -> join();
			}
		}
	}
	void join_transportation(){
		
		join_add_from_net();
		fi ->join_filter();
		
	}
	void start_transportation(){
		
		#ifdef BEBUG
		LOG_TRIVIAL(info)<<"starting get update from network";
		#endif

		start_add_from_net();
		
		#ifdef BEBUG
		LOG_TRIVIAL(info)<<"starting filter the update ";
		#endif
		fi->start_filt();



	}
};

#endif