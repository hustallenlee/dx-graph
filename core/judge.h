#ifndef _JUDGE_
#define _JUDGE_
#include "../utils/ini_config.h"
#include <string>

template <class update_type>
class judgement{

protected:
	int min_id;
	int max_id;
public:
	judgement(std::string ini_filename ){
		format::ini_file ifile(ini_filename);
		min_id = ifile.get_value<long >("machines", "min_id");
		max_id = ifile.get_value<long >("machines", "max_id");
				
	}
	virtual int test(update_type update) = 0;
};

#endif
