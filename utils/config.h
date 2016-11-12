/*
 *
 * author:      allen lee(Junhao Li) allen_lee922@foxmail.com
 * address:     WNLO, Huazhong University of Science & Technology
 * Time:        2016.11
 *
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
#include <bits/stdc++.h>
#include <cassert>
#include "log_wrapper.h"
#define NKEY 4
namespace format {
	//typedef struct{
	//	std::string key;
	//	std::string value;
	//} kv;
	class config{
    public:
        typedef std::map<std::string, std::string> _KV;
        
	private:
		std::string config_filename;
		_KV conf;
        std::string keys[NKEY];
	public:

		explicit config(std::string config_fname){
            assert(config_fname != "");
            keys[0] = "type";
            keys[1] = "name";
            keys[2] = "vertices";
            keys[3] = "edges";
			load(config_fname);
		}

		bool load(std::string config_fname){
			config_filename = config_fname;
            std::string line;

            std::ifstream config_file(config_filename);
			if (!config_file){
                LOG_TRIVIAL(error)<<"config file can not be opened for read";
				return false;
			}

            //std::cout<<"gou test"<< std::endl;
            while(getline(config_file,line)){

                if ( line !="" && line.find_first_of('#') == std::string::npos){
                    std::istringstream record(line);
                    std::string str;
                    for(int i=0;i<NKEY;i++){
                        record >> str;
                        record.clear();
                        conf[keys[i]]=str;
                    }
                    break;
                }
            }
            config_file.close();
			return true;
		}	

		bool save(){
            std::ofstream config_file(config_filename);
            if(!config_file){
                    LOG_TRIVIAL(error)<<"config file can not be opened for write";
                    return false;
            }
            else{
                config_file<<"#";
                for(int i = 0; i< NKEY; i++){
                    if(i != (NKEY-1)){
                        config_file<<keys[i]<<" ";
                    }
                    else{
                        config_file<<keys[i]<<std::endl;
                    }
                }
                
                for(int i = 0; i< NKEY; i++){
                    if(i != (NKEY-1)){
                        config_file<<conf[keys[i]]<<" ";
                    }
                    else{
                        config_file<<conf[keys[i]]<<std::endl;
                    }
                }
            }
            return true;
		}
        ~config(){
            save();
        }
		void show(){
            for(int i=0;i<NKEY;i++){
			    std::cout<< keys[i]<<": "<< conf[keys[i]]<<std::endl;
            }
		}
		/*std::string &read(std::string key){
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
		}*/
		/*void write(){
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
		}*/

		std::string &operator[](std::string key){
			return conf[key];
		}
	};
}
#endif
