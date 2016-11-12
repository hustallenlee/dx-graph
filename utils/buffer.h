/*
 * author:      allen lee(Junhao Li) allen_lee922@foxmail.com
 * address:     WNLO, Huazhong University of Science & Technology
 * Time:        2016.11
 *
 * Copyright 2015 Key Laboratory of Data Storage System, Ministry of Education WNLO HUST
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  This namespace dx_lib includes somes utils for our project.
 *  Class buffer is a class that buffers graph
 *
 *
 */
#ifndef _BUFFER_
#define _BUFFER_
#include <iostream>
#include <fstream>
#include <string>
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/thread/condition.hpp>
//#include <thread>
#include <string.h>
#include "log_wrapper.h"
#include "mylock.h"
#define BUFFERSIZE 1000*1000*100

//typedef char BYTE;

namespace dx_lib {






    /*template<class elem_type, 
                unsigned int BUFFERSIZE, 
                unsigned int INPUTSIZE, 
                unsigned int OUTPUTSIZE>
    class buffer_c{
    public:
        typedef char BYTE;
        
    private:
        std::vector<elem_type> data;
        std::vector< std::vector<elem_type> > input;//作为一个整体放入buffer中
        std::vector< std::vector<elem_type> > out;//从buffer中作为一个整体取出
        
    public:
        buffer_c(){
            data.resize(BUFFERSIZE);
        }

    }；*/

	//缓冲区类
    std::mutex buffer_mutex;
	template <class elem_type>
	class buffer {
    public:
        typedef char BYTE;
	    //boost::mutex buffer_mutex;



	protected:
		elem_type *data;    //存放数据数组
		int front;          //有效数据头
		int rear;           //有效数据尾
		const unsigned int len;            //缓冲区长度
		int over;
		int read_count;
		int write_count;
		//boost::thread *thrd;
        std::thread *thrd;
		//boost::mutex over_flag_mutex;
		int elem_size;



	public:
		buffer() :len(BUFFERSIZE),
                    front(0),
                    rear(0),
                    over(0),
                    read_count(0),
                    write_count(0),
                    thrd(NULL){
			//std::cout<<"gou 1"<<std::endl;
			data = new elem_type[BUFFERSIZE]();
			elem_size = sizeof(elem_type);
		}

		buffer(unsigned int size):len(size),
                                    front(0),
                                    rear(0),
                                    over(0),
                                    read_count(0),
                                    write_count(0),
                                    thrd(NULL){
			data = new elem_type[size]();
			elem_size = sizeof(elem_type);
		}
	
		~buffer(){
			delete [] data;
			//delete thrd;
		}

		/*void clear(){
			
			front = 0;
			rear = 0;
			over = 0;
			read_count=0;
			
			delete [] data;
		}*/
		
		//full states
		bool is_full(){
			
			return ((rear + 1) % len == front);
		}

		/*friend std::ostream &operator<< (std::ostream &os, const buffer & buf){
			boost::mutex::scoped_lock lock(buffer_mutex);
			std::cout<<buf.front<<" "<<buf.rear<<std::endl;
			int i=0;
			for (i = buf.front; i != buf.rear; i = (i+1)%(buf.len))
				std::cout<<buf.data[i]<<" ";
			std::cout<<std::endl;
		}*/
		
		//empty states
		bool is_empty(){
			return (front == rear);
		}		
			
		//enqueue the buffer
		int enqueue(elem_type *buf,int n){
			int in_num = n;
			
			if ( is_full() ){
				return 0;
			} 
			if (is_empty()){
				in_num = ( (len - 1) < n ) ? (len - 1) : n;
			}
			else if ( n > (len + front - rear - 1 )%len ){ //n is bigger than empty space
				in_num = (len + front -rear - 1)%len;
			}
			
			if ( (rear + in_num ) <= len){
				memcpy((void * )(data + rear), 
                        (void *)buf ,
                        in_num * elem_size);
			}
			else{
				int temp_num = len - rear;
				memcpy((void *) (data + rear),
                        (void *)buf, 
                        temp_num * elem_size );
				memcpy((void *)data, 
                        (void *)(buf + temp_num), 
                        (in_num - temp_num) * elem_size );
			}
			
			rear = (rear + in_num )%len;
			return in_num;
		}

