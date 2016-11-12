#include <iostream>
#include "../utils/config.h"

int main(int argc, char** argv){
	std::string str(argv[1]);
	format::config conf(str);
	conf.show();
	std::cout<< conf["name"]<< std::endl;
	conf["name"]="123.bin";
	conf.show();
	//std::cout<< conf["name"]<< std::endl;	
	return 0;
}
