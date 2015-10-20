#include "buffer.h"
#include "config.h"
#include "log_wrapper.h"
#include "types.h"
#include "type_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>
#define COMPACT_GRAPH

typedef struct{
	int degree;
	weight_t res[2];
}array;

int main(int argc,char * argv[]){
	if(argc != 3){
		std::cout<<"Usage: execute_file filename iterator_times "
					<< std::endl;
		return 0;
	}
	std::stringstream record;
	std::string filename;
	int times = 0;
	int vertex_num = 0;
	int edge_num = 0;
	int type=1;
	char buf[30];
	array init_array;
	init_array.degree = 0;
	init_array.res[0] = 0.0;
	init_array.res[1] = 0.0;


	record << argv[1];
	record >> filename;	
	record << argv[2];
	record >> times;
	format::config conf(filename + ".ini");
	
	if (conf){
		LOG_TRIVIAL(fatal)<< "can not open config file "
							<<filename + ".ini";
		return 0;
	}
	record<< conf["vertices"];
	record>> vertex_num;	
	record<< conf["type"];
	record>> type;

	std::bitset<vertex_num> update_set;
	std::vector<array > aux_array(vertex_num, init_array); //auxiliary array
	int	edge_size = sizeof(format::edge_t);
	int flag = 1;   //indicate which is the old
	format::edge_t edge;
	int update_num = 0;
	

	
	dx_lib::buffer disk_io(1000*1000*50);
	
	disk_io.start_write(filename);
	while( !disk_io.is_over()){			//the first scan, get every vertice's degree
		disk_io.read(buf,edge_size);
		format::format_utils::read_edge(buf, edge);
		aux_array[edge.src].degree += 1;
		aux_array[edge.src].res[flag] = edge.value;
		
	}
	disk_io.write_join();
	
	while (times){
		disk_io.start_write(filename);
		while( !disk_io.is_over()){			//the second scan, scatter and gather phase
			disk_io.read(buf,edge_size);
			format::format_utils::read_edge(buf, edge);	
			
			aux_array[edge.dst].res[1-flag] += 
		 	0.85 * aux_array[edge.src].res[flag] / aux_array[edge.src].degree;
			
		}

		disk_io.write_join();
		for(auto iter = aux_array.begin(); iter!=aux_array.end(); iter++){
			if ( fabs(iter->res[flag] - iter->res[1-flag]) >0.0001){
				update_num ++;
				iter->update = 1;
			}
			else{
				iter->update = 0;
			}
		}
		flag = (flag == 0? 1:0);
		time --;
	}
	
}
