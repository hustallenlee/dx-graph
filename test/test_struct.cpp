#include <iostream>
//#pragma pack(1)
typedef struct {
	char a;
	int b;
} test;
//#pragma pack()
int main() {
	test *t;
	char c[10] = {'7','1','2','3','4','5','6','7','8','9'};
	t = (test *)c;
	std::cout<<sizeof(test)<<" "<<sizeof(float) <<" "<<sizeof(unsigned int)<<std::endl;
	//std::cout.setf(std::ios::hex);
	std::cout<< t->a <<" "<<std::hex<< t->b<<std::endl;
	//std::cout.unsetf(std::ios::hex);
	
}
