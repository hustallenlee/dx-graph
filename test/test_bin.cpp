#include <iostream>
#include <fstream>
#include "../utils/type_utils.h"
#include "../utils/types.h"
int main(int argc, char * argv[]){
	format::edge_t edge;
	char buf[1200];
	int i = 0;
	std::ifstream in(argv[1],std::ios::in  | std::ios::binary);
	in.read(buf,1200);
	for (i=0; i<1200; i = i + sizeof(format::edge_t)){
		format::format_utils::read_edge(buf + i,edge);
		std::cout<< edge.src<< " "  << edge.dst << " " <<edge.value<<std::endl;
	}
	return 0;

}
