




#include"Population.h"
#include<iostream>
#include<string>
#include"Global_variables.h"
#include<errno.h>




// Platform-specific commands
#ifdef _WIN32
	#include"direntvc.h"
	#include"population_sacha\Log_steps.h"	
	#include"population_sacha\Statistics_placenta.h"
#elif __linux__
	#include"dirent.h"
	#include"population_sacha/Log_steps.h"
	#include"population_sacha/Statistics_placenta.h"
#endif




using namespace std; //std namespace // cin/cout, string







void create_dot_finished_file(const string resultDir,
							  const string img_name_withoutExtension
							  )

{
	// Local variables
	stringstream ss_tmp;
	char *tmpStr;


	// Initialization
	tmpStr = new char[FILENAME_LENGTH];

	ss_tmp.clear();
	ss_tmp.str(string());
	ss_tmp<<resultDir<<img_name_withoutExtension<<".finished";
	string finished_file_string=ss_tmp.str();
	std::copy(finished_file_string.begin(), finished_file_string.end(), tmpStr);
	tmpStr[finished_file_string.size()] = '\0';
	ofstream finished_file_stream;
	finished_file_stream.open(tmpStr);
	finished_file_stream.close();

}




