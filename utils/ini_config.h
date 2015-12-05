#ifndef _ini_config_
#define _ini_config_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>
#include <sstream>

namespace format{
	class ini_file{
		
	private:
		std::string filepath;
		//boost::property_tree::ptree root;
		//boost::property_tree::ptree items;
		template <class type>
		void _put(std::string section, std::string key, type value){
			boost::property_tree::ptree pt;
			boost::property_tree::ini_parser::read_ini(filepath, pt);	
			std::string path = section + "." + key;
			pt.put(path, value);
			boost::property_tree::ini_parser::write_ini(filepath, pt);
		}
		
		template<class type>
		void _add(std::string section, std::string key, type value){
			boost::property_tree::ptree pt;
            std::string path = section + "." + key;
            pt.add(path, value);
            boost::property_tree::ini_parser::write_ini(filepath, pt);

		}

	public:
		ini_file(std::string file_path){
			filepath = file_path;
		}
		
		template <class type>
		type get_value(std::string section, std::string key ){
			boost::property_tree::ptree pt;
			boost::property_tree::ini_parser::read_ini(filepath, pt);
			std::string path = section + "." + key;
			return pt.get<type>(path);
		}
		
	
		void write_value(std::string section, std::string key, std::string value){
			_put<std::string>(section, key, value);
		}
		
		void write_value(std::string section, std::string key, int value){
			_put<int>(section, key, value);
		}

		void write_value(std::string section, std::string key, float value){
			_put<float>(section, key, value);
		}

		void write_value(std::string section, std::string key, double value){
			_put<double>(section, key, value);
		}

		void add_value(std::string section, std::string key, std::string value){
			_add<std::string>(section, key, value);
		}

		void add_value(std::string section, std::string key, double value){
			_add<double>(section, key, value);
		}

		void add_value(std::string section, std::string key, int value){
			_add<int>(section, key, value);
		}

		void add_value(std::string section, std::string key, float value){
			_add<float>(section, key, value);
		}
		
		//std::string &operator	
		
	};//end ini file
}//end format
#endif
