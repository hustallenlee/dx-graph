#include <iostream>
#include "../utils/config.h"

int main(){
	std::string str("./sw.bin.ini");
	format::config conf(str);
	conf.show();
	std::cout<< conf["name"]<< std::endl;
	conf["name"]="123.bin";
	conf.show();
	conf.write();
	//std::cout<< conf["name"]<< std::endl;	
	return 0;
}
