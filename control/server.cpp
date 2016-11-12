#include "../core/control.h"

#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char * argv[]){
	std::string str(argv[1]);
	std::stringstream ss(str);
	int niter;
	ss >> niter;
	controller_server server(niter);

	server.start();	

	return 0;
}