		//dequeue the buffer
		int dequeue(elem_type * buf, int n){
			int out_num = n ;
			if(is_empty()){
				return 0;
			}
			if (is_full()){
				out_num = ( (len-1) < n )? (len -1) : n;
			}
			else if ( n > (len + rear -front )%len ){  
                //n is bigger than it already has
				out_num = ( len + rear - front )%len;
			}
			
			if ( (front + out_num) <= len){
				memcpy((void *)buf, 
                        (void *)(data + front), 
                        out_num * elem_size);
			}
			else{
				int temp_num = len - front;
				memcpy((void *)buf, 
                        (void *) (data + front), 
                        temp_num * elem_size);

				memcpy((void *)(buf + temp_num), 
                        (void * )data, 
                        (out_num - temp_num) * elem_size);
			}
			front = (front + out_num) % len;
			return out_num;
		}
		
		//write the file into the buffer 
		//can be used in non-BYTE buffer
		int write( std::string filename ){
			std::ifstream infile( filename,std::ios::in | std::ios::binary);
            if(!infile){
                LOG_TRIVIAL(error)<<"The file can not be opened";
                return 0;
            }
			const int size = 100;
			elem_type buf[size];
			int total;
			while((total = infile.read((char *)buf, elem_size * size).gcount())){
				//std::cout<<"total "<<total<<std::endl;
				//std::cout<<"write "<<buf<<std::endl;
				int temp = total;
				
				while(temp){
					//boost::mutex::scoped_lock lock(buffer_mutex);
                    //buffer_mutex.lock();
                    
                    //scope lock
                    dx_lib::scoped_lock lock(buffer_mutex);
					
                    
                    int write_num = enqueue(buf + total - temp, temp);
					temp = temp - write_num;
					write_count += write_num;
					//buffer_mutex.unlock();
				}
				if(infile.eof()){
					//LOG_TRIVIAL(info)<<"file "<<filename << " has being written into the buffer";
					//boost::mutex::scoped_lock lock(buffer_mutex);
                    //scope lock
                    dx_lib::scoped_lock lock(buffer_mutex);
					over = 1;
                    //buffer_mutex.unlock();
				}
				
			}
			//LOG_TRIVIAL(info)<< "write thread has finished total write "<< write_count << " bytes";
			return write_count;
		}


		int read(elem_type * buf, int n){
			int total = n;
			int temp = n;
			while(temp){
				//boost::mutex::scoped_lock lock(buffer_mutex);
                //buffer_mutex.lock();
                dx_lib::scoped_lock lock(buffer_mutex);
				int read_num = dequeue(buf + total - temp, temp);
				temp = temp - read_num; 
				read_count += read_num; 
				if (over == 1 && is_empty())
					break;
                //buffer_mutex.unlock();
			}
			return total - temp ;
		}
		
		void start_write(std::string filename){
			reset();
			//auto f = boost::bind(&buffer::write, this, filename);
            auto f = std::bind(&buffer::write, this, filename);
			//LOG_TRIVIAL(info) << "starting write the buffer using file "<< filename;
			//thrd = new boost::thread(f);
            thrd = new std::thread(f);
		}


		void write_join(){
			if (thrd){
				if (thrd->joinable() ){
					thrd->join();
				}
				delete thrd;
				thrd = NULL;
			}
		}

		bool is_over(){
			//boost::mutex::scoped_lock lock(buffer_mutex);
			//std::cout<<"in buffer is_over"<<std::endl;	
            dx_lib::scoped_lock lock(buffer_mutex);
			return (over == 1) && (is_empty()) ;
		}

		void reset(){
			over = 0;
			read_count = 0;
			write_count = 0;
			write_join();
		}
	};
}	
#endif

