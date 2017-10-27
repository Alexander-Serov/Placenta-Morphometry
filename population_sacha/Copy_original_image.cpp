

#include <string>		// std::string
#include <sstream>		// std::stringstream
#include "Global_variables.h"


// Platform-specific commands
#ifdef _WIN32
	#define SHELL_COPY_COMMAND "copy"
#elif __linux__|| defined __APPLE__ || __APPLE__
	#define SHELL_COPY_COMMAND "cp"
#endif


// using namespace std;








void copy_original_image(const std::string img_In
						 /*const char *resultDir,
						 const string img_name_withoutExtension,
						 const string extension*/
						 )
{
	char* tmpStr = new char [FILENAME_LENGTH];


	//ss_tmp.clear();
	std::stringstream ss_tmp;
	ss_tmp<<SHELL_COPY_COMMAND<<" "<<img_In<<" "<<resultDir<<str_img_name_withoutExtension<<"_00_Original_image"<<extension;
	std::string s_tmp = ss_tmp.str();
	std::copy(s_tmp.begin(), s_tmp.end(), tmpStr);
	tmpStr[s_tmp.size()] = '\0';
	system(tmpStr);

}




