#include "../utils/buffer.h"
#include "../utils/config.h"
#include "../utils/log_wrapper.h"
#include "../utils/types.h"
#include "../utils/type_utils.h"
#include "../core/engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <iomanip>

typedef struct{
    int degree;
    format::weight_t res[2];
}array;

class pagerank: public engine{
private:
	char buf[1000000];
	array init_array;
	format::weight_t init_weight;
	std::vector<bool> *update_bitset;
    std::vector<array > *aux_array; //auxiliary array
    int edge_size;
    int flag;   //indicate which is the old

public:
	pagerank(std::string fn, int mloop): engine(fn, mloop){
		init_array.degree = 0;
    	init_array.res[0] = init_weight;
    	init_array.res[1] = init_weight;
		init_weight = 0.15 / vertex_num;
		update_bitset = new std::vector<bool>(vertex_num, false);
    	aux_array = new std::vector<array>(vertex_num, init_array); //auxiliary array
    	edge_size = sizeof(format::edge_t);
    	flag = 1;   //indicate which is the old
	}

	~ pagerank(){
		delete update_bitset;
		delete aux_array;
	}
	void scatter() {
		init_read();
		format::edge_t edge;
		while( get_next_edge(edge) ){
					
		}
				
	

	}
	
	bool gatter(){
		
	}
	
};

int main(int argc, char * argv[]){
	 if(argc != 3){
        std::cout<<"Usage: execute_file filename iterator_times "
                    << std::endl;
        return 0;
    }
	std::stringstream record;
	std::string filename;
	int niter;

	record.clear();
	record<< argv[1];
	record>> filename;

	record.clear();
	record<< argv[2];
	record>> niter;

	pagerank pr(filename, niter);
	pr.run();
	return 0;
}
