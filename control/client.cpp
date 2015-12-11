#include "../core/control.h"
#include "../utils/log_wrapper.h"
#include <unistd.h>
int main(int argc, char * argv[]){
	controller_client client("./machine.ini");
	int type;
	while(1){
		type = client.recv_msg();
		int flag = 0;
		switch(type){
			case 1:
				LOG_TRIVIAL(info)<<"received start message";
				sleep(10);
				break;
			case 2:
				LOG_TRIVIAL(info)<<"received go on message";
				sleep(5);
				break;
			case 3:
				LOG_TRIVIAL(info)<<"received end  message";
				sleep(5);
				break;
			default:
				LOG_TRIVIAL(info)<<"the message can not be recongnized";
				flag = 1;
				break;
		}
		if (flag){
			LOG_TRIVIAL(info)<<"end";
			break;
		}
	
	}


	return 0;
}
