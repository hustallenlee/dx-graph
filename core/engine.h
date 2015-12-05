#ifndef _ENGINE_
#define _ENGINE_
#include "../utils/buffer.h"
#include "../utils/config.h"
#include "../utils/log_wrapper.h"
#include "../utils/types.h"
#include "../utils/type_utils.h"
#include "../utils/update_stream.h"
#include "../utils/filter.h"
#include "transportation.h"
#include "judge.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#define DISK_IO_SIZE 50000000

template <class update_type >
class update_array: public std::vector<update_type >{
private:

public:
	update_array(){}
	
	virtual void push_update( update_type update){

		this -> push_back(update);
	}
	
};

//inherit the class judgement
template <class update_type>
class contrete_judgement:public judgement<update_type>{
private:
	
public:
	contrete_judgement(std::string ini_filename):judgement<update_type>(ini_filename) {}
	virtual int test(update_type update ){
		if ((this -> min_id <= update.id) && (this -> max_id >= update.id)){
			return 1;
		}
		else{
			return 0;
		}
	}
};



//
//class is an engine that user can inherit it achieve 
//function scatter(), gather(), output()
//
template <class update_type>
class engine {
protected:
	
	//an user-defined max loop, send by argv
	int max_loop;

	//count the superstep
	int step_counter;

	//std::string config_file;
	//the edge list file
	std::string filename;

	//a disk buffer, that one thread can read the file and writes
	//the bytes into the buffer, another thread can read bytes
	//from the buffer
	dx_lib::buffer<BYTE> *disk_io;

	//the vertex number of the graph
	unsigned long vertex_num;

	//the edge number of graph
	unsigned long edge_num;
	
	//the edge list type
	int type;

	//the edge size of one edge
	int edge_size;

	
	char buf[1000000];
	int offset;
	int edge_num_once;
	int byte_num_once;
	int readed_bytes;
	long updated_num;
	//update_array<update_type > ua;

	//update_buf is a buffer which update belongs to local machine 
	dx_lib::update_stream<update_type > *update_buf;
	
	//trans is used to tansport update from network or local to local or netowrk 
	transportation<update_type > * trans;

	//judge a vertex belongs to local or not
	contrete_judgement<update_type > *ju;
	//scatter thread
	boost::thread *scatter_thrd;

	//gather thread
	boost::thread *gather_thrd;

	bool gather_return;

	
public:
	
	engine (std::string fn, int mloop){

		//the path of the edge list file
		filename = fn;
		max_loop = mloop;
		step_counter = 0;
		vertex_num = 0;
		edge_num =0;
		type = 0;
		edge_size = sizeof(format::edge_t);
		offset = 0;
		edge_num_once = 100000;      //read 1000 edges once a time
    	byte_num_once = edge_num_once * edge_size;
		readed_bytes = 0;
		updated_num = 0;

		//pointer = 0;
		//ua.clear();
		//the edge config file
		format::config conf;
		std::stringstream record;
		if( !conf.open_fill(filename + ".ini")){
			LOG_TRIVIAL(fatal)<< "can not open config file "
								<<filename + ".ini";
			exit(0);
		}

		//change from string to int
		record.clear();
		record<< conf["vertices"];
		record>> vertex_num;

		//change from stirng to int
		record.clear();
		record<< conf["type"];
		record>> type;
		
		//change from string to int
		record.clear();
		record<< conf["edges"];
		record>> edge_num;
		
		//disk buffer
		disk_io = new dx_lib::buffer<BYTE>(DISK_IO_SIZE);
		
		//local update buffer
		update_buf = new dx_lib::update_stream<update_type >(100000);

		//contrete_judgement
		ju = new contrete_judgement<update_type >(filename + ".machine");

		//remain to be 
		trans = new transportation<update_type >(update_buf, ju, filename + ".machine");
		trans -> start_transportation();
		scatter_thrd = NULL;
		gather_thrd = NULL;
		gather_return = false;
		//scatter_return = false;
	}
	~ engine(){
		delete disk_io;
		delete update_buf;
		delete trans;
		delete ju;
	}	
	
	//go to next super step
	virtual	void next_super_step() {
		step_counter ++;
	}
	
	//return current super step
	virtual int super_step(){
		return step_counter ;
	}	
	
	//return whether reach the max super step
	virtual bool reach_max_step (){
		if (step_counter >= max_loop){
			return true;
		}
		else{
			return false;
		}
	}

	//return: false means the file has been written fully
	//while true means disk read successful
	bool fill_in_buf(int start){
		if(!disk_io -> is_over()){
			readed_bytes = disk_io -> read(buf + start, byte_num_once);
			offset = 0;
			return true;
		}
		else{
			readed_bytes = 0;
			offset = 0;
			return false;
		}
	}

	//return: false means that reach the end of the file 
	//true means that read ok
	bool get_next_edge(format::edge_t & edge){
		if ( (offset + edge_size ) <= readed_bytes ){//will not overflow
			format::format_utils::read_edge(buf + offset, edge);
			offset += edge_size;
			return true;
		}
		else if( (offset + edge_size > readed_bytes) && offset <= readed_bytes ){

			memcpy(buf, buf + offset, readed_bytes - offset);
			
			if (fill_in_buf( readed_bytes - offset)){
				if((offset + edge_size) <= readed_bytes ){
					format::format_utils::read_edge( buf + offset, edge );
					offset += edge_size;
					return true;
				}
			}
			else{
				update_buf ->set_write_over();
				return false;  //there isn't more bytes in the buffer
			}
		}
		else{
			update_buf ->set_write_over();
			return false;	//means that file is fully written in the buffer
		}
	
	}


	void set_convergence(){
		gather_return = true;
	}
	
	bool is_convergence(){
		return gather_return;
	}
	
	void add_update(update_type update ){
		//update_buf -> add_update(update);
		trans -> add_to_mul_from_local(update);
	}

	bool get_update(update_type &update){
		return update_buf -> get_update(update);
	}

	//return means that a computing in a 
	//super step is convergence or not
	//
	virtual void scatter() = 0;
	virtual bool gather() = 0;	
	virtual void output() = 0;

	//run the super step
	virtual void run(){
		
		auto f_scatter = boost::bind(&engine::scatter, this);
		auto f_gather = boost::bind(&engine::gather, this);

		while ( !reach_max_step() ){
			LOG_TRIVIAL(info)<<"iterator "<<super_step();
			update_buf -> reset();
			disk_io -> start_write(filename);
			scatter_thrd = new boost::thread(f_scatter);
			gather_thrd = new boost::thread(f_gather);

			
			disk_io -> write_join();		
			scatter_thrd -> join();
			gather_thrd -> join();
			if ( is_convergence() ){
				break;
			}
			next_super_step();
		}
		
		output();
		trans -> join_transportation();
	}
	
};
#endif

