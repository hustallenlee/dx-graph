#ifndef _UPDATE_STREAM_
#define _UPDATE_STREAM_
#include "buffer.h"
#define TEMPSIZE 2000
namespace dx_lib{
	
	template <class elem_type>
	class update_stream : public buffer<elem_type>{
	protected:
		//bool write_is_over;		
		elem_type temp_update_snd[TEMPSIZE];
    	elem_type temp_update_rev[TEMPSIZE];
    	int pointer_snd;
    	int pointer_rev;


	public:
		update_stream(): buffer<elem_type>::buffer(){
			//write_is_over = false;
			pointer_snd = 0;
			pointer_rev = 0;
		}

		update_stream(unsigned int size): buffer<elem_type>::buffer(size){
			//write_is_over = false;
			pointer_snd = 0; //point to the next of the end element
			pointer_rev = 0; //point to the next of the end element
		}

		void write(elem_type *buf, int size){
			//over = 0;
            //read_count = 0;
            //write_count = 0;
        	int total = size;
        	int temp = size;
        	while(temp){
                boost::mutex::scoped_lock lock(buffer_mutex);
                //std::cout<<"total "<<total<<"front rear "<<front <<" "<< rear <<std::endl;
                temp -= this-> enqueue(buf + total - temp, temp);
                //temp = temp - write_num;
                //write_count += write_num;
                //std::cout<<"write "<<total - temp << " byte(s)"<<std::endl;
            }
        }

		/*int read(elem_type * buf, int n){
            int total = n;
            int temp = n;
            while(temp){
                boost::mutex::scoped_lock lock(buffer_mutex);
                int read_num = dequeue(buf + total - temp, temp);
                temp = temp - read_num;
                //if (read_num == 0){
                //  std::cout<<"temp "<<n- temp<<" over "<< over << " empty "<<is_empty()<<std::endl;
                //}
                read_count += read_num;
                if (over == 1 && is_empty())
                    break;
                //std::cout<<"read "<<total - temp << " byte(s)"<<std::endl;
            }
            return total - temp ;
        }*/
		
		void set_write_over(){
            boost::mutex::scoped_lock lock(buffer_mutex);
			this -> over = 1;
		}

		void add_update(elem_type update){
            
			//the temp buffer is not full, add to the temp buffer
        	if (pointer_snd !=  TEMPSIZE ){
            	temp_update_snd[pointer_snd] = update;
            	pointer_snd ++;
        	}
			else{
            	write(temp_update_snd, TEMPSIZE);
            	pointer_snd = 0;
				temp_update_snd[pointer_snd] = update;
				pointer_snd ++;
			}
    	}
		
		bool get_update(elem_type &update){
			int readed_num = 0;

        	if (pointer_rev != 0){
            	update = temp_update_rev[pointer_rev] ;
            	pointer_rev --;
            	return true;
        	}
        	else{	//the rev buffer is empty, then get the data from the stream
				
            	readed_num = this -> read(temp_update_rev, TEMPSIZE);
				if (readed_num == 0){ //means that the write is over
					//std::cout<<"hehe"<<std::endl;
					return false;
				}
				else{
            		pointer_rev = readed_num;
					update = temp_update_rev[pointer_rev];
					pointer_rev --;
					return true;
				}
        	}
    	}

		void reset(){
			this->over = 0;
			this->read_count = 0;
			this->write_count = 0;
		}
	};
}
#endif
