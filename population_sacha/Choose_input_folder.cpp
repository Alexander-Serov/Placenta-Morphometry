

#include"Global_variables.h"
#include<iostream>
#include<sstream>
#include<string.h>
#include "Log_steps.h"


// Platform-specific commands
#ifdef _WIN32
	#define DIRECTORY_SEPARATOR "\\"

	#include"direntvc.h"		// For working with folders
	#include<direct.h>			// For working with folders
	#define getcwd _getcwd		// Get current working directory
/*

	#include <direct.h>
    
	*/
#elif __linux__
	#define DIRECTORY_SEPARATOR "/"

	#include"dirent.h"		// For working with folders
	#include"unistd.h"		// Analog of <direct.h>
/*
	#include"dirent.h"
	#include"unistd.h"

	*/
#endif


using namespace std;




void choose_input_folder(	char*& fileInDir,
							char*& mainDir, 
							int argc,
							char *argv[]
							)
{
	// Local variables
	char *tmpStr;

	string s_tmp;

	stringstream ss_tmp;
	
	// Initialization
	tmpStr = new char [FILENAME_LENGTH];

	// Determining the main directory
	getcwd(tmpStr, FILENAME_LENGTH);
	s_tmp = tmpStr;
	s_tmp += DIRECTORY_SEPARATOR;
	// Replacing backslashes with slashes for linux
	size_t start_pos=0;
	while((start_pos = s_tmp.find("\\", start_pos)) != std::string::npos) 
	{
		s_tmp.replace(start_pos, 1, "/");
		start_pos += 1;
	}
	copy(s_tmp.begin(), s_tmp.end(), mainDir);
	mainDir[s_tmp.size()] = '\0';


	if (argc==1)
	{
		cout<<"Input folder can be specified as a command line argument";
		s_tmp = "E:\\05_Modifying_Zhenzhens_project\\population\\mes_images\\in\\";
		copy(s_tmp.begin(), s_tmp.end(), fileInDir);
		fileInDir[s_tmp.size()] = '\0';
	}
	else
	{
		fileInDir=argv[1];
		ss_tmp.clear();
		ss_tmp.str(string());
	}		
	cout<<"Loading input images from: "<<endl<<fileInDir<<endl<<endl;


}