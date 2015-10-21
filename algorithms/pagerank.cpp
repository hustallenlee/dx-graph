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
//#include <bitset>
//#define COMPACT_GRAPH

typedef struct{
	int degree;
	format::weight_t res[2];
}array;
void output(std::vector<array > &aux_array,int flag);
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

	record.clear();
	record << argv[1];
	record >> filename;	
	
	record.clear();
	record << argv[2];
	record >> times;

	format::config conf;
	if ( !conf.open_fill(filename + ".ini")){
		LOG_TRIVIAL(fatal)<< "can not open config file "
							<<filename + ".ini";
		return 0;
	}
	record.clear();
	record<< conf["vertices"];
	record>> vertex_num;	

	record.clear();
	record<< conf["type"];
	record>> type;

	std::vector<bool> update_bitset(vertex_num, false);
	std::vector<array > aux_array(vertex_num, init_array); //auxiliary array
	int	edge_size = sizeof(format::edge_t);
	int flag = 1;   //indicate which is the old
	format::edge_t edge;
	int update_num = 0;
	

	
	dx_lib::buffer disk_io(1000*1000*50);
	std::cout<<"here1 "<<times<<std::endl;	

	disk_io.start_write(filename);
	while( !disk_io.is_over()){			//the first scan, get every vertice's degree
		disk_io.read(buf,edge_size);
		format::format_utils::read_edge(buf, edge);
	//std::cout<<"test "<<sizeof(edge)<<std::endl;	
		aux_array[edge.src].degree += 1;
	//std::cout<<"here2"<<std::endl;	
		aux_array[edge.src].res[flag] = edge.value;
		
	}
	disk_io.write_join();
		
	while (times){
		disk_io.start_write(filename);
		std::cout<<"here2"<<times<<std::endl;	
		while( !disk_io.is_over()){			//the second scan, scatter and gather phase
			//std::cout<<"test"<<times<<std::endl;	
			disk_io.read(buf,edge_size);
			format::format_utils::read_edge(buf, edge);	
			
			if (update_bitset[ edge.dst ] == false ){	
				//if the vertice is not converged, then update
				aux_array[edge.dst].res[1-flag] += 		//new
		 		0.85 * aux_array[edge.src].res[flag] / aux_array[edge.src].degree;
			}
			//else{ //else copy the pagerank value to the new res
			//	aux_array[edge.dst].res[1-flag] = aux_array[edge.dst].res[flag];
			//}
		}

		disk_io.write_join();
		int pos = 0;
		for(auto iter = aux_array.begin(); iter != aux_array.end(); iter++, pos++){
			if ( update_bitset[ pos ] == false ){ 		//updated
				if ( fabs(iter->res[flag] - iter->res[1-flag]) <0.0001){ //convergence
					update_bitset[ pos ] = true;
					//update_num ++;
				}
				iter->res[flag] = iter->res[1-flag]; //update the old
			}
		}
		auto iter2 = update_bitset.begin();
		for (iter2 = update_bitset.begin(); iter2 != update_bitset.end(); iter2++){
			if ((*iter2) == false){
				break;
			}
		}
		if ( iter2 == update_bitset.end() ){ //all bits are 1
			break;
		}
		times --;
	}
	output(aux_array, 1);
	
}
void output(std::vector<array > &aux_array, int flag){
	std::ofstream out("output.csv", std::ios::out);
	auto begin = aux_array.begin();
	for (auto iter = aux_array.begin(); iter != aux_array.end(); iter++){
		out<<iter -begin<<" "<<iter -> res[flag]<<std::endl;
	}
}
