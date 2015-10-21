/*
 * dist-xs
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
#ifndef _CONFIG_
#define _CONFIG_
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace format {
	typedef struct{
		std::string key;
		std::string value;
	} kv;
	class config{
	private:
		std::string filename;
		kv conf[4];
		std::string str_null;

	public:
		config(){
			str_null = "";
		}
		config(std::string config_filename){
			open_fill(config_filename);
		}
		bool open_fill(std::string config_filename){
			filename = config_filename;
            str_null = "";
            char line[1024];
            std::string str;

            std::ifstream config_file(filename);
			if (!config_file){
				return false;
			}

            //std::cout<<"gou test"<< std::endl;
            while(config_file.getline(line, sizeof(line))){
                str = line;

                if ( str !="" && str.find_first_of('#') == std::string::npos){
                    std::istringstream record(line);
                    record >> conf[0].value;
                    //std::cout<< conf[0].value << std::endl;

                    record >> conf[1].value;
                    record >> conf[2].value;
                    record >> conf[3].value;
                    break;
                }
            }
            config_file.close();
			return true;
		}	
		//~config(){
		//	if (config_file){
		//		config_file.close();
		//	}
		void show(){
			std::cout<< "type: "<< conf[0].value<<std::endl;
			std::cout<< "name: "<< conf[1].value<<std::endl;
			std::cout<< "vertices: "<< conf[2].value<<std::endl;
			std::cout<< "edges: "<< conf[3].value<<std::endl;
		}
		std::string &read(std::string key){
				if (key == "type")
					return conf[0].value;

				else if (key == "name"){
					//std::cout<<"test"<< conf[1].value<<std::endl;
					return conf[1].value;
				}

				else if (key == "vertices")
					return conf[2].value;

				else if( key == "edges"){
					return conf[3].value;
				}
				else 
					return str_null;
		}
		void write(){
			char line[1024];
			int i=0;
			std::ifstream config_file(filename);
			config_file.getline(line, sizeof(line));
			config_file.close();

			std::ofstream write_config_file(filename);
			//std::cout<<line<<std::endl;
			write_config_file<< std::string(line)<< std::endl;
			for (i=0; i<4; i++){
				write_config_file<< conf[i].value<<" ";
			}
			write_config_file.close();
		}

		std::string &operator[](std::string key){
			return read(key);
		}
	};
}
#endif
