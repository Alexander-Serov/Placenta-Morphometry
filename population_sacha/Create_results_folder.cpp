
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <string>
// #include <cstdlib>		// for `system`

// #include <stdio.h>
// #include "Global_variables.h"




// Platform-specific commands
#ifdef _WIN32
	#define DIRECTORY_SEPARATOR "\\"
	#define DIRECTORY_SEPARATOR_SEARCH "/\\"
	#define REMOVE_FOLDER_COMMAND "rmdir /S /Q "
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
	#define REMOVE_FOLDER_COMMAND "rm -rf "
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

// External
extern const bool bl_clear_results_folder;
extern char* mainDir;
extern const int FILENAME_LENGTH;


using namespace std;



void create_results_folder(char *fileInDir, char *resultDir   )
{
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
	string defaultFolder= "";
	defaultFolder= defaultFolder + mainDir + "results" + DIRECTORY_SEPARATOR;
	copy(defaultFolder.begin(), defaultFolder.end(), resultDir);

//throw 20;


	DIR *dir2;
	dir2 = opendir(resultDir);


// Clearing the results folder if necessary
	if(dir2!=NULL)
	{
		closedir(dir2);
		if (bl_clear_results_folder)
		{
			ss_tmp.clear();
			ss_tmp.str(string());
			ss_tmp<<REMOVE_FOLDER_COMMAND<<resultDir;
			s_tmp = ss_tmp.str();
			copy(s_tmp.begin(), s_tmp.end(), tmpStr);
			tmpStr[s_tmp.size()] = '\0';
			system(tmpStr);
		}
	}
	else // (Re-)Creating the results folder if missing
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

}









