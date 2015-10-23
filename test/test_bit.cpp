#include <vector>
#include <iostream>
int main(){
	std::vector<bool> bitset(10,false);
	int i=0;
	bitset.at(5) = true ;
	for (i=0; i<10; i++){
		std::cout<<i<<" "<< bitset[i]<<std::endl;
	}
	return 0;
}
