#include <ctime>
#include <iostream>
#include <string>
#include "../utils/log_wrapper.h"
#include "../utils/types.h"
#include "../utils/buffer.h"
#include "../utils/type_utils.h"
int main(int argc, char * argv[]){
	dx_lib::buffer disk_io(1000*1000*50);
	time_t start = clock();	
	std::cout<< "here"<<std::endl;
	std::string filename = argv[1];
	char buf[1000000];
	int readed_bytes = 0;
	int edge_num_once = 100000;      //read 1000 edges once a time
    int edge_size = sizeof(format::edge_t);
    int byte_num_once = edge_num_once * edge_size;
	disk_io.start_write(filename);
	format::edge_t edge;
	while( !disk_io.is_over()){         //the first scan, get every vertice's degree
        readed_bytes = disk_io.read(buf, byte_num_once);
    //  std::cout<<"readed "<< readed_bytes<<std::endl;
        int i = readed_bytes;
        while( i ){
            format::format_utils::read_edge(buf + readed_bytes - i, edge);

            //std::cout<<"test "<<sizeof(edge)<<std::endl;  
            i -= edge_size;
    //  std::cout<<"test "<< i<<std::endl;
            //std::cout<<"here2"<<std::endl;    
        }
    }
	time_t end = clock();
	std::cout<< double(end -start)/CLOCKS_PER_SEC<< std::endl;
}

