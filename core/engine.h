#ifndef _ENGINE_
#define _ENGINE_
#include "../utils/buffer.h"
#include "../utils/config.h"
#include "../utils/log_wrapper.h"
#include "../utils/types.h"
#include "../utils/type_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#define DISK_IO_SIZE 50000000
typedef struct{
//	format::vertex_t id;
	format::weight_t update_value;
}update_t;

template <class update_type >
class update_array: public std::vector<update_type >{
private:

public:
	update_array(){}
	
	virtual void push_update( update_type update){

		this -> push_back(update);
	}
	
};

template <class update_type>
class engine {
protected:
	int max_loop;
	int step_counter;
	//std::string config_file;
	std::string filename;
	dx_lib::buffer<BYTE> *disk_io;
	int vertex_num;
	int edge_num;
	int type;
	int edge_size;
	char buf[1000000];
	int offset;
	int edge_num_once;
	int byte_num_once;
	int readed_bytes;
	int updated_num;
	update_array<update_type > ua;
	//update_type update_buf[200];
public:
	
	engine (std::string fn, int mloop){
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
		ua.clear();
		format::config conf;
		std::stringstream record;
		if( !conf.open_fill(filename + ".ini")){
			LOG_TRIVIAL(fatal)<< "can not open config file "
								<<filename + ".ini";
			exit(0);
		}
		record.clear();
		record<< conf["vertices"];
		record>> vertex_num;

		record.clear();
		record<< conf["type"];
		record>> type;

		record.clear();
		record<< conf["edges"];
		record>> edge_num;
		disk_io = new dx_lib::buffer<BYTE>(DISK_IO_SIZE);
		
	}
	~ engine(){
		delete disk_io;
	}	
	virtual	void next_super_step() {
		step_counter ++;
	}
	
	virtual int super_step(){
		return step_counter ;
	}	
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
				return false;  //there isn't more bytes in the buffer
			}
		}
		else{
			return false;	//means that file is fully written in the buffer
		}
	
	}

	//return means that a computing in a 
	//super step is convergence or not
	//virtual bool compute_once () = 0;
	virtual void scatter() = 0;
	virtual bool gatter() = 0;	
	virtual void output() = 0;
	virtual void run(){
		while ( !reach_max_step() ){
			disk_io -> start_write(filename);
			LOG_TRIVIAL(info)<<"iterator "<<super_step();
			scatter();
			disk_io -> write_join();		
			if ( gatter() ){
				break;
			}
			next_super_step();
		}
		output();
	}
	
};
#endif

