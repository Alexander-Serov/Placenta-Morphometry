#include"Population.h"
#include "core/algorithm/GeometricalTransformation.h"
#include<iostream>
#include<string>
#include"Global_variables.h"
#include<errno.h>
#include "Log_steps.h"	
#include "Statistics_placenta.h"

//#include<fstream>
//#include<sys/types.h>
//#include<errno.h>
//#include<algorithm>
//#include<utility>  
//#include<ctime>
//#include"CImg.h"


// Platform-specific commands
#ifdef _WIN32
	#include"direntvc.h"
#elif __linux__
	#include"dirent.h"
#endif


using namespace std; //std namespace // cin/cout, string




int load_placenta_picture(string& img_name_withoutExtension,
						   const string& img_name,
						   string& img_In,
						   string& lock,
						   string& extension,
						   const char *fileInDir,
						   const char *resultDir,
						   pop::Mat2RGBUI8& subImg,
						   bool& processed_a_new_picture)
{
	// Local variables
	string finished_file_name;
	string lock_file_name;
	vector <string> nameFileLock;
	stringstream ss_tmp;
	char *tmpStr = new char[FILENAME_LENGTH];
	
	// Selecting a file in the input folder
	
	size_t place;

	log_steps("Entering loading procedure", "general");
	
	img_name_withoutExtension=img_name;
	if(img_name.compare(img_name.size()-5,5,".lock")==0)
	{
		place = img_name.find(".lock");
		img_name_withoutExtension = img_name_withoutExtension.erase(place,img_name_withoutExtension.length()-1);
		nameFileLock.push_back(img_name_withoutExtension);
		return 0;
	}
	else if(img_name.compare(img_name.size()-4,4,".png")==0)//2 string equal if compare==0
	{
		place = img_name.find(".png");
		extension=".png";
		img_name_withoutExtension = img_name_withoutExtension.erase(place,img_name_withoutExtension.length()-1);
	}
	else if(img_name.compare(img_name.size()-4,4,".jpg")==0)
	{
		place = img_name.find(".jpg");
		extension=".jpg";
		img_name_withoutExtension = img_name_withoutExtension.erase(place,img_name_withoutExtension.length()-1);
	}
	else if(img_name.compare(img_name.size()-4,4,".tif")==0)//2 string equal if compare==0
	{
		place = img_name.find(".tif");
		extension=".tif";
		img_name_withoutExtension = img_name_withoutExtension.erase(place,img_name_withoutExtension.length()-1);
	}
	else if(img_name.compare(img_name.size()-5,5,".tiff")==0)//2 string equal if compare==0
	{
		place = img_name.find(".tiff");
		extension=".tiff";
		img_name_withoutExtension = img_name_withoutExtension.erase(place,img_name_withoutExtension.length()-1);
	}
	
	log_steps("Accessing the results folder", "general");
	try{
		// Verifying in the results folder whether the slected input picture needs to be processed
		DIR *dir2 = opendir(resultDir); 
		closedir(dir2);
		if(dir2==NULL)
		{
			cout<<"Error opening results folder"<<endl<<endl;
			log_steps("Error opening results folder");
			log_steps("Error opening the results folder", "general");
			return 0;
		}
	}
	catch (const std::exception& e) 
	{
	//	log_steps("Encountered exception: " + (string)e.what() + ". Error number: "+strerror(errno), "general");
		return 0;
	}
	catch (...) 
	{
		log_steps("Caught an unknown exception", "general");
		return 0;
	}
						
	log_steps("Verifying the presence of a .lock file", "general");
	// Verifying whether this picture is locked by another application instance
	lock_file_name=resultDir+img_name_withoutExtension+".lock";
	if (FILE *temp_file = fopen(lock_file_name.c_str(), "r")) 
	{
		fclose(temp_file);
		return 0;
	}

	log_steps("Verifying the presence of a .finished file", "general");
	// Verifying whether this picture has been already processed
	finished_file_name=resultDir+img_name_withoutExtension+".finished";
	if (FILE *temp_file = fopen(finished_file_name.c_str(), "r")) 
	{
		fclose(temp_file);
		return 0;
	}

	log_steps("Creating a .lock file", "general");
	processed_a_new_picture=true;
	// Creating a .lock file
	ss_tmp.clear();
	ss_tmp.str(string());
	ss_tmp<<resultDir<<img_name_withoutExtension<<".lock";
	lock=ss_tmp.str();
	std::copy(lock.begin(), lock.end(), tmpStr);
	tmpStr[lock.size()] = '\0';
	ofstream file_lock;
	file_lock.open(tmpStr);
	file_lock.close();

	// ===================================
	// Loading the picture
	log_steps("Loading subImage");
	log_steps("Loading the picture itself", "general");
	img_In = fileInDir + img_name; 
	try
	{															
		subImg.load(img_In); 

		// // Mirror and rotate image
		// subImg = pop::GeometricalTransformation::mirror(subImg, 0);

	}
	catch (exception &e)
	{
		e.what();
		cout<<img_name<<" doesn't exist !"<<endl<<endl;
		copy(lock.begin(), lock.end(), tmpStr);
		tmpStr[lock.size()] = '\0';
		remove(tmpStr);
		log_steps("Encountered an excpetion during the loading process. Exception description: " + (string)e.what(), "general");
		return 0;
	}
	log_steps();
	return 1;
}