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
#include <climits>
typedef struct{
   format::vertex_t label;
}array;

class wcc: public engine<format::vertex_t >{
private:
	std::vector<bool> *update_bitset;
    std::vector<array > *aux_array; //auxiliary array
    int edge_size;
    //int flag;   //indicate which is the old
	//format::weight_t temp;
public:
	wcc(std::string fn, int mloop): engine(fn, mloop){


		update_bitset = new std::vector<bool>;
    	aux_array = new std::vector<array>(vertex_num); //auxiliary array
    	edge_size = sizeof(format::edge_t);
	
		ua.resize(vertex_num, UINT_MAX);
		
		//for (auto iter = aux_array->begin(), iter != aux_array->end(); iter ++){
		//	iter->label = iter -aux_array->begin();
		//}
	}

	~ wcc(){
		delete update_bitset;
		delete aux_array;
	}
	void scatter() {
		//init_read();
		LOG_TRIVIAL(info)<<"scatter ...";
		format::edge_t edge;
		format::vertex_t min;
		format::vertex_t src;
		format::vertex_t dst;

		if (super_step() == 0){//set every vertex's id

			for (auto iter = aux_array->begin(); iter != aux_array->end(); iter ++){
				iter->label = iter - aux_array->begin();
			}
		}
		while( get_next_edge(edge) ){
			src = (*aux_array)[edge.src].label;
			dst = (*aux_array)[edge.dst].label;
				
			min = src < dst ? src : dst ;
			ua[edge.src] = min < ua[edge.src] ? min : ua[edge.src]; 
			ua[edge.dst] = min < ua[edge.dst] ? min : ua[edge.dst];
        }
	}

	bool gatter(){
		LOG_TRIVIAL(info)<<"gatter ...";
		format::vertex_t updated_num = 0;
		auto start = ua.begin();
		int pos = 0;

		for (auto iter = ua.begin(); iter != ua.end(); iter ++){
			pos = iter - start;
			if ( (*aux_array)[pos].label > (*iter) ){
				(*aux_array)[pos].label = (*iter);
				updated_num ++;
			}
		}
		LOG_TRIVIAL(info)<<updated_num<<" / "<<vertex_num;
        if ( updated_num == 0 ){ //all bits are 1
           	LOG_TRIVIAL(info) << "convergence and exit after "
								<<super_step() 
								<< " iterations";
           	return true;
        }
		
		return false;
	}
	void output(){
    	std::ofstream out("output.csv", std::ios::out);
    	auto begin = (*aux_array).begin();
    	for (auto iter = begin; iter != (*aux_array).end(); iter++){
        	out << iter -begin<<" "
            	<<std::fixed<<std::setprecision(16)
            	<<iter -> label<<std::endl;
    	}
    	/*auto begin = (*aux_array).begin();
    	for (auto iter = begin; iter != (*aux_array).end(); iter++){
        	out << iter -begin<<" "
            	<<iter -> degree<<std::endl;
    	}*/
		
	}       
};

int main(int argc, char * argv[]){
	 if(argc != 2){
        std::cout<<"Usage: execute_file filename "
                    << std::endl;
        return 0;
    }
	std::stringstream record;
	std::string filename;

	record.clear();
	record<< argv[1];
	record>> filename;
	
	wcc cc(filename,INT_MAX);
	cc.run();
	return 0;
}
