#include "../core/control.h"
#include "../utils/log_wrapper.h"
#include <unistd.h>
int main(int argc, char * argv[]){
	controller_client client("./machine.ini");
	int type;
	int i =0;
	while(1){
		type = client.recv_msg();
		int flag = 0;
		switch(type){
			case 1:
				LOG_TRIVIAL(info)<<"received start message";
				break;
			case 2:
				LOG_TRIVIAL(info)<<"received go on message";
				break;
			case 3:
				LOG_TRIVIAL(info)<<"received end  message";
				flag = 1;
				break;
			default:
				LOG_TRIVIAL(info)<<"the message can not be recongnized";
				break;
		}
		if (flag){
			LOG_TRIVIAL(info)<<"end";
			break;
		}	
		client.send_msg_to_server(i, 0);
		i++;
	}


	return 0;
}
