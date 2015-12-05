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
	format::weight_t temp;
}array;

typedef struct{
	format::vertex_t id;
    format::weight_t update_value;
}update_t;

class pagerank: public engine<update_t >{
private:
	//char buf[1000000];
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
    	init_array.temp = 0;

		update_bitset = new std::vector<bool>(vertex_num, false);
    	aux_array = new std::vector<array>(vertex_num, init_array); //auxiliary array
    	edge_size = sizeof(format::edge_t);
		
		//ua.resize(vertex_num,{0,0.0});
    	//flag = 0;   //flag means that result
	}

	~ pagerank(){
		delete update_bitset;
		delete aux_array;
	}
	void scatter() {
		//init_read();
		LOG_TRIVIAL(info)<<"scatter ...";
		format::edge_t edge;
		update_t update;

		if (super_step() == 0){//get every vertex's degree
			while( get_next_edge(edge) ){
				(*aux_array)[edge.src].degree += 1;			
			}
		}
		else{
			while( get_next_edge(edge) ){
				if ((*update_bitset)[edge.dst ] == false){
					
					update.id = edge.dst;
					update.update_value =
                    	(*aux_array)[edge.src].res / (*aux_array)[edge.src].degree;
					add_update(update);
				}
			}
		}
	}

	bool gather(){
		LOG_TRIVIAL(info)<<"gatter ...";
		update_t update;
		format::weight_t temp;

		if (super_step() != 0){
			//int pos = 0;
			while (get_update(update)){
				//std::cout<<update.id<<" "<<update.update_value<<std::endl;
				if ((*update_bitset)[ update.id ] == false){
					//update the new array
					(*aux_array)[update.id].temp += update.update_value ;

				}
			}
			
			for (auto iter = aux_array->begin(); iter != aux_array->end(); iter++){
				temp = init_weight + 0.85 * iter->temp;
				if (fabs((*aux_array)[update.id].res - temp ) < 0.000000001){

					(*update_bitset)[update.id] = true;
					updated_num ++ ;	
				}	
				else{
					(*aux_array)[iter - aux_array->begin() ].res = temp;
				}
				iter -> temp =0.0;
			}
			LOG_TRIVIAL(info)<<"gather "<<vertex_num - updated_num<<" / "<<vertex_num;
        	if ( updated_num == vertex_num ){ //all bits are 1
            	LOG_TRIVIAL(info) << "convergence and exit";
				set_convergence();
            	return true;
        	}
			return false;
		}
		return false;
	}
	void output(){
		std::cout<<"here"<<std::endl;
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
