
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>		// for `system`
#include "Global_variables.h"




// Platform-specific commands
#ifdef _WIN32
	#define DIRECTORY_SEPARATOR "\\"
	#define DIRECTORY_SEPARATOR_SEARCH "/\\"
/*
	#include"direntvc.h"
	#include <direct.h>
    #define getcwd _getcwd
	#include"population_sacha\Choose_input_folder.h"
	#include"population_sacha\Create_dot_finished_file.h"
	#include"population_sacha\Create_results_folder.h"
	#include"population_sacha\Discriminate_capillaries_from_IVS.h"
	#include"population_sacha\Fill_holes_in_the_villi.h"
	#include"population_sacha\Initial_segmentation.h"
	#include"population_sacha\Load_placenta_picture.h"
	#include"population_sacha\Log_steps.h"
	#include"population_sacha\Remove_noise.h"
	#include"population_sacha\Small_functions.h"
	#include"population_sacha\Statistics_placenta.h"
	#define SHELL_COPY_COMMAND "copy"
//	#define DEFAULT_RESULTS_DIRECTORY ".//results//"
*/
#else
	#define DIRECTORY_SEPARATOR "/"
	#define DIRECTORY_SEPARATOR_SEARCH "/"
/*
	#include"dirent.h"
	#include"unistd.h"
	#include"population_sacha/Choose_input_folder.h"
	#include"population_sacha/Create_dot_finished_file.h"
	#include"population_sacha/Create_results_folder.h"
	#include"population_sacha/Discriminate_capillaries_from_IVS.h"
	#include"population_sacha/Fill_holes_in_the_villi.h"
	#include"population_sacha/Initial_segmentation.h"
	#include"population_sacha/Load_placenta_picture.h"
	#include"population_sacha/Log_steps.h"
	#include"population_sacha/Remove_noise.h"
	#include"population_sacha/Small_functions.h"
	#include"population_sacha/Statistics_placenta.h"
	#define SHELL_COPY_COMMAND "cp"
//	#define DEFAULT_RESULTS_DIRECTORY ".\results\"
*/
#endif



using namespace std;







void create_results_folder(char *fileInDir,
						   char *resultDir
						   )
{
	// Local variables
	string s_tmp;
	char *tmpStr;
	//string fileResult;
	stringstream ss_tmp;


	// Initialization
	tmpStr = new char [FILENAME_LENGTH];

	string fileResult (fileInDir);
	unsigned found = fileResult.find_last_of(DIRECTORY_SEPARATOR_SEARCH);
	fileResult=fileResult.substr(0,found);
	found = fileResult.find_last_of(DIRECTORY_SEPARATOR_SEARCH);
	fileResult=fileResult.substr(0,found);
	fileResult=fileResult+DIRECTORY_SEPARATOR+"results"+DIRECTORY_SEPARATOR;
	/*s_tmp="/";
	size_t found1 = fileResult.find(s_tmp);
	while(found1!=string::npos)
	{
		fileResult.replace(fileResult.find(s_tmp),s_tmp.length(),DIRECTORY_SEPARATOR);
		found1 = fileResult.find(s_tmp);
		cout<<"It's a loop.";
	}*/
	copy(fileResult.begin(), fileResult.end(), resultDir);
	resultDir[fileResult.size()] = '\0';

	//throw 20;

	DIR *dir2;
	dir2 = opendir(resultDir);

	// Clearing the results folder if necessary
	if(dir2!=NULL && bl_clear_results_folder)
	{
		closedir(dir2);
		ss_tmp.clear();
		ss_tmp.str(string());
		ss_tmp<<REMOVE_FOLDER_COMMAND<<resultDir;
		s_tmp = ss_tmp.str();
		copy(s_tmp.begin(), s_tmp.end(), tmpStr);
		tmpStr[s_tmp.size()] = '\0';
		system(tmpStr);
	}
	else
	{
		closedir(dir2);	// else just close the reference to the folder
	}

	// (Re-)Creating the results folder if missing
	dir2 = opendir(resultDir);
	if(dir2==NULL)
	{
		//mkdir(resultDir);
		ss_tmp.clear();
		ss_tmp.str(string());
		ss_tmp<<"mkdir "<<resultDir;
		s_tmp = ss_tmp.str();
		copy(s_tmp.begin(), s_tmp.end(), tmpStr);
		tmpStr[s_tmp.size()] = '\0';
		system(tmpStr);
	}

	cout<<"Results are output into: "<<endl<<resultDir<<endl<<endl;
	closedir(dir2);

}









