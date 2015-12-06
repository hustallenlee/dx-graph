#ifndef _FILTER_
#define _FILTER_
#include <zmq.hpp>
#include <cstdlib>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "types.h"
#include "update_stream.h"
#include "log_wrapper.h"
#include "net_stream.h"
#include "ini_config.h"
#include "../core/judge.h"
#define RECVSIZE 10000
#define SENDSIZE 10000
namespace dx_lib{
	template <class elem_type>	
	class multi_update_stream: public update_stream<elem_type>{
	
	protected:
		elem_type update_from_net[RECVSIZE];
		int pointer_net;
	public:
		multi_update_stream(): update_stream<elem_type >::update_stream(){
			pointer_net = 0;
		}

		multi_update_stream(unsigned int size)
			: update_stream<elem_type >::update_stream(size){
			pointer_net = 0;
		}

		void add_update_from_net(elem_type update, bool immediate){
			//the temp buffer is not full, add to the temp buffer

            if (pointer_net !=  RECVSIZE && immediate != 0){
                update_from_net[pointer_net] = update;
                pointer_net ++;
            }
            else{
                write(update_from_net, RECVSIZE);
                pointer_net = 0;
                update_from_net[pointer_net] = update;
                pointer_net ++;
            }
		}
	};

	template <class update_type>
	class filter{
		
	private:
		
		//if an update belongs to the self node, 
		//it will be added into update_buf
		update_stream<update_type> *update_buf;

		//multi_write_update_buf is an update buffer 
		//that update from net and update from self
		//can be added into this buffer
		//also, filter can get the update from this buffer
		multi_update_stream<update_type> *multi_write_update_buf;

		//ns is a net stream, while an update go through the filter,
		//if an update belongs to other machine, it will be send to ns
		dx_lib::net_stream *ns;

		//judgement is a tool, judge whether an update belongs to self machine
		judgement<update_type> *judge;
		//update_type update[SENDSIZE];
		//int pointer;
		boost::thread * thrd;
		unsigned long offset;
	public:
		filter(update_stream<update_type>        *update_buffer,
			   multi_update_stream<update_type>  *multi_write_update_buffer,
			   net_stream                        *net_io,
			   judgement<update_type>            *ju,
				unsigned long					 ofse
				){

			update_buf = update_buffer;
			multi_write_update_buf = multi_write_update_buffer;
			ns = net_io;
			judge = ju;
			offset = ofse;
			//pointer = 0;
			thrd = NULL;
		}
		~filter(){
			delete thrd;
		}
		
		//run(): ge update from multi buffer, test the update, send to local,
		// or send to network	
		void run(){
			update_type update;
			int update_size =  sizeof (update_type);

			//get an update
			while(multi_write_update_buf->get_update( update )){

				if (judge->test(update)){//true, add to the update buffer
					update.id = update.id - offset;
					update_buf->add_update(update);
				}
				else{
					//if(pointer == SENDSIZE){//indicate that the send buf is full 

					//}
					ns -> send((char *) (&update), update_size);
				}
			}//while
		}//run

		//start the thread
		void start_filt(){
			auto f = boost::bind(&filter::run, this);		
			thrd = new boost::thread(f);
		}

		//wait the thread end
		void join_filter(){
			if(thrd){
				if(thrd -> joinable() ){
					thrd -> join();	
				}		
			}
		}
	};//filter
	

}
#endif
