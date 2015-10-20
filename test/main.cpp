#include <unistd.h>
#include <fstream>
#include "../utils/buffer.h"
//using namespace dx_lib;
#define SIZE 1000
int main()
{
	dx_lib::buffer bf(1000*1000);
	std::string str = "a.bin";
	BYTE buf[SIZE] = {'b','b','b'};
	int i=0;
	std::ofstream fout("c.bin",std::ios::binary);
	bf.start_write(str);
	//bf.write(str);
	sleep(2);
	//std::cout<<bf;
	while (!bf.is_over()){
		int read_num = bf.read(buf,1000);
		//std::cout<<bf;
		//std::cout<<std::endl;
		fout.write(buf, read_num);
	}
/*	bf.read(buf,1)
;

	std::cout<<bf;
	bf.read(buf,1);

	std::cout<<bf;
	bf.read(buf,1);

	std::cout<<bf;
	bf.read(buf,1);

	std::cout<<bf;
	bf.read(buf,1);*/

	bf.write_join();
	//bf.dequeue(buf, 4);
	//std::cout<<buf<<std::endl;
	return 0;

}
