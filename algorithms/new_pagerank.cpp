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
    format::weight_t res;
}array;

class pagerank: public engine<update_t >{
private:
	char buf[1000000];
	array init_array;
	format::weight_t init_weight;
	std::vector<bool> *update_bitset;
    std::vector<array > *aux_array; //auxiliary array
    int edge_size;
    int flag;   //indicate which is the old
	format::weight_t temp;
public:
	pagerank(std::string fn, int mloop): engine(fn, mloop){

		init_weight = 0.15 / vertex_num;
		init_array.degree = 0;
    	init_array.res = init_weight;

		update_bitset = new std::vector<bool>(vertex_num, false);
    	aux_array = new std::vector<array>(vertex_num, init_array); //auxiliary array
    	edge_size = sizeof(format::edge_t);

		ua.resize(vertex_num,{0.0});
    	flag = 1;   //indicate which is the old
	}

	~ pagerank(){
		delete update_bitset;
		delete aux_array;
	}
	void scatter() {
		//init_read();
		LOG_TRIVIAL(info)<<"scatter ...";
		format::edge_t edge;
		if (super_step() == 0){//get every vertex's degree
			while( get_next_edge(edge) ){
				(*aux_array)[edge.src].degree += 1;
				
			}
		}
		else{
			while( get_next_edge(edge) ){
				if ((*update_bitset)[edge.dst ] == false){
					ua[edge.dst].update_value +=
                    	(*aux_array)[edge.src].res / (*aux_array)[edge.src].degree;

				}
			}
		}
	}

	bool gatter(){
		LOG_TRIVIAL(info)<<"gatter ...";
		if (super_step() !=0){
			int pos = 0;
			for (auto iter = ua.begin(); iter != ua.end(); iter ++){
				pos = iter - ua.begin();
				temp =0.15/init_weight + 0.85 * (iter->update_value) ;
				if ((*update_bitset)[ pos ] == false){
					if (fabs(temp - (*aux_array)[pos].res ) < 0.000000001){
						(*update_bitset)[pos] = true;
						updated_num ++ ;
					}
				}
			}
			LOG_TRIVIAL(info)<<vertex_num - updated_num<<" / "<<vertex_num;
        	if ( updated_num == vertex_num ){ //all bits are 1
            	LOG_TRIVIAL(info) << "convergence and exit";
            	return true;
        	}
			return false;
		}
		return false;
	}
	void output(){
    	std::ofstream out("output.csv", std::ios::out);
    	auto begin = (*aux_array).begin();
    	for (auto iter = begin; iter != (*aux_array).end(); iter++){
        	out << iter -begin<<" "
            	<<std::fixed<<std::setprecision(16)
            	<<iter -> res<<std::endl;
    	}
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
